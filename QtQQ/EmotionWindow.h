#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = nullptr);
	~EmotionWindow();

private:
	void initControl();		//初始化控件

private slots:
	void addEmotion(int emotionNum);		//添加一个表情项到表情窗口中

signals:
	void signalEmotionWindowHide();		//表情窗口隐藏的信号	
	void signalEmotionItemClicked(int emotionNum);		//表情项被点击的信号，传递表情编号

private:
	void paintEvent(QPaintEvent* event) override;		//重写绘制事件，绘制表情窗口的背景和边框

private:
	Ui::EmotionWindow ui;
};

