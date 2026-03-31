#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include <QListWidget>
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include "WindowManager.h"
#include "ReceiveFile.h"

extern QString gLoginEmployeeID;		//全局变量，保存当前登录员工的ID，可以在其他类中使用，例如在获取员工ID列表时可以排除当前登录员工的ID等
QString gfileName;		//文件名称
QString gfileData;		//文件数据

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏	
	initControl();
	initTcpSocket();
	initUdpSocket();

	QStringList employeesIDList;
	getEmployeesID(employeesIDList);		//获取员工ID列表，可以根据实际需求进行修改，例如从数据库中获取员工ID列表，或者从文件中读取员工ID列表等
	QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (!file.size())	//如果文件不存在或者文件大小为0，说明JS文件不存在或者为空，需要创建JS文件
	{
		if (!createJSFile(employeesIDList))	//调用创建JS文件的函数，传入员工ID列表，如果创建成功则继续执行，否则弹出提示信息框，告知用户创建失败
		{
			QMessageBox::information(this, "提示", "更新JS文件失败！");		//如果创建JS文件失败，弹出提示信息框，告知用户创建失败
		}
	}
	

}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;		//删除表情窗口对象，释放内存
	m_emotionWindow = NULL;		//将表情窗口指针置空，避免悬空指针
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem ,const QString& uid/*, GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow);		//将新的聊天窗口添加到右侧的堆叠窗口中
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);		//创建一个新的聊天窗口列表项
	m_talkwindowItemMap.insert(aItem, talkWindow);		//将聊天窗口列表项和对应的聊天窗口添加到映射关系中

	aItem->setSelected(1);		//设置新添加的聊天窗口列表项为选中状态，表示当前显示的聊天窗口

	//判断是群聊还是私聊
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();
	if (rows == 0)
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}
	QModelIndex index = sqlDepModel.index(0, 0);
	QImage img;
	img.load(sqlDepModel.data(index).toString());	//图片地址

	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));	//设置聊天窗口列表项的头像图片，可以根据实际需求传入不同的图片路径或资源
	ui.listWidget->addItem(aItem);		//将新的聊天窗口列表项添加到左侧的聊天窗口列表中
	ui.listWidget->setItemWidget(aItem, talkWindowItem);		//将聊天窗口列表项设置为新的聊天窗口列表项对象，显示在聊天窗口列表中

	onTalkWindowItemClicked(aItem);		//调用聊天窗口列表项被点击的槽函数，切换到新添加的聊天窗口，显示该聊天窗口的内容

	connect(talkWindowItem, &TalkWindowItem::signalCloseBtnClicked, [talkWindowItem, talkWindow,aItem,this]() {		//连接聊天窗口列表项的关闭按钮被点击的信号和一个lambda函数，当关闭按钮被点击时执行这个函数，删除对应的聊天窗口和聊天窗口列表项
		m_talkwindowItemMap.remove(aItem);		//从映射关系中删除对应的聊天窗口列表项和聊天窗口的映射关系
		talkWindow->close();		//关闭对应的聊天窗口，触发窗口的关闭事件，执行相应的清理操作，例如释放资源等
		ui.listWidget->takeItem(ui.listWidget->row(aItem));	//从聊天窗口列表中移除对应的聊天窗口列表项，更新聊天窗口列表的显示
		delete talkWindowItem;		//删除聊天窗口列表项对象，释放内存
		ui.rightStackedWidget->removeWidget(talkWindow);		//从右侧的堆叠窗口中移除对应的聊天窗口，更新堆叠窗口的显示
		if (ui.rightStackedWidget->count() < 1)
			close();
		});

}

void TalkWindowShell::setCurrentTalkWindow(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);		//将右侧的堆叠窗口切换到指定的聊天窗口，显示该聊天窗口的内容
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap()
{
	return m_talkwindowItemMap;		//返回聊天窗口列表项和对应的聊天窗口的映射关系，供外部使用，例如在其他类中获取当前聊天窗口等操作
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");		//加载样式表，设置窗口的外观和风格
	setWindowTitle("聊天窗口");		//设置窗口标题
	
	m_emotionWindow = new EmotionWindow(this);		//创建表情窗口对象，并将当前窗口作为父窗口
	m_emotionWindow->hide();		//默认隐藏表情窗口

	QList<int> leftWidgetSize;	//创建一个整数列表，用于设置左侧控件的大小
	leftWidgetSize << 154 << width() - 154;		//将左侧控件的大小设置为154像素，右侧控件的大小为窗口宽度减去154像素
	ui.splitter->setSizes(leftWidgetSize);	//将设置好的大小列表应用到分割器控件上，调整左右控件的大小比例
	
	ui.leftWidget->setStyle(new CustomPoxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	//连接聊天窗口列表项被点击的信号和槽函数

	connect(m_emotionWindow,SIGNAL(signalEmotionItemClicked(int)), this,SLOT(onEmotionBtnClicked(int)));
	//连接表情窗口的表情项被点击的信号和槽函数，当表情项被点击时会触发这个槽函数，执行相应的操作，例如将表情添加到当前聊天窗口中等
}

void TalkWindowShell::onEmotionBtnClicked(int emotionNum)
{
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());		//获取当前显示的聊天窗口，并将其转换为TalkWindow类型的指针
	if (curTalkWindow)
	{
		curTalkWindow->addEmotionImage(emotionNum);		//如果当前聊天窗口存在，则调用其addEnotionImage函数，添加表情图片到聊天窗口中
	}

}
/**********************************************************************************************
数据包格式
文本数据包格式：群聊标志+发信息人ID+收消息人（群）ID+消息类型（1）+数据长度+文本数据
表情数据包格式：群聊标志+发信息人ID+收消息人（群）ID+消息类型（0）+数据个数+images+表情名称
文件数据包格式：群聊标志+发信息人ID+收消息人（群）ID+消息类型（2）+文件字节数+bytes+文件名+data_begin+文件数据

群聊标志占1位：1表示群聊，0表示私聊
信息类型占1位：0表示表情，1表示文本，2表示文件

ID号占5位，群ID占4位，数据长度占5位，表情名称占3位
（注意：当群聊标志为1，则数据包中没有收信息员工ID而是收消息群ID
		当群聊标志为0，则数据包中没有收信息群ID而是收消息员工ID）

群聊文本信息如：1 10001 2001 1 00005 Hello
				0 10004 10001 0 1 images 006
				1 10001 2001 2 10 bytes test.txt data_begin helloworld
************************************************************************************************/
void TalkWindowShell::processPendingData()
{
	while (m_udpReceicer->hasPendingDatagrams())		//检查UDP套接字中是否有待处理的数据报，如果有则继续处理，否则退出函数
	{
		const static int groupFlagWidth = 1;		//群聊标志占1位
		const static int groupWidth = 4;			//群ID占4位
		const static int employeeWidth = 5;		//员工ID占5位
		const static int msgTypeWidth = 1;			//消息类型占1位
		const static int msgLengthWidth = 5;		//文本数据长度占5位
		const static int pictureWidth = 3;		//表情占3位


		QByteArray btData;		//创建一个字节数组对象，用于存储接收到的数据报内容
		btData.resize(m_udpReceicer->pendingDatagramSize());		//调整字节数组的大小，以适应接收到的数据报的大小
		m_udpReceicer->readDatagram(btData.data(), btData.size());		//读取数据报内容，并将其存储在字节数组中，使用readDatagram函数可以同时获取数据报的内容和发送者的地址等信息，可以根据实际需求进行处理，例如根据发送者的地址判断是来自哪个员工或群等

		QString strData = btData.data();		
		QString strWindowID;	//聊天窗口ID，可以是员工ID或者群ID，根据群聊标志的不同而不同
		QString strSendEmployeeID, strRecevieEmployeeID;	//发送员工ID和接收员工ID，根据群聊标志的不同而不同
		QString strMsg;		//数据
		int msgLen;			//数据长度
		int msgType;			//消息类型，0表示表情，1表示文本，2表示文件

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);		//从数据字符串中提取发送员工ID，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID
		//自己发的消息不处理
		if (strSendEmployeeID == gLoginEmployeeID)
		{
			return;		//如果发送员工ID与当前登录员工ID相同，说明是自己发的消息，不需要处理，直接返回函数
		}
		if (btData[0] == '1')
		{
			//如果是群聊，则提取群ID作为聊天窗口ID
			strWindowID = strData.mid(groupFlagWidth+employeeWidth, groupWidth);		
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];		//从数据字符串中提取消息类型，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID
			if (cMsgType == '1')	//文本信息
			{
				msgType = 1;		//设置消息类型为文本
				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();		//从数据字符串中提取文本数据长度，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth, msgLen);		//从数据字符串中提取文本数据，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID
			}
			else if (cMsgType == '0')	//表情信息
			{
				msgType = 0;		//设置消息类型为表情
				int posImage = strData.indexOf("images");		//在数据字符串中查找表情标志"images"，获取其位置
				strMsg = strData.right(strData.length() - posImage - QString("images").length());		//从数据字符串中提取表情数据，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID

			}
			else if (cMsgType == '2')	//文件信息
			{
				msgType = 2;		//设置消息类型为文件'
				int bytesWidth = 5;		//文件字节数占5位
				int posBytes = strData.indexOf("bytes");		//在数据字符串中查找文件标志"bytes"，获取其位置
				int posDataBegin = strData.indexOf("data_begin");		//在数据字符串中查找文件数据开始标志"data_begin"，获取其位置
				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posDataBegin - posBytes - bytesWidth);
				gfileName = fileName;		//将文件名称保存到全局变量中，供其他函数使用，例如在接收文件数据时可以使用这个文件名称来保存文件等

				//文件数据
				int dataLengthWidth;
				int posData = posDataBegin + QString("data_begin").length();		//文件数据开始位置
				strMsg = strData.mid(posData);	//从这里开始后面都是文件数据
				gfileData = strMsg;		//将文件数据保存到全局变量中，供其他函数使用，例如在接收文件数据时可以使用这个文件数据来保存文件等

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery queryGroupName(QString("SELECT employee_name FROM tab_employees WHERE employID = %1").arg(employeeID));
				queryGroupName.exec();
				
				if (queryGroupName.next())
				{
					sender = queryGroupName.value(0).toString();		//获取查询结果中的员工姓名，作为发送者姓名
				}
				//接收文件的后续操作
				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
					});
				QString msgLabel = QString("收到来自%1发送的文件 %2，是否接受？").arg(sender).arg(fileName);		//构造提示信息，告知用户有文件要接收，并询问是否接受
				recvFile->setMsg(msgLabel);
				recvFile->show();		//显示接收文件的窗口，等待用户的操作，例如接受或拒绝文件等
			}
		}
		else		//私聊
		{
			strRecevieEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);		//如果是私聊，则提取接收员工ID作为聊天窗口ID
			strWindowID = strSendEmployeeID;	//窗口ID就是发送者ID

			//判断接收者对象是我吗
			if (strRecevieEmployeeID != gLoginEmployeeID)
			{
				return;		//如果接收员工ID与当前登录员工ID不同，说明消息不是发给自己的，不需要处理，直接返回函数
			}

			//获取信息类型
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];		//从数据字符串中提取消息类型，根据群聊标志的不同而不同，如果是群聊则提取群ID，如果是私聊则提取员工ID
			if (cMsgType == '1')		//文本信息
			{
				msgType = 1;
				//文本数据长度
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();		
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth, msgLen);

			}
			else if (cMsgType == '0')		//表情信息
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.mid(posImages + 6);
			}
			else if (cMsgType == '2')		//文件信息
			{
				msgType = 2;
				int posBytes = strData.indexOf("bytes");
				int bytesWidth = 5;		//文件字节数占5位
				int posDataBegin = strData.indexOf("data_begin");
				int data_beginWidth = 10;		//文件数据开始标志"data_begin"占10位

				QString fileName = strData.mid(posBytes + bytesWidth, posDataBegin - posBytes - bytesWidth);
				gfileName = fileName;		//将文件名称保存到全局变量中，供其他函数使用，例如在接收文件数据时可以使用这个文件名称来保存文件等
				//文件内容
				strMsg = strData.mid(posDataBegin + data_beginWidth);
				gfileData = strMsg;		//将文件数据保存到全局变量中，供其他函数使用，例如在接收文件数据时可以使用这个文件数据来保存文件等

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHERE employID = %1").arg(employeeID));
				querySenderName.exec();

				if (querySenderName.next())
				{
					sender = querySenderName.value(0).toString();		//获取查询结果中的员工姓名，作为发送者姓名
				}

				//接收文件的后续操作
				ReceiveFile* recvFile = new ReceiveFile(this);
				connect(recvFile, &ReceiveFile::refuseFile, [this]() {
					return;
					});
				QString msgLabel = QString("收到来自%1发送的文件 %2，是否接受？").arg(sender).arg(fileName);		//构造提示信息，告知用户有文件要接收，并询问是否接受
				recvFile->setMsg(msgLabel);
				recvFile->show();		//显示接收文件的窗口，等待用户的操作，例如接受或拒绝文件等

			}
		}

		//将聊天窗口设为活动窗口
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget)		//如果找到对应的聊天窗口对象
		{
			this->setCurrentTalkWindow(widget);		//如果找到对应的聊天窗口对象，则将其设置为当前显示的聊天窗口，显示该聊天窗口的内容
			
			//同步激活左侧聊天窗
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(1);
		}
		else	//如果没有找到对应的聊天窗口对象
		{
			return;		
		}
		//文件信息另外处理
		if(msgType != 2)
			handleReceivedMsg(strSendEmployeeID.toInt(), msgType, strMsg);		//调用处理接收到的数据的函数，传入发送员工ID、消息类型和消息内容，根据实际需求进行处理，例如将消息显示在聊天窗口中等

	}
}

