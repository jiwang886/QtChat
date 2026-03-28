#include "TalkWindowItem.h"
#include "CommonUtils.h"
TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initControl();
}

TalkWindowItem::~TalkWindowItem()
{}

void TalkWindowItem::setHeadPixmap(const QPixmap & pixmap)
{
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");
	//QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");
	const QPixmap& headPixmap = CommonUtils::getRoundImage(pixmap, mask, ui.headlabel->size());
	ui.headlabel->setPixmap(headPixmap);
}

void TalkWindowItem::setMsgLabelContent(const QString& text)
{
	ui.msgLabel->setText(text);
}

QString TalkWindowItem::getMsgLbaelText() const
{
	return ui.msgLabel->text();
}

void TalkWindowItem::initControl()
{
	ui.tclosebtn->setVisible(0);		//默认隐藏关闭按钮，鼠标悬停时显示
	connect(ui.tclosebtn, &QPushButton::clicked, [=]() {		//连接关闭按钮的点击信号和一个lambda函数，当关闭按钮被点击时执行这个函数，隐藏当前聊天窗口列表项
		emit signalCloseBtnClicked();		//发出关闭按钮被点击的信号，传递当前聊天窗口列表项的指针	
		});

}

void TalkWindowItem::enterEvent(QEnterEvent* event)
{
	ui.tclosebtn->setVisible(1);		//鼠标进入时显示关闭按钮
	__super::enterEvent(event);
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(0);		//鼠标离开时隐藏关闭按钮
	__super::leaveEvent(event);
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);	//调用基类的resizeEvent函数，确保窗口大小改变事件被正确处理
}
