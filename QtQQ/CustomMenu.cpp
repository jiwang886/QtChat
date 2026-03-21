#include "CustomMenu.h"
#include "CommonUtils.h"
CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);		//设置菜单背景透明
	CommonUtils::loadStyleSheet(this, "Menu");		//加载样式表，设置菜单的外观和风格
	//由于这个自定义菜单没有继承到任何可以加载样式表的父类
	// 所以需要在构造函数中调用CommonUtils::loadStyleSheet函数，传入this指针和样式表名称"Menu"
	// 来加载菜单的样式表，使其显示为预期的外观和风格
}

CustomMenu::~CustomMenu()
{}

void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{
	// 创建一个新的QAction对象，使用addAction函数将其添加到菜单中，并设置图标和文本
	QAction* pAction = addAction(QIcon(icon), name);
	// 将菜单项文本和对应的QAction对象存储到m_menuActMap中，方便后续通过文本获取对应的QAction对象
	m_menuActMap.insert(text, pAction);

}

QAction * CustomMenu::getAction(const QString& text)
{
	// 通过文本获取对应的QAction对象，直接从m_menuActMap中查找并返回对应的QAction对象
	return m_menuActMap[text];
}

