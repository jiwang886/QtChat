#include "QClickLabel.h"
#include <QMouseEvent>
QClickLabel::QClickLabel(QWidget *parent)
	: QLabel(parent)
{}

QClickLabel::~QClickLabel()
{}

void QClickLabel::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)		//如果鼠标左键被按下
		emit clicked();							//发出clicked信号，通知外部有点击事件发生
}

