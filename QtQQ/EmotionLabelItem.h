#pragma once

#include "QClickLabel.h"
#include <QMovie>

class EmotionLabelItem  : public QClickLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget *parent);
	~EmotionLabelItem();
	void setEmotionName(int emotionNum);

private:
	void initControl();		//初始化控件

signals:
	void emotionClicked(int emotionNum);		//表情项被点击的信号，传递表情编号

private:
	int m_emotionName;		//表情编号
	QMovie* m_apngMovie;	//表情动画对象
};

