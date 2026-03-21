#pragma once

#include <QMenu>
#include <QMap>

class CustomMenu  : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = NULL);
	~CustomMenu();

public:
	// 添加自定义菜单项，传入菜单项文本、图标文件路径和对象名称
	void addCustomMenu(const QString& text, const QString& icon, const QString& name);
	// 通过文本获取对应的QAction对象
	QAction * getAction(const QString& text);

private:
	// 使用QMap来存储菜单项文本和对应的QAction对象的映射关系，方便后续通过文本获取对应的QAction对象
	QMap<QString, QAction*> m_menuActMap;
};

