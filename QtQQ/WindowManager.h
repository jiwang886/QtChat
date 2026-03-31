#pragma once

#include <QObject>
#include "TalkWindowShell.h"

class WindowManager  : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();

public:
	QWidget* findWindowName(const QString& qsWindowName);	
	//通过窗口名称获取对应的窗口对象，如果窗口名称存在于窗口映射关系中，返回对应的窗口对象，否则返回nullptr
	void deleteWindow(const QString& qsWindowName);			
	//从窗口映射关系中删除指定窗口名称的映射关系，如果窗口名称存在于窗口映射关系中，删除对应的窗口对象的映射关系，否则不进行任何操作
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);	
	//添加新的窗口名称和对应的窗口对象的映射关系，如果窗口名称不存在于窗口映射关系中，添加新的窗口名称和对应的窗口对象的映射关系，否则不进行任何操作
	static WindowManager* getInstance();	
	//获取WindowManager单例实例的静态函数，返回一个指向WindowManager对象的指针
	
	TalkWindowShell* getTalkWindowShell() const { return m_talkWindowShell; }	//直接返回聊天窗口容器指针

	QString getCreatingTalkId();		//获取正在创建的聊天窗口的ID，返回一个字符串，表示正在创建的聊天窗口的ID，如果没有正在创建的聊天窗口，则返回一个空字符串
	void addNewTalkWindow(const QString& uid/*, GroupType groupType = COMPANY,const QString& strPeople = ""*/);
	//添加一个新的聊天窗口，如果聊天窗口容器指针为空，创建一个新的TalkWindowShell对象，并连接聊天窗口容器的销毁信号和一个lambda函数，当聊天窗口容器被销毁时将聊天窗口容器指针置空，避免悬空指针。通过窗口名称获取对应的窗口对象，如果窗口对象不存在，创建一个新的TalkWindow对象，并传入相关参数，例如用户ID、群组类型、人员信息等。创建一个新的TalkWindowItem对象，并根据群组类型设置聊天窗口的标题和聊天窗口列表项的内容。将新的聊天窗口添加到聊天窗口容器中，并将新的聊天窗口和对应的聊天窗口列表项的映射关系添加到聊天窗口列表项和对应的聊天窗口的映射关系中。将新的聊天窗口设置为当前显示的聊天窗口。
	
private:
	TalkWindowShell* m_talkWindowShell;		//聊天窗口容器指针，管理所有的聊天窗口，例如添加、删除、切换等操作
	QMap<QString, QWidget*> m_windowMap;		//窗口名称与窗口对象的映射关系，方便通过窗口名称来获取对应的窗口对象，例如在打开聊天窗口时可以通过窗口名称来获取对应的聊天窗口对象，并进行相应的操作，例如显示、隐藏等
	QString m_strCreatingTalkId = "";		//正在创建的聊天窗口的ID
};

