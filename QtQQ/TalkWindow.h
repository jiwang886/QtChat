#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent,const QString& uid,GroupType groupType);
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);	//添加表情图片到聊天窗口中，根据表情编号来确定要添加的表情图片，例如可以根据表情编号来加载对应的表情图片资源，并将其添加到聊天窗口的消息输入框中等
	void setWindowName(const QString& name);	//设置聊天窗口的标题，根据传入的名称来设置聊天窗口的标题，例如可以将用户的昵称、群组名称等信息作为聊天窗口的标题，方便用户识别当前聊天窗口的内容和对象等

private slots:
	void onSendBtnClicked(bool);		//发送按钮被点击的槽函数，处理发送消息的逻辑，例如获取消息输入框中的文本内容，创建一个新的消息对象，并将其添加到聊天窗口的消息列表中等
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);	//树形控件项被双击的槽函数

private:
	void initControl();		//初始化控件，设置聊天窗口的控件属性和布局，例如设置消息输入框的占位文本、设置发送按钮的图标等
	//初始化各个部门的聊天窗口
	void initCompanyTalk();		
	void initPersonelTalk();
	void initDevelopTalk();
	void iniMarketTalk();
	void initPtoPTalk();
	void addPeopleInfo(QTreeWidgetItem* pRootGroupItem);

private:
	Ui::TalkWindow ui;
	QString m_talkId;		//用户ID，唯一标识一个用户，例如在打开聊天窗口时可以通过用户ID来获取对应的用户信息，并进行相应的操作，例如显示用户头像、昵称等
	GroupType m_groupType;	//群组类型，例如公司群、部门群、私聊等，根据群组类型来设置聊天窗口的标题和聊天窗口列表项的内容等
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;	//群组成员信息映射表，使用一个QMap来存储群组成员的信息，例如将群组成员的树形控件项作为键，成员的用户ID作为值，在需要获取成员信息时可以通过树形控件项来查找对应的用户ID，并进行相应的操作，例如显示成员的头像、昵称等

};

