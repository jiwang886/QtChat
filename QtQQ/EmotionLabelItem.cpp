#include "EmotionLabelItem.h"


EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: QClickLabel(parent)
{
	initControl();	//初始化控件
	connect(this, &QClickLabel::clicked, [=]() {
		emit emotionClicked(m_emotionName);	//当标签被点击时，发出emotionClicked信号，并传递表情编号
		});
}

EmotionLabelItem::~EmotionLabelItem()
{}

void EmotionLabelItem::setEmotionName(int emotionNum)
{
	m_emotionName = emotionNum;	//设置表情编号
	QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionNum);	//根据表情编号构造表情图片的路径
	m_apngMovie = new QMovie(imageName,"apng",this);	//创建QMovie对象，加载表情图片
	m_apngMovie->start();
	m_apngMovie->stop();	//停止动画，保持在第一帧显示
	setMovie(m_apngMovie);	//将QMovie对象设置为标签的动画
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);	//设置标签内容居中显示
	setObjectName("EmotionLabelItem");	//设置对象名称，方便在样式表中进行样式设置
	setFixedSize(32, 32);		//设置标签的固定大小为32x32像素，适合显示表情图标
}

