#include "SysTray.h"
#include "CustomMenu.h"
#include <QApplication>
SysTray::SysTray(QWidget *parent)
	:m_parent(parent),QSystemTrayIcon(parent)
{
	initSystemTray();		//调用初始化系统托盘图标和菜单的函数
	show();					//显示系统托盘图标
}

SysTray::~SysTray()
{}

void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason)	
//系统托盘图标被激活时的槽函数，根据激活原因执行相应的操作
{
	if(reason == QSystemTrayIcon::Trigger)	//如果激活原因是单击
	{
		//if (m_parent->isHidden())	//如果父窗口被隐藏
		//{
			m_parent->show();		//显示父窗口
			m_parent->activateWindow();	//激活父窗口，使其获得焦点
		//}
		//else						//如果父窗口没有被隐藏
		//{
		//	m_parent->hide();		//隐藏父窗口
		//}
	}
	else if (reason == QSystemTrayIcon::Context)	//如果激活原因是右键点击
	{
		addSysTrayMenu();
	}
}

void SysTray::initSystemTray()
{
	setToolTip("自制的QtQQ");		
	//设置系统托盘图标的提示文本，当鼠标悬停在图标上时显示
	setIcon(QIcon(":/Resources/MainWindow/app/02_128x128.ico"));		//设置系统托盘图标，使用资源文件中的图标路径
	connect(this, &QSystemTrayIcon::activated, this, &SysTray::onIconActivated);	
	//连接系统托盘图标的激活信号到槽函数，当图标被激活时调用onIconActivated函数
}

void SysTray::addSysTrayMenu()
{
	CustomMenu* customMenu = new CustomMenu(m_parent);		//创建一个自定义菜单对象
	customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/02_128x128.ico", "显示");
	customMenu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png", "退出");

	connect(customMenu->getAction("onShow"), &QAction::triggered, [this]() {	//连接显示菜单项的触发信号到一个lambda函数
		m_parent->show();		//在lambda函数中显示父窗口
		m_parent->activateWindow();	//激活父窗口，使其获得焦点
		});
	connect(customMenu->getAction("onQuit"), &QAction::triggered, [this]() {	//连接退出菜单项的触发信号到一个lambda函数
		qApp->quit();			//在lambda函数中退出应用程序
		});
	customMenu->exec(QCursor::pos());	//执行菜单，显示在鼠标当前位置
	delete customMenu;		//执行菜单，显示在鼠标当前位置，并在使用完毕后删除菜单对象，释放内存
	customMenu = NULL;
}

