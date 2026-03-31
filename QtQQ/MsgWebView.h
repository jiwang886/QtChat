#pragma once

#include <QWebEngineView>
#include <QDomNode>

//自定义类MsgHtmlObj用于存储消息的HTML模板
class MsgHtmlObj :public QObject
{
	Q_OBJECT
	//发来的信息
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
	//自己发送的信息
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)

public:
	MsgHtmlObj(QObject* parent, QString msgLPicPath = "");//发信息来的人的头像路径

signals:
	void signalMsgHtml(const QString& html);		//消息HTML模板改变的信号，通知聊天窗口更新消息显示，例如当消息内容发生改变时，可以发出这个信号来通知聊天窗口重新渲染消息内容

private:
	void initHtmlTmpl();		//初始化消息HTML模板，设置默认的消息HTML模板
	QString getMsgTmplHtml(const QString& code);		//获取消息HTML模板

private:
	QString m_msgLPicPath;	//发信息来的人的头像路径，使用一个QString来存储发信息来的人的头像路径
	QString m_msgLHtmlTmpl;		//消息HTML模板，使用一个QString来存储消息的HTML模板
	QString m_msgRHtmlTmpl;		//消息HTML模板，使用一个QString来存储消息的HTML模板


};

//自定义类MsgWebPage继承自QWebEnginePage，用于处理消息中的链接点击事件
class MsgWebPage : public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = NULL) :QWebEnginePage(parent) {}
	~MsgWebPage();

protected:
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;	
	//重写acceptNavigationRequest方法，处理导航请求，例如当用户点击消息中的链接时，可以在这个方法中处理链接的打开方式
};

//QWebEngineView是Qt提供的一个用于显示网页内容的控件，可以在聊天窗口中使用MsgWebView来显示消息内容
class MsgWebView  : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget* parent);	
	~MsgWebView();

	void appendMsg(const QString& html,QString strObj = "0");	//在消息显示区域追加消息内容，例如当收到一条新消息时，可以调用这个方法来将消息内容追加到聊天窗口的消息显示区域

private:
	QList<QStringList>parseHtml(const QString& html);	//解析HTML内容，提取出消息中的文本、表情图片等信息
	//Qt中所有DOM节点都可以通过QDomNode类来表示
	QList<QStringList>parseDocNode(const QDomNode& node);	//解析HTML节点

signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile);

private:
	MsgHtmlObj* m_msgHtmlObj;	//消息HTML对象，使用一个MsgHtmlObj对象来存储消息的HTML内容
	QWebChannel* m_webChannel;	//WebChannel对象，使用一个QWebChannel对象来实现JavaScript与C++之间的通信
};

