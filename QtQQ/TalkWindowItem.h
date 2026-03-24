#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"
#include <QEvent>
#include <QEnterEvent>

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();
	void setHeadPixmap(const QPixmap& pixmap);		//设置头像图片
	void setMsgLabelContent(const QString& text);			//设置消息文本
	QString getMsgLbaelText() const;						//获取消息文本

private:
	void initControl();		//初始化控件

signals:
	void signalCloseBtnClicked();		//关闭按钮被点击的信号，传递当前聊天窗口列表项的指针
private:
	void enterEvent(QEnterEvent* event) override;		//鼠标进入事件
	void leaveEvent(QEvent* event) override;		//鼠标离开事件
	void resizeEvent(QResizeEvent* event) override;		//窗口大小改变事件



private:
	Ui::TalkWindowItem ui;
};

