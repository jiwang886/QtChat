#pragma
#include <QPixmap>
#include <QSize>
#include <QProxyStyle>

//自定义样式类，继承自QProxyStyle，可以重写其中的绘制方法来实现自定义的界面风格
//改变默认部件的外观和行为，例如按钮、标签、滚动条等，以适应特定的设计需求
class CustomPoxyStyle :public QProxyStyle
{
public:
	CustomPoxyStyle(QObject* parent)
	{
		setParent(parent);
	}
	//重写drawPrimitive函数，根据不同的元素类型进行自定义绘制，例如绘制圆角矩形、设置背景颜色等
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const
	{
		if (PE_FrameFocusRect == element)
		{
			//去掉windows部件默认的边框或虚线框，部件获取焦点时直接返回，不进行绘制
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
			//调用父类的drawPrimitive函数，进行默认的绘制操作
		}
	}
};

#pragma once
class CommonUtils
{
public:
	CommonUtils();
public:
	static QPixmap getRoundImage(const QPixmap& src,QPixmap &mask ,QSize masksize = QSize(0,0));	
	//获取圆头像，参数是原图片和掩膜图片的大小（默认为0，0，即与原图片大小相同）
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);		//加载样式表，用于设置窗口的外观和风格
	static void setDefaultSkinColor(const QColor& color);	//设置默认皮肤颜色
	static QColor getDefaultSkinColor();					//获取默认皮肤颜色
};

