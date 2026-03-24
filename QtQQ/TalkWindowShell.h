#pragma once

#include "BasicWindow.h"
#include "ui_TalkWindowShell.h"
#include <QMap>

class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

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
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem* talkWindowItem,GroupType grouptype);		//添加一个聊天窗口到容器中
	void setCurrentTalkWindow(QWidget* widget);		//设置当前显示的聊天窗口

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap();		//获取聊天窗口列表项和对应的聊天窗口的映射关系

private:
	void initControl();		//初始化控件

public slots:
	void onEmotionBtnClicked(bool);		//表情按钮被点击的槽函数，仅判断

private slots:
	void onTalkWindowItemClicked(QListWidgetItem* item);		//聊天窗口列表项被点击的槽函数
	void onEmotionBtnClicked(int emotionNum);		//表情按钮被点击的槽函数

private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkwindowItemMap;	//存储聊天窗口列表项和对应的聊天窗口的映射关系
	EmotionWindow* m_emotionWindow;		//表情窗口指针
};

