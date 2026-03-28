#pragma once

#include "BasicWindow.h"
#include "ui_TalkWindowShell.h"
#include <QMap>
#include <QTcpSocket>


class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gtcpPort = 8888;		//TCP通信端口号，可以根据实际需求进行修改，例如设置为一个未被占用的端口号，确保通信的正常进行

enum GroupType
{
	COMPANY = 0,		//公司
	PERSONELGROUP = 1,	//人事
	DEVELOPMENTGROUP = 2,	//开发
	MARKETGROUP = 3,	//市场
	PTOP = 4,			//私聊
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();

public:
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem* talkWindowItem,const QString& uid/*,GroupType grouptype*/);		//添加一个聊天窗口到容器中
	void setCurrentTalkWindow(QWidget* widget);		//设置当前显示的聊天窗口

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap();		//获取聊天窗口列表项和对应的聊天窗口的映射关系

private:
	void initControl();		//初始化控件
	void initTcpSocket();		//初始化TCP套接字

	void getEmployeesID(QStringList& employeesList);		//获取员工ID列表，可以根据实际需求进行修改，例如从数据库中获取员工ID列表，或者从文件中读取员工ID列表等
	bool createJSFile(QStringList& employeesList);		//创建JS文件，保存员工列表等信息，可以根据实际需求进行修改		

public slots:
	void onEmotionBtnClicked(bool);		//表情按钮被点击的槽函数，仅判断

	//客户端发送Tcp数据（数据，数据类型，文件）
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");
	


private slots:
	void onTalkWindowItemClicked(QListWidgetItem* item);		//聊天窗口列表项被点击的槽函数
	void onEmotionBtnClicked(int emotionNum);		//表情按钮被点击的槽函数

private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkwindowItemMap;	//存储聊天窗口列表项和对应的聊天窗口的映射关系
	EmotionWindow* m_emotionWindow;		//表情窗口指针

private:
	QTcpSocket* m_tcpClientSocket;		//TCP客户端套接字
};

