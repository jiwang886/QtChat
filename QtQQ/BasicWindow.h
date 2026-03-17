#pragma once

#include <QDialog>
#include "TitleBar.h"
#include <QMouseEvent>
class BasicWindow  : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = NULL);
	virtual ~BasicWindow();		//虚析构
	//确保当通过基类指针删除派生类对象时，能够正确调用派生类的析构函数，避免资源泄漏。
	//因为这个类会被其他类继承，所以需要将析构函数声明为虚函数，以确保在删除派生类对象时能够正确调用析构函数。

public:
	//加载样式表，用于设置窗口的外观和风格
	void loadStyleSheet(const QString& sheetName);
	//获取圆头像
	QPixmap getRoundImage(const QPixmap& src, QPixmap &mask,QSize masksize = QSize(0,0));
	//参数分别是：原图片，掩膜图片，掩膜图片的大小（默认为0，0，即与原图片大小相同）

private:
	void initBackGroundColor();		//初始化背景颜色

protected:
	void paintEvent(QPaintEvent* event);				//重绘事件
	void mousePressEvent(QMouseEvent* event);		//鼠标按下事件
	void mouseMoveEvent(QMouseEvent* event);		//鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event);	//鼠标释放事件
	void mouseDoubleClickEvent(QMouseEvent* event);	//鼠标双击事件

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);					//初始化标题栏
	void setTitleBarTitle(const QString& title, const QString& icon = "");					//设置标题栏内容

public slots:
	void onShowClose(bool);		//关闭窗口的槽函数
	void onShowMin(bool);		//最小化窗口的槽函数
	void onShowHide(bool);		//隐藏窗口的槽函数
	void onShowNormal(bool);	//正常显示窗口的槽函数
	void onShowQuit(bool);		//退出程序的槽函数

	void onSignalSkinChanged(const QColor& color);	//接收来自标题栏的信号，改变窗口背景颜色的槽函数
	void onButtonMinClicked();			//最小化按纽被点击的槽函数
	void onButtonMaxClicked();			//最大化按纽被点击的槽函数
	void onButtonRestoreClicked();		//还原按纽被点击的槽函数
	void onButtonCloseClicked();		//关闭按纽被点击的槽函数

protected:
	QPoint mousePoint;				//鼠标位置
	bool mousePressed;				//鼠标按下
	QColor m_backGroundColor;		//背景颜色
	QString m_styleName;			//样式文件名称
	TitleBar* m_TitleBar;			//标题栏指针
};

