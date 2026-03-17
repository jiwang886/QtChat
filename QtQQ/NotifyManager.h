#pragma once

#include <QObject>

class NotifyManager  : public QObject
{
	Q_OBJECT

public:
	NotifyManager();		//构造函数
	~NotifyManager();		//析构函数

signals:
	void signalSkinChanged(const QColor& color);	//皮肤改变的信号，参数是新的颜色

public:
	static NotifyManager* getInstance();	//获取NotifyManager的单例实例

	void notifyOtherWindowChangeSkin(const QColor& color);	//通知其他窗口改变皮肤颜色的函数，参数是新的颜色

private:
	static NotifyManager* m_instance;	//NotifyManager的单例实例指针
};

