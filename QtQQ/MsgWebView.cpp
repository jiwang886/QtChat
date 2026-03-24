#include "MsgWebView.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <qwebchannel.h>

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
	
	//初始化收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));		//加载消息HTML模板

}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString & html)	//在消息显示区域追加消息内容
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>& msgLst = parseHtml(html);		//解析HTML内容，提取出消息中的文本、表情图片等信息，使用parseHtml函数来解析HTML内容，并将解析结果存储在一个QList<QStringList>对象中
	for (int i=0; i < msgLst.size(); i++)
	{
		if (msgLst.at(i).at(0) == "img")
		{
			QString imagePath = msgLst.at(i).at(1);		
			QPixmap pixmap(imagePath);		
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
		}
	}
	msgObj.insert("MSG", qsMsg);	//qsMsg替换MSG
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);		//紧凑
	this->page()->runJavaScript(QString("appendHtml(%1)").arg(Msg));		//调用JavaScript函数来将消息内容追加到聊天窗口的消息显示区域，使用QWebEngineView类的page函数来获取当前页面，并使用runJavaScript函数来执行JavaScript代码，将消息内容传递给JavaScript函数进行处理和显示
	


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