void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)
{
	QMsgTextEdit msgTextEdit;		//创建一个消息文本编辑对象，用于显示接收到的消息内容

	if (msgType == 1)
	{
		msgTextEdit.setText(strMsg);		//将接收到的消息内容设置到消息文本编辑对象中
	}
	else if(msgType == 0)	//表情信息
	{
		const int emotionWidth = 3;		//表情图片的宽度，可以根据实际需求进行调整
		int emotionNum = strMsg.length() / emotionWidth;		//根据表情数据的长度和每个表情占用的宽度，计算出表情的数量
		for (int i = 0; i < emotionNum; i++)
		{
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}
	QString html = msgTextEdit.document()->toHtml();		//将消息文本编辑对象中的文本内容转换为HTML格式，可以根据实际需求进行格式化处理，例如添加发送者姓名、时间戳等信息等

	//文本html如果没有字体则添加,msgFont.txt
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");	//加载消息字体样式文件，例如可以将消息字体样式保存在资源文件中，并在发送消息时加载该文件来设置消息的字体样式等
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();	//读取消息字体样式文件的内容，例如可以将文件内容保存到一个字符串变量中，以便在发送消息时将该样式应用到消息内容中等
			fontHtml.replace("%1", strMsg);	//将消息内容替换到消息字体样式中，例如可以在消息字体样式文件中使用占位符来表示消息内容，并在发送消息时将占位符替换为实际的消息内容等
			file.close();
		}
		else
		{
			QMessageBox::information(this, "提示", "消息字体样式文件加载失败！");	//如果消息字体样式文件加载失败，则显示一个提示信息框，告知用户消息字体样式文件加载失败等
		}
		if (!html.contains(fontHtml))
		{
			html.replace(strMsg, fontHtml);
		}
	}
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());		//获取当前显示的聊天窗口，并将其转换为TalkWindow类型的指针
	//不能直接访问记得在talkwindow.h添加友元
	talkWindow->ui.msgWidget->appendMsg(html,QString::number(senderEmployeeID));		//调用聊天窗口的appendMsg函数，将发送员工ID和消息内容添加到聊天窗口中，显示该消息内容，可以根据实际需求进行格式化处理，例如添加发送者姓名、时间戳等信息等

}


