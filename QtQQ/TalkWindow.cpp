#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include <QToolTip>
#include <QFile>
#include <QMessageBox>
TalkWindow::TalkWindow(QWidget *parent, const QString& uid, GroupType groupType)
	: QWidget(parent), m_talkId(uid), m_groupType(groupType)	
	//初始化成员变量，例如将用户ID和群组类型保存到成员变量中，以便在后续的操作中使用，例如在设置聊天窗口标题时可以根据群组类型来设置不同的标题等
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId, this);	//将当前聊天窗口的用户ID和窗口对象添加到窗口管理器中，以便在需要获取聊天窗口对象时可以通过用户ID来查找对应的窗口对象，并进行相应的操作，例如显示聊天窗口、关闭聊天窗口等
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏
	initControl();	//初始化控件，设置聊天窗口的控件属性和布局，例如设置消息输入框的占位文本、设置发送按钮的图标等
	
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindow(m_talkId);	//将当前聊天窗口的用户ID从窗口管理器中移除，以便在窗口关闭时能够正确地清理窗口对象，并避免内存泄漏等问题
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionUrl(emotionNum);	//调用消息输入框的添加表情图片URL的函数，将表情编号传递给消息输入框，以便在消息输入框中添加对应的表情图片等
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);	//设置聊天窗口标题标签的文本为传入的名称，例如可以将用户的昵称、群组名称等信息作为聊天窗口的标题，方便用户识别当前聊天窗口的内容和对象等

}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();	//获取被双击的树形控件项的数据，判断该项是根项还是子项，例如可以通过设置不同的用户角色数据来区分根项和子项等
	if (bIsChild)
	{
		m_groupType = PTOP;	//如果被双击的树形控件项是子项，即成员项，则将群组类型设置为私聊，以便在打开聊天窗口时能够正确地设置聊天窗口的标题和聊天窗口列表项的内容等
		QString strPeopleName = m_groupPeopleMap.value(item);	//根据被双击的树形控件项来获取对应的成员信息，例如可以通过树形控件项的数据来查找对应的成员信息，并进行相应的操作，例如显示成员的头像、昵称等
		WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole+1).toString(), m_groupType, strPeopleName);
	}
}

void TalkWindow::initControl()
{
	QList<int>rightWidgetSize;	//创建一个整数列表，用于设置右侧控件的大小
	rightWidgetSize << 600 << 138;		//将右侧控件的大小设置为600像素，右侧控件的大小为窗口宽度减去600像素
	ui.bodySplitter->setSizes(rightWidgetSize);	//将设置好的大小列表应用到分割器控件上，调整左右控件的大小比例

	ui.textEdit->setFontPointSize(10);	//设置消息输入框的字体大小为10点
	ui.textEdit->setFocus();	//设置消息输入框为默认焦点，用户打开聊天窗口后可以直接输入消息内容
	ui.textEdit->setPlaceholderText("请输入消息内容...");	//设置消息输入框的占位文本，提示用户输入消息内容

	connect(ui.sysmin,SIGNAL(clicked(bool)), parent(),SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	switch (m_groupType)
	{
	case COMPANY: {
		initCompanyTalk();
		break;
		}
	case PERSONELGROUP: {
		initPersonelTalk();
		break;
	}
	case DEVELOPMENTGROUP: {
		initDevelopTalk();
		break;
	}
	case MARKETGROUP: {
		iniMarketTalk();
		break;
	}
	default: {
		
		initPtoPTalk();
		break;
	}	
	}
}

void TalkWindow::initCompanyTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	//设置data区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false,ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//shell高度-shell头高（talkwindow title）

	int nEmployeeNum = 50;		//假设公司有50个员工
	QString qsGroupName = QString("公司群 %1/%2").arg(0).arg(nEmployeeNum);	//公司群名称
	pItemName->setText(qsGroupName);	//设置树控件项的文本为公司群名称

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);	//将树控件项添加到树控件的顶层，显示在树控件中
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);	//将树控件项设置为树控件的子控件，显示在树控件中

	//展开
	pRootItem->setExpanded(true);	//将树控件项设置为展开状态，显示其子项
	for (int i = 0; i < nEmployeeNum; ++i)
	{
		addPeopleInfo(pRootItem);
	}
}

void TalkWindow::addPeopleInfo(QTreeWidgetItem* pRootGroupItem)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;
	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");	//加载员工头像图片资源，例如可以将员工头像图片保存在资源文件中，并根据员工编号来加载对应的头像图片等
	//const QImage image(":/Resourses/MainWindow/girl.png");
	QPixmap image;
	image.load(":/Resources/MainWindow/girl.png");

	pChild->setData(0, Qt::UserRole, 1);	//设置data区分根项子项
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));	//设置data区分员工编号

	ContactItem* pContactItem = new ContactItem(ui.treeWidget);	//创建一个联系人项对象，表示员工信息，例如可以包含员工的头像、姓名等信息等

	static int i = 1;	//员工编号索引，假设从1开始递增
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(image,pix1,pContactItem->getHeadLabelSize()));	//设置联系人项的头像图片为加载的员工头像图片
	pContactItem->setUserName(QString("员工%1").arg(i++));	//设置联系人项的用户名为员工编号，例如可以根据员工编号来设置不同的用户名等
	pContactItem->setSignName("");	//设置联系人项的签名为员工的个性签名，例如可以根据员工编号来设置不同的个性签名等


	pRootGroupItem->addChild(pChild);	//将树控件项添加到根项的子项列表中，显示在树控件中
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);	//将树控件项设置为树控件的子控件，显示在树控件中

	QString str = pContactItem->getUserName();	//获取联系人项的用户名，例如可以通过调用联系人项的getUserName()函数来获取员工的用户名等
	m_groupPeopleMap.insert(pChild,str);	//将树控件项和员工编号的映射关系保存到群组成员信息映射表中，方便在需要获取员工信息时通过树控件项来查找对应的员工编号等
}

