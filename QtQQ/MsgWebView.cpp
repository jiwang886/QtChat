#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <qwebchannel.h>
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QSqlQueryModel>
#include <QImage>
#include <QBuffer>
#include <QByteArray>
#include <QUrl>

extern QString gstrLoginHeadPath;		//全局变量，保存当前登录员工的头像路径，可以在其他类中使用，例如在获取员工头像路径时可以排除当前登录员工的头像路径等

QString getBase64ImageStr(const QString& path)	//将图片路径转换为Base64字符串，使用QImage类来加载图片，并使用QBuffer类来将图片数据写入一个QByteArray中，最后将QByteArray转换为Base64字符串返回
{
	if (path.isEmpty()) return "";
	QString actualPath = path;
	if (actualPath.startsWith("qrc:/")) {
		return actualPath;
	}
	if (actualPath.startsWith("file:///")) {
		actualPath = QUrl(actualPath).toLocalFile();
	}
	QImage img;
	if (img.load(actualPath)) {
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		img.save(&buffer, "PNG");
		return QString("data:image/png;base64,") + QString(ba.toBase64());
	}
	return actualPath;
}

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath)
	: QObject(parent)
{
	m_msgLPicPath = msgLPicPath;		//设置发信息来的人的头像路径，使用传入的参数来初始化成员变量，确保在聊天窗口中能够正确地显示发信息来的人的头像
	initHtmlTmpl();		//初始化消息HTML模板，设置默认的消息HTML模板
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");		//获取左侧消息的HTML模板，使用一个函数来获取对应的HTML模板，例如根据传入的代码来确定要获取哪个消息HTML模板
	m_msgLHtmlTmpl.replace("%1", getBase64ImageStr(m_msgLPicPath));		//将HTML模板中的占位符替换为发信息来的人的头像路径，使用QString的replace函数来替换HTML模板中的占位符，确保在聊天窗口中能够正确地显示发信息来的人的头像


	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");		//获取右侧消息的HTML模板，使用一个函数来获取对应的HTML模板，例如根据传入的代码来确定要获取哪个消息HTML模板
	m_msgRHtmlTmpl.replace("%1", getBase64ImageStr(gstrLoginHeadPath));		//将HTML模板中的占位符替换为自己的头像路径，使用QString的replace函数来替换HTML模板中的占位符，确保在聊天窗口中能够正确地显示自己的头像
}
QString MsgHtmlObj::getMsgTmplHtml(const QString& code)		//获取消息HTML模板
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QIODevice::ReadOnly);		//以只读方式打开HTML模板文件，使用QFile类来操作文件，确保文件被正确地打开和读取
	QString strData;
	if (file.isOpen())
	{
		strData = QString::fromUtf8(file.readAll());		//读取HTML模板文件的内容，使用QFile类的readAll函数来获取文件的全部内容，并将其存储在一个QString变量中
	}
	else
	{
		QMessageBox::information(NULL, "Tips", "Failed to init html!");
		return QString("");	//如果HTML模板文件无法打开，则显示一个消息框提示用户，并返回一个空的QString对象，表示获取HTML模板失败
	}
	file.close();		//关闭HTML模板文件，使用QFile类的close函数来关闭文件，释放资源等操作
	return strData;		//返回HTML模板内容，返回一个QString对象，包含了HTML模板的内容，可以在聊天窗口中使用这个HTML模板来渲染消息内容
}

MsgWebPage::~MsgWebPage()
{
}

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//只接受qrc:/*.html的链接，其他链接不接受
	if(url.scheme() == QString("qrc"))	//如果链接的协议是qrc，则接受导航请求，允许在聊天窗口中显示链接内容
	{
		return true;
	}
	else	//如果链接的协议不是qrc，则拒绝导航请求，阻止在聊天窗口中打开链接
	{
		return false;
	}
}


