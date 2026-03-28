#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <qwebchannel.h>
#include "TalkWindowShell.h"
#include "WindowManager.h"


MsgHtmlObj::MsgHtmlObj(QObject* parent)
	: QObject(parent)
{
	initHtmlTmpl();		//初始化消息HTML模板，设置默认的消息HTML模板
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");		//获取左侧消息的HTML模板，使用一个函数来获取对应的HTML模板，例如根据传入的代码来确定要获取哪个消息HTML模板
	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");		//获取右侧消息的HTML模板，使用一个函数来获取对应的HTML模板，例如根据传入的代码来确定要获取哪个消息HTML模板
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
	: QWebEngineView(parent)
{
	MsgWebPage* page = new MsgWebPage(this);		//创建一个MsgWebPage对象，并将其设置为MsgWebView的页面，使用MsgWebPage类来处理导航请求等操作，确保在聊天窗口中正确地显示消息内容
	setPage(page);

	QWebChannel* channel = new QWebChannel(this);		//创建一个QWebChannel对象，并将MsgHtmlObj对象注册到QWebChannel中，使用QWebChannel类来实现C++和JavaScript之间的通信，确保在聊天窗口中能够正确地处理消息内容的显示和交互
	m_msgHtmlObj = new MsgHtmlObj(this);
	channel->registerObject("external", m_msgHtmlObj);
	this->page()->setWebChannel(channel);		//将QWebChannel对象设置为MsgWebView页面的WebChannel，确保在聊天窗口中能够正确地处理消息内容的显示和交互
	
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();		//获取聊天窗口的Shell对象，使用WindowManager类来管理聊天窗口的创建和销毁等操作，确保在聊天窗口中能够正确地处理消息内容的显示和交互
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);


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
	
	int imageNum = 0;		//表情数量
	int msgType = 1;		//消息类型:0表情，1文本，2文件等
	bool isImageMsg = false;		//是否是表情消息，使用一个布尔变量来标识消息内容是否包含表情图片等信息，确保在处理消息内容时能够正确地判断消息类型和内容
	QString strData;		//消息内容: 表情都给三位比如：055 008 123，文本直接给文本内容，文件给文件路径

	
	for (int i=0; i < msgLst.size(); i++)
	{
		if (msgLst.at(i).at(0) == "img")
		{
			QString imagePath = msgLst.at(i).at(1);		
			QPixmap pixmap;		

			//获取表情名称的位置
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";		//表情图片的路径前缀，使用一个QString变量来存储表情图片的路径前缀，确保在加载表情图片时能够正确地找到图片文件
			int pos = strEmotionPath.length();		//表情图片的路径前缀长度，使用一个整数变量来存储表情图片的路径前缀长度，确保在获取表情名称时能够正确地截取表情图片的文件名
			isImageMsg = true;		//如果消息内容包含表情图片等信息，则将isImageMsg变量设置为true，表示消息内容是表情消息，确保在处理消息内容时能够正确地判断消息类型和内容

			//获取表情名称
			QString strEmotionName = imagePath.mid(pos);		//表情名称，使用QString类的mid函数来获取表情图片的文件名，确保在获取表情名称时能够正确地截取表情图片的文件名
			strEmotionName.replace(".png", "");		//去掉表情图片的扩展名，使用QString类的replace函数来去掉表情图片的扩展名，确保在获取表情名称时能够正确地获取表情名称
			int emotionNameL = strEmotionName.length();		//表情名称的长度，使用一个整数变量来存储表情名称的长度，确保在获取表情编号时能够正确地截取表情名称的最后三位数字
			if (emotionNameL == 1)	//补足三位
			{
				strData = strData + "00" + strEmotionName;		//如果表情名称的长度为1，则在表情名称前面补足两个0，确保表情编号的格式为三位数字
			}
			else if(emotionNameL == 2)
			{
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameL == 3)
			{
				strData = strData + strEmotionName;
			}
			msgType = 0;		//消息类型为表情，设置消息类型为0，表示消息内容是表情图片等信息
			imageNum++;		//表情数量加1，使用一个整数变量来统计消息内容中的表情图片数量，确保在处理消息内容时能够正确地统计表情图片的数量



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
			qsMsg += msgLst.at(i).at(1);		//如果消息内容是文本信息，则直接将文本内容追加到qsMsg变量中，表示消息内容中的文本信息
			strData = qsMsg;		//将文本内容赋值给strData变量，表示消息内容中的文本信息，确保在处理消息内容时能够正确地获取文本信息
		}
	}
	msgObj.insert("MSG", qsMsg);	//qsMsg替换MSG
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);		//紧凑
	if (strObj == "0")	//发信息
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));//调用JavaScript函数来将消息内容追加到聊天窗口的消息显示区域，使用QWebEngineView类的page函数来获取当前页面，并使用runJavaScript函数来执行JavaScript代码，将消息内容传递给JavaScript函数进行处理和显示
		
		if (isImageMsg)
		{
			strData = QString::number(imageNum) + "images" + strData;		//如果消息内容包含表情图片等信息，则在消息内容前面添加表情数量，确保在处理消息内容时能够正确地统计表情图片的数量和类型
		}

		emit signalSendMsg(strData, msgType, "");//发送消息内容，使用Qt的信号和槽机制来发送消息内容，确保在聊天窗口中能够正确地处理消息内容的显示和交互
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
			if (element.tagName() == "span")
			{
				QStringList attributeList;
				attributeList << "text" << element.text();		//如果元素的标签
				attribute << attributeList;		//如果元素的标签名是span，则获取span元素的文本内容，并将其存储在一个QStringList对象中，表示消息中的文本信息
			}
			if (node.hasChildNodes())
			{
				attribute << parseDocNode(node);		//如果子节点还有子节点，则递归调用parseDocNode函数来解析子节点，直到解析到最底层的节点为止
			}
		}
	}
	return attribute;
}

