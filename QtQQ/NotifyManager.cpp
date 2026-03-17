#include "NotifyManager.h"
#include "CommonUtils.h"
NotifyManager* NotifyManager::m_instance = nullptr;	//初始化NotifyManager的单例实例指针为nullptr

NotifyManager::NotifyManager():QObject(NULL)
{
}

NotifyManager::~NotifyManager()
{}

NotifyManager* NotifyManager::getInstance()	//获取NotifyManager的单例实例
{
	if (m_instance == nullptr)	//如果单例实例指针为nullptr
	{
		m_instance = new NotifyManager();	//创建NotifyManager的单例实例
	}
	return m_instance;	//返回NotifyManager的单例实例指针
}

void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color)	//通知其他窗口改变皮肤颜色的函数，参数是新的颜色
{
	emit signalSkinChanged(color);	//发出皮肤改变的信号，参数是新的颜色
	CommonUtils::setDefaultSkinColor(color);	//更新默认皮肤颜色
}