void TalkWindow::initPersonelTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	//设置data区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//shell高度-shell头高（talkwindow title）

	int nEmployeeNum = 5;		//假设公司有50个员工
	QString qsGroupName = QString("人事群 %1/%2").arg(0).arg(nEmployeeNum);	//公司群名称
	pItemName->setText(qsGroupName);	//设置树控件项的文本为公司群名称

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);	//将树控件项添加到树控件的顶层，显示在树控件中
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);	//将树控件项设置为树控件的子控件，显示在树控件中

	//展开
	pRootItem->setExpanded(true);	//将树控件项设置为展开状态，显示其子项
	for (int i = 0; i < nEmployeeNum; ++i)
	{
		addPeopleInfo(pRootItem);
	}
}

void TalkWindow::initDevelopTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	//设置data区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//shell高度-shell头高（talkwindow title）

	int nEmployeeNum = 32;		//假设公司有50个员工
	QString qsGroupName = QString("研发群 %1/%2").arg(0).arg(nEmployeeNum);	//公司群名称
	pItemName->setText(qsGroupName);	//设置树控件项的文本为公司群名称

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);	//将树控件项添加到树控件的顶层，显示在树控件中
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);	//将树控件项设置为树控件的子控件，显示在树控件中

	//展开
	pRootItem->setExpanded(true);	//将树控件项设置为展开状态，显示其子项
	for (int i = 0; i < nEmployeeNum; ++i)
	{
		addPeopleInfo(pRootItem);
	}
}

void TalkWindow::iniMarketTalk()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem;
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	//设置data区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//shell高度-shell头高（talkwindow title）

	int nEmployeeNum = 8;		//假设公司有50个员工
	QString qsGroupName = QString("市场群 %1/%2").arg(0).arg(nEmployeeNum);	//公司群名称
	pItemName->setText(qsGroupName);	//设置树控件项的文本为公司群名称

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);	//将树控件项添加到树控件的顶层，显示在树控件中
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);	//将树控件项设置为树控件的子控件，显示在树控件中

	//展开
	pRootItem->setExpanded(true);	//将树控件项设置为展开状态，显示其子项
	for (int i = 0; i < nEmployeeNum; ++i)
	{
		addPeopleInfo(pRootItem);
	}
}

void TalkWindow::initPtoPTalk()
{
	
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");	//加载员工头像图片资源，例如可以将员工头像图片保存在资源文件中，并根据员工编号来加载对应的头像图片等
	ui.widget->setFixedSize(pixSkin.size());
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);	//设置个人聊天窗口的背景图片为加载的员工头像图片，例如可以根据个人聊天窗口的用户ID来加载对应的背景图片等
	skinLabel->setFixedSize(ui.widget->size());

	QList<int> sizes;
	sizes << 1 << 0;
	ui.splitter->setSizes(sizes);	//将设置好的大小列表应用到分割器控件上，调整左右控件的大小比例，例如在个人聊天窗口中可以将左侧的树形控件和右侧的消息显示区域的大小设置为不同的比例，以便更好地展示聊天内容等
}
void TalkWindow::onSendBtnClicked(bool)
{
	if (ui.textEdit->toPlainText().isEmpty())
	{
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("发送的信息不能为空！"),
			this, QRect(0, 0, 120, 100), 2000);
		return;
	}
	QString html = ui.textEdit->document()->toHtml();

	//文本html如果没有字体则添加,msgFont.txt
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");	//加载消息字体样式文件，例如可以将消息字体样式保存在资源文件中，并在发送消息时加载该文件来设置消息的字体样式等
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();	//读取消息字体样式文件的内容，例如可以将文件内容保存到一个字符串变量中，以便在发送消息时将该样式应用到消息内容中等
			fontHtml.replace("%1", text);	//将消息内容替换到消息字体样式中，例如可以在消息字体样式文件中使用占位符来表示消息内容，并在发送消息时将占位符替换为实际的消息内容等
			file.close();
		}
		else
		{
			QMessageBox::information(this, "提示", "消息字体样式文件加载失败！");	//如果消息字体样式文件加载失败，则显示一个提示信息框，告知用户消息字体样式文件加载失败等
		}
		if (!html.contains(fontHtml))
		{
			html.replace(text, fontHtml);
		}
	}

	ui.textEdit->clear();	//清空消息输入框中的文本内容，例如在发送消息后可以调用消息输入框的clear()函数来清空输入框中的文本内容，以便用户可以继续输入新的消息内容等
	ui.textEdit->deletAllEmotionImage();	//删除消息输入框中的所有表情图片，例如在发送消息后可以调用消息输入框的deletAllEmotionImage()函数来删除输入框中的所有表情图片，以便用户可以继续输入新的消息内容等
	ui.msgWidget->appendMsg(html);	//收信息窗口添加信息

}
