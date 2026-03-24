#include "RootContactItem.h"
#include <QPainter>
RootContactItem::RootContactItem(bool hasArrow,QWidget *parent)
	: QLabel(parent), m_rotation(0), m_hasArrow(hasArrow)	//初始化成员变量，调用父类构造函数
{
	setFixedHeight(32);		//设置固定高度为32像素
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);		//设置水平扩展，垂直固定的大小策略，使得该部件在水平上可以扩展以填充可用空间，而在垂直上保持固定高度

	m_animation = new QPropertyAnimation(this, "rotation");		//创建一个属性动画对象，绑定到当前对象的"rotation"属性上，用于实现箭头的旋转动画效果
	m_animation->setDuration(50);		//设置动画持续时间为50毫秒
	m_animation->setEasingCurve(QEasingCurve::InQuad);		//设置动画的缓动曲线为InQuad，使得动画在开始时较慢，然后逐渐加速，最后在结束时较快，产生一种自然的动画效果

}

RootContactItem::~RootContactItem()
{}

void RootContactItem::setText(const QString & text)
{
	m_titleText = text;		//将传入的文本参数text赋值给成员变量m_titleText，更新标题文本内容
	update();		//调用update函数，触发重绘事件，更新界面显示
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand)
	{
		m_animation->setEndValue(90);		//如果展开状态为true，设置动画的结束值为90度，使得箭头旋转到90度的位置，表示展开状态
	}
	else
	{
		m_animation->setEndValue(0);		//如果展开状态为false，设置动画的结束值为0度，使得箭头旋转回0度的位置，表示折叠状态
	}
	m_animation->start();		//启动动画，使其开始执行，根据设置的结束值来实现箭头的旋转动画效果
}

int RootContactItem::rotation() const
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;		//将传入的旋转角度参数rotation赋值给成员变量m_rotation，更新箭头的旋转角度
	update();		//调用update函数，触发重绘事件，更新界面显示
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);		//创建一个QPainter对象，使用当前部件作为绘制设备
	painter.setRenderHint(QPainter::TextAntialiasing,1);	//设置绘制提示为TextAntialiasing，使得文本绘制时使用抗锯齿效果，提升文本的显示质量

	QFont font;					//创建一个QFont对象，表示绘制文本的字体
	font.setPointSize(10);		//设置字体的点大小为10，表示使用10号字体进行绘制
	painter.setFont(font);		//设置绘制文本的字体为10号字体

	
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	//绘制文本，指定文本的绘制区域为从(24, 0)开始，宽度为部件的宽度减去24像素，高度为部件的高度，文本对齐方式为左对齐和垂直居中，绘制内容为成员变量m_titleText

	painter.setRenderHint(QPainter::SmoothPixmapTransform);	//设置绘制提示为SmoothPixmapTransform，使得在绘制位图时使用平滑变换，提升位图的显示质量
	painter.save();	//保存当前绘制状态，以便后续恢复

	if (m_hasArrow)
	{
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");		//加载箭头图像资源，创建一个QPixmap对象，使用指定的图像路径加载箭头图像

		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);		//创建一个与箭头图像大小相同的透明图像，作为临时绘制设备

		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::TextAntialiasing, 1);	//设置绘制提示为TextAntialiasing，使得文本绘制时使用抗锯齿效果，提升文本的显示质量

		p.translate(pixmap.width() / 2, pixmap.height() / 2);		//将绘制坐标系的原点移动到箭头图像的中心位置，以便后续进行旋转变换
		p.rotate(m_rotation);		//根据成员变量m_rotation的值，顺时针旋转绘制坐标系，使得箭头图像按照指定的旋转角度进行旋转
		p.drawPixmap(0-pixmap.width() / 2, 0-pixmap.height() / 2, pixmap);		//绘制箭头图像，指定绘制位置为(-pixmap.width() / 2, -pixmap.height() / 2)，使得旋转后的箭头图像的中心位置与原点重合

		painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);		//将旋转后的箭头图像绘制到部件上，指定绘制位置为(4, (height() - pixmap.height()) / 2)，使得箭头图像在垂直方向上居中显示，并且距离左边缘有4像素的间距
		painter.restore();	//恢复之前保存的绘制状态，撤销之前的坐标变换和旋转设置，确保后续的绘制操作不受之前的变换影响
	}
	
	QLabel::paintEvent(event);		//调用父类QLabel的paintEvent函数，确保文本和其他默认绘制操作正常执行
}

