#pragma once

#include <QLabel>
#include <QPropertyAnimation>
class RootContactItem  : public QLabel
{
	Q_OBJECT

	//箭头角度
		Q_PROPERTY(int rotation READ rotation WRITE setRotation)	
		//定义一个属性rotation，表示箭头的旋转角度，可以通过读取函数rotation和写入函数setRotation来访问和修改这个属性
public:
	RootContactItem(bool hasArrow = true, QWidget *parent = NULL);
	//hasArrow参数表示是否有箭头，默认为true，表示有箭头；parent参数表示父部件，默认为NULL，表示没有父部件
	~RootContactItem();

public:
	void setText(const QString& text);		//设置文本内容
	void setExpanded(bool expand);			//设置展开状态，expanded为true表示展开，false表示折叠

private:
	int rotation() const;				//获取箭头的旋转角度
	void setRotation(int rotation);		//设置箭头的旋转角度，angle为旋转角度的值

protected:
	void paintEvent(QPaintEvent* event) override;		//重绘事件，绘制文本和箭头，根据展开状态来调整箭头的旋转角度

private:
	QPropertyAnimation* m_animation;		//动画对象，用于实现箭头的旋转动画效果
	QString m_titleText;		//标题文本内容
	int m_rotation;				//箭头的旋转角度
	bool m_hasArrow;			//是否有箭头，true表示有箭头，false表示没有箭头
};

