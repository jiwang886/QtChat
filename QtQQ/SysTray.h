#pragma once

#include <QSystemTrayIcon>
#include <QWidget>
class SysTray  : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SysTray(QWidget *parent);
	~SysTray();
	void onIconActivated(QSystemTrayIcon::ActivationReason reason);	
	//系统托盘图标被激活时的槽函数，参数reason表示激活的原因，例如单击、双击等

private:
	void initSystemTray();		//初始化系统托盘图标和菜单
	void addSysTrayMenu();		//添加系统托盘菜单

private:
	QWidget* m_parent;		//父窗口指针
	//因为系统托盘图标需要与父窗口进行交互，例如显示或隐藏父窗口，所以需要保存父窗口的指针
};