//QWebEngineView是Qt提供的一个用于显示网页内容的控件，可以在聊天窗口中使用MsgWebView来显示消息内容
MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent),m_webChannel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);		//创建一个MsgWebPage对象，并将其设置为MsgWebView的页面，使用MsgWebPage类来处理导航请求等操作，确保在聊天窗口中正确地显示消息内容
	setPage(page);

	m_msgHtmlObj = new MsgHtmlObj(this);
	m_webChannel->registerObject("external0", m_msgHtmlObj);
	
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();		//获取聊天窗口的Shell对象，使用WindowManager类来管理聊天窗口的创建和销毁等操作，确保在聊天窗口中能够正确地处理消息内容的显示和交互
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

	//当前正在构建的聊天窗口ID
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();		//获取当前正在构建的聊天窗口ID，使用WindowManager类来管理聊天窗口的创建和销毁等操作，确保在聊天窗口中能够正确地处理消息内容的显示和交互

	QSqlQueryModel queryEmployeeModel;		//创建一个QSqlQueryModel对象，用于执行SQL查询，使用QSqlQueryModel类来操作数据库，确保在聊天窗口中能够正确地获取员工信息等数据
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;

	//获取公司群ID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg("公司群"));		
	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();		//获取公司群ID，使用QSqlQueryModel类的index函数来获取查询结果的索引，并使用data函数来获取查询结果的值，确保在聊天窗口中能够正确地获取公司群ID等数据
	if (strTalkId == strCompanyID)
	{
		queryEmployeeModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
		isGroupTalk = true;
	}
	else
	{
		if (strTalkId.length() == 4)
		{
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE departmentID = %1 AND status = 1").arg(strTalkId));
			isGroupTalk = true;
		}
		else
		{
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE employeeID = %1 AND status = 1").arg(strTalkId));
			
			QModelIndex index = queryEmployeeModel.index(0, 0);
			strPicturePath = queryEmployeeModel.data(index).toString();

			strExternal = "external_" + strTalkId;		//外部对象名称，使用一个字符串变量来存储外部对象的名称，例如根据聊天窗口ID来生成一个唯一的外部对象名称，确保在聊天窗口中能够正确地处理消息内容的显示和交互
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);		//创建一个MsgHtmlObj对象，并将其设置为WebChannel的外部对象，使用MsgHtmlObj类来处理消息内容的显示和交互等操作，确保在聊天窗口中能够正确地处理消息内容的显示和交互
			m_webChannel->registerObject(strExternal, msgHtmlObj);			//将MsgHtmlObj对象注册到WebChannel中
		}
	}
	if (isGroupTalk)
	{
		QModelIndex employeeModelIndex,pictureModelIndex;
		int rows = queryEmployeeModel.rowCount();
		for (int i = 0; i < rows; i++)
		{
			employeeModelIndex = queryEmployeeModel.index(i, 0);
			pictureModelIndex = queryEmployeeModel.index(i, 1);

			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;		//外部对象名称，使用一个字符串变量来存储外部对象的名称，例如根据员工ID来生成一个唯一的外部对象名称，确保在聊天窗口中能够正确地处理消息内容的显示和交互
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_webChannel->registerObject(strExternal, msgHtmlObj);			//将MsgHtmlObj对象注册到WebChannel中，确保在聊天窗口中能够正确地处理消息内容的显示和交互
		}
	}
	
	this->page()->setWebChannel(m_webChannel);		//将QWebChannel对象设置为MsgWebView页面的WebChannel，确保在聊天窗口中能够正确地处理消息内容的显示和交互
	//初始化收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));		//加载消息HTML模板

}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString & html,QString strObj)	//在消息显示区域追加消息内容
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>& msgLst = parseHtml(html);		//解析HTML内容，提取出消息中的文本、表情图片等信息，使用parseHtml函数来解析HTML内容，并将解析结果存储在一个QList<QStringList>对象中

	for (int i=0; i < msgLst.size(); i++)
	{
		if (msgLst.at(i).at(0) == "img")
		{
			QString imagePath = msgLst.at(i).at(1);		
			QPixmap pixmap;		

			if (imagePath.left(3) == "qrc")
			{
				pixmap.load(imagePath.mid(3));		//去掉qrc前缀，使用QPixmap类的load函数来加载图片，确保图片被正确地加载和显示
			}
			else
			{
				pixmap.load(imagePath);		//如果图片路径不是以qrc开头，则直接使用QPixmap类的load函数来加载图片，确保图片被正确地加载和显示
			}
			//表情图片html格式组合
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\" />").arg(imagePath).arg(pixmap.width()).arg(pixmap.height());		
			qsMsg += imgPath;		//将表情图片的HTML格式组合成一个字符串，并将其追加到qsMsg变量中，表示消息内容中的表情图片等信息

		}
		else if(msgLst.at(i).at(0) == "text")
		{
			QString textValue = msgLst.at(i).at(1);
			textValue.replace("\n", "<br/>");
			qsMsg += textValue;		//如果消息内容是文本信息，则直接将文本内容追加到qsMsg变量中，表示消息内容中的文本信息
		}
	}
	if (qsMsg.endsWith("<br/>"))
	{
		qsMsg.chop(5);
	}

	msgObj.insert("MSG", qsMsg);	//qsMsg替换MSG
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);		//紧凑
	if (strObj == "0")	//发信息
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));//调用JavaScript函数来将消息内容追加到聊天窗口的消息显示区域，使用QWebEngineView类的page函数来获取当前页面，并使用runJavaScript函数来执行JavaScript代码，将消息内容传递给JavaScript函数进行处理和显示

		// 因为底层UDP通信协议对于表情(类型0)为纯占位符识别，文本(类型1)则为纯文本。不支持图文混排内容！
		// 因此我们将消息分割，连续的文本作为文本发，连续的表情作为表情发。
		QString continuousText = "";
		QString continuousImages = "";
		int imageNum = 0;

		for (int i=0; i < msgLst.size(); i++)
		{
			if (msgLst.at(i).at(0) == "img")
			{
				if (!continuousText.isEmpty()) {
					if (continuousText.endsWith("\n")) continuousText.chop(1);
					if (!continuousText.isEmpty()) {
						int msgType = 1;
						emit signalSendMsg(continuousText, msgType, "");
					}
					continuousText = "";
				}

				QString imagePath = msgLst.at(i).at(1);
				QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
				int pos = strEmotionPath.length();
				QString strEmotionName = imagePath.mid(pos);
				strEmotionName.replace(".png", "");
				int emotionNameL = strEmotionName.length();

				if (emotionNameL == 1) continuousImages += "00" + strEmotionName;
				else if(emotionNameL == 2) continuousImages += "0" + strEmotionName;
				else if(emotionNameL == 3) continuousImages += strEmotionName;

				imageNum++;
			}
			else if(msgLst.at(i).at(0) == "text")
			{
				if (!continuousImages.isEmpty()) {
					QString emitData = QString::number(imageNum) + "images" + continuousImages;
					int msgType = 0;
					emit signalSendMsg(emitData, msgType, "");
					continuousImages = "";
					imageNum = 0;
				}
				continuousText += msgLst.at(i).at(1);
			}
		}

		if (!continuousImages.isEmpty()) {
			QString emitData = QString::number(imageNum) + "images" + continuousImages;
			int msgType = 0;
			emit signalSendMsg(emitData, msgType, "");
		}
		if (!continuousText.isEmpty()) {
			if (continuousText.endsWith("\n")) continuousText.chop(1);
			if (!continuousText.isEmpty()) {
				int msgType = 1;
				emit signalSendMsg(continuousText, msgType, "");
			}
		}
	}
	else	//收信息
	{
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}
}


QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	QDomDocument doc;
	doc.setContent(html);		//将HTML内容解析为DOM文档对象
	const QDomElement& root = doc.documentElement();		//获取DOM文档的根节点
	const QDomNode& node = root.firstChildElement("body");	//获取根节点下的<body>节点，通常消息内容会放在<body>节点中
	return parseDocNode(node);
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList> attribute;
	const QDomNodeList& list = node.childNodes();		//获取HTML节点的子节点列表
	for (int i = 0; i < list.count(); i++)
	{
		const QDomNode& node = list.at(i);		//遍历子节点列表，获取每个子节点
		if (node.isElement())	//如果子节点是一个元素节点
		{
			//转换元素
			const QDomElement& element = node.toElement();	
			if (element.tagName() == "img")
			{
				QStringList attributeList;
				attributeList << "img" << element.attribute("src");		//如果元素的标签名是img，则获取img元素的src属性值，并将其存储在一个QStringList对象中，表示消息中的表情图片等信息
				attribute << attributeList;		//将QStringList对象添加到attribute列表中，表示消息中的表情图片等信息
			}
			if (element.tagName() == "br")
			{
				QStringList attributeList;
				attributeList << "text" << "\n";
				attribute << attributeList;
			}
			if (node.hasChildNodes())
			{
				attribute << parseDocNode(node);		//如果子节点还有子节点，则递归调用parseDocNode函数来解析子节点，直到解析到最底层的节点为止
			}
			if (element.tagName() == "p" || element.tagName() == "div")
			{
				QStringList attributeList;
				attributeList << "text" << "\n";
				attribute << attributeList;
			}
		}
		else if (node.isText())
		{
			QString txt = node.nodeValue();
			// 去除带有排版性质的换行符，后续我们统一按照结构(换行段落P或者BR)添加 \n
			txt.replace("\n", "");
			txt.replace("\r", "");
			if (!txt.isEmpty())
			{
				QStringList attributeList;
				attributeList << "text" << txt;
				attribute << attributeList;
			}
		}
	}
	return attribute;
}