void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());		//切换表情窗口的可见状态，如果当前不可见则显示，反之则隐藏
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));		//获取当前窗口在屏幕上的全局位置，转换为全局坐标系中的点

	emotionPoint.setX(emotionPoint.x() + 170);		//调整表情窗口的水平位置，使其靠右对齐当前窗口
	emotionPoint.setY(emotionPoint.y() + 220);		//调整表情窗口的垂直位置，使其位于当前窗口的下方
	m_emotionWindow->move(emotionPoint);		//将表情窗口移动到计算得到的位置，使其出现在当前窗口的右下方
}
void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	QWidget* talkWindow = m_talkwindowItemMap.find(item).value();		//根据被点击的聊天窗口列表项，从映射关系中获取对应的聊天窗口
	ui.rightStackedWidget->setCurrentWidget(talkWindow);		//根据被点击的聊天窗口列表项，从映射关系中获取对应的聊天窗口，并将右侧的堆叠窗口切换到该聊天窗口，显示该聊天窗口的内容
}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);		//创建一个新的TCP套接字对象
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);	//连接到服务器的IP地址和端口号，可以根据实际需求修改为服务器的IP地址和端口号等
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceicer = new QUdpSocket(this);		//创建一个新的UDP套接字对象
	for (quint16 port = gudpPort; port < gudpPort + 200; ++port)
	{
		if(m_udpReceicer->bind(port,QUdpSocket::ShareAddress))	//尝试绑定UDP套接字到指定的端口号，使用共享地址模式，如果绑定成功则退出循环，否则继续尝试下一个端口号，直到找到一个可用的端口号或者达到最大端口号限制
			break;
	}
	connect(m_udpReceicer, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void TalkWindowShell::getEmployeesID(QStringList& employeesList)
{
	QSqlQueryModel queryModel;		//创建一个新的SQL查询模型对象，用于执行SQL查询并获取结果
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");		//执行SQL查询，获取员工ID列表，可以根据实际需求修改为其他查询语句，例如根据部门ID获取员工ID列表等
	
	int employeesNum = queryModel.rowCount();		//获取查询结果的行数，即员工数量
	QModelIndex index;		//创建一个模型索引对象，用于访问查询结果中的数据
	for (int i = 0; i < employeesNum; i++)
	{
		index = queryModel.index(i, 0);		//获取查询结果中第i行第0列的模型索引，即员工ID所在的列
		employeesList << queryModel.data(index).toString();		//将模型索引中的数据转换为字符串，即员工ID

	}
}

bool TalkWindowShell::createJSFile(QStringList& employeesList)
{
	//读取txt文件数据
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";		//要读取的文本文件路径，可以根据实际需求修改为其他文件路径等
	QFile fileRead(strFileTxt);		//创建一个文件对象，表示要读取的文本文件
	QString strFile;		//创建一个字符串对象，用于存储读取到的文本文件数据
	if (fileRead.open(QIODevice::ReadOnly))
	{
		strFile = fileRead.readAll();		//如果文件成功打开，则读取文件内容，并将其存储在字符串对象中
		fileRead.close();		//关闭文件，释放资源
	}
	else
	{
		QMessageBox::information(this, "提示", "读取msgtmpl.txt文本文件失败！");		//如果文件打开失败，弹出提示信息框，告知用户读取失败
		return false;		//返回false表示创建JS文件失败
	}
	//替换(external0,appendHtml0用作自己发信息使用)
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");		//要写入的JS文件路径，可以根据实际需求修改为其他文件路径等
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate))	//如果文件成功打开，则进行文本替换和写入操作
	{
		//更新空值
		QString strSourceInitNULL = "var external = null;";
		//更新初始化
		QString strSourceInit = "external = channel.objects.external;";
		//更新newWebChannel
		QString strSourceNew = 
			"new QWebChannel(qt.webChannelTransport,function(channel)\
			 {external = channel.objects.external;}\
			);";
		//更新追加recvHtml,脚本中有双引号无法直接进行赋值，采用读文件方式
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resources/MainWindow/MsgHtml/recvHtml.txt");		//要读取的文本文件路径，可以根据实际需求修改为其他文件路径等
		if (fileRecvHtml.open(QIODevice::ReadOnly))
		{
			strSourceRecvHtml = fileRecvHtml.readAll();		//如果文件成功打开，则读取文件内容，并将其存储在字符串对象中
			fileRecvHtml.close();		//关闭文件，释放资源
		}
		else
		{
			QMessageBox::information(this, "提示", "读取recvHtml.txt文本文件失败！");
			return false;		//如果文件打开失败，弹出提示信息框，告知用户读取失败，并返回false表示创建JS文件失败
		}

		//保存替换后的脚本
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < employeesList.length(); i++)
		{
			//编辑替换后的空值
			QString strInitNull = strSourceInitNULL;
			strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInitNull += strInitNull + "\n";

			//编辑替换后的初始化
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceInit += strInit + "\n";

			//编辑替换后的newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strReplaceNew += strNew + "\n";

			//编辑替换后的追加recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeesList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
			strReplaceRecvHtml += strRecvHtml + "\n";

		}

		strFile.replace(strSourceInitNULL, strReplaceInitNull);		//将文本文件中的空值替换为编辑后的空值
		strFile.replace(strSourceInit, strReplaceInit);		//将文本文件中的初始化替换为编辑后的初始化
		strFile.replace(strSourceNew, strReplaceNew);		//将文本文件中的newWebChannel替换为编辑后的newWebChannel
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);		//将文本文件中的追加recvHtml替换为编辑后的追加recvHtml
		
		QTextStream stream(&fileWrite);		//创建一个文本流对象，并将其与要写入的文件对象关联起来，方便进行文本写入操作
		stream << strFile;		//将替换后的文本文件内容写入到JS文件中，完成JS文件的创建和更新
		fileWrite.close();		//关闭文件，释放资源

		return true;		//返回true表示创建JS文件成功
	}
	else
	{
		QMessageBox::information(this, "提示", "写msgtmpl.txt文件失败！");
		return false;		//如果文件打开失败，弹出提示信息框，告知用户读取失败，并返回false表示创建JS文件失败
	}
}



