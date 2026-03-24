#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "EmotionLabelItem.h"
#include <QPainter>
#include <QStyleOption>
const int emtionColumn = 14;		//表情窗口每行显示的表情数量
const int emotionRow = 12;		//表情窗口每列显示的表情数量


EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);	//设置窗口为无边框子窗口，去掉标题栏和边框，使表情窗口看起来更简洁和美观
	setAttribute(Qt::WA_TranslucentBackground);	//设置窗口背景透明，使表情窗口的背景可以显示为透明，增强视觉效果
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏
	ui.setupUi(this);

	initControl();
}

EmotionWindow::~EmotionWindow()
{}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this,"EmotionWindow");		//加载样式表，设置窗口的外观和风格
	for (int row = 0; row < emotionRow; row++)
	{
		for (int column = 0; column < emtionColumn; column++)
		{
			EmotionLabelItem* label = new EmotionLabelItem(this);	//创建一个表情标签项，并将表情窗口的表情显示区域作为父窗口
			label->setEmotionName(row * emtionColumn + column);	//设置表情标签项的表情编号，根据行列计算得到唯一的表情编号
			connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::signalEmotionItemClicked);	//连接表情标签项被点击的信号和表情窗口的信号，传递表情编号
			ui.gridLayout->addWidget(label, row, column);	//将表情标签项添加到表情窗口的网格布局中，按照行列位置进行排列
		}
	}

}
void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;		//创建一个样式选项对象，用于描述当前窗口的样式状态
	opt.initFrom(this);		//初始化样式选项对象，从当前窗口获取样式状态信息
	QPainter painter(this);		//创建一个绘图对象，用于在当前窗口上进行绘制操作

	style()->drawPrimitive(QStyle::PE_Widget,&opt , &painter ,this);	//在表情窗口上绘制基本的样式元素，使用当前窗口的样式进行绘制
	__super::paintEvent(event);		//调用父类的绘制事件处理函数，确保窗口的其他部分也能正确绘制
}
void EmotionWindow::addEmotion(int emotionNum)
{
	hide();		//隐藏表情窗口
	emit signalEmotionWindowHide();		//发出表情窗口隐藏的信号，通知外部表情窗口已经隐藏
	emit signalEmotionItemClicked(emotionNum);		//发出表情项被点击的信号，传递表情编号
}

