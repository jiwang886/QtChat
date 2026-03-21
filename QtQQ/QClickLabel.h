#pragma once

#include <QLabel>

class QClickLabel  : public QLabel
{
	Q_OBJECT

public:
	QClickLabel(QWidget *parent);
	~QClickLabel();
protected:
	void mousePressEvent(QMouseEvent* event);		//鼠标按下事件
signals:
	void clicked();		//点击信号
	
};