//客户端发送Tcp数据（数据，数据类型，文件）
//文本数据包格式：群聊标志 + 发信息员工ID + 收消息员工ID(群ID) + 信息类型 + 数据长度 + 数据
//表情数据包格式：群聊标志 + 发信息员工ID + 收消息员工ID(群ID) + 信息类型 + 表情个数 + 数据
//msgType: 0表情信息，1文本信息，2文件信息
void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
{
	//获取当前活动聊天窗口
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());	//获取当前显示的聊天窗口，并将其转换为TalkWindow类型的指针
	QString talkId = curTalkWindow->getTalkId();	//获取当前聊天窗口的群聊标志

	QString strGroupFlag;
	QString strSend;
	if (talkId.length() == 4)		//如果聊天窗口ID长度为4，说明是群聊窗口，群聊标志为1，否则为0
		strGroupFlag = "1";
	else
		strGroupFlag = "0";

	//int dataLength = strData.length();		//获取文本数据的字节长度，使用UTF-8编码方式计算长度，确保中文字符等多字节字符的长度计算正确
	//const int sourceLength = dataLength;		//文本信息的长度约定为5位，如果数据长度不足5位，则需要在数据前面补0，保证数据包格式的一致性
	int nstrDataL = strData.length();	//获取文本数据的字节长度，使用UTF-8编码方式计算长度，确保中文字符等多字节字符的长度计算正确
	int dataLength = QString::number(nstrDataL).length();//文本信息的长度约定为5位，如果数据长度不足5位，则需要在数据前面补0，保证数据包格式的一致性
	QString strdataLength;

	if (msgType == 1)	//发送文本信息
	{
		//文本信息的长度约定为5位
		if(dataLength == 1)	//hello等文本信息长度为1字节时，数据长度部分需要补0，保证数据包格式的一致性
			strdataLength = "0000" + QString::number(nstrDataL);
		else if (dataLength == 2)
			strdataLength = "000" + QString::number(nstrDataL);
		else if (dataLength == 3)
			strdataLength = "00" + QString::number(nstrDataL);
		else if (dataLength == 4)
			strdataLength = "0" + QString::number(nstrDataL);
		else if (dataLength == 4)
			strdataLength =QString::number(nstrDataL);
		else
		{
			QMessageBox::information(this, "提示", "文本信息长度超过约定长度限制，无法发送！");		//如果文本信息长度超过约定长度限制，弹出提示信息框，告知用户发送失败，并返回
		}
		//文本数据包格式：群聊标志 + 发信息员工ID + 收消息员工ID(群ID) + 信息类型 + 数据长度 + 数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;		//构造要发送的文本数据包，按照约定的格式进行拼接，确保数据包格式的一致性
	}

	else if (msgType == 0)	//发送表情信息
	{
		//表情数据包格式：群聊标志 + 发信息员工ID + 收消息员工ID(群ID) + 信息类型 + 表情个数 + images + 数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;		//构造要发送的表情数据包，按照约定的格式进行拼接，确保数据包格式的一致性
	}
	else if (msgType == 2)
	{
		//文件数据包格式：群聊标志 + 发信息员工ID + 收消息员工ID(群ID) + 信息类型 + 文件长度 + "bytes" + 文件名 + "data_begin" + 文件数据
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName + "data_begin" + strData;		//构造要发送的文件数据包，按照约定的格式进行拼接，确保数据包格式的一致性
	}

	QByteArray dataBt;
	dataBt.resize(strSend.length());
	dataBt = strSend.toUtf8();		//将要发送的数据包转换为UTF-8编码的字节数组，确保数据在网络传输中的编码一致性
	m_tcpClientSocket->write(dataBt);		//通过TCP套接字发送数据包，将字节数组写入到套接字中，完成数据的发送操作

}
