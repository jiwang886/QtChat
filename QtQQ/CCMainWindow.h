#pragma once

#include "BasicWindow.h"
#include "ui_CCMainWindow.h"

class CCMainWindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainWindow(QWidget *parent = nullptr);
    ~CCMainWindow();

	void initControl();		//初始化控件
    void setUserName(const QString& username);		//设置用户名
	void setHeadPixmap(const QPixmap& headPath);		//设置用户头像
	void setLevelPixmap(int level);		//设置用户等级
	void setStatusMenuIcon(const QString& statusPath);		//设置用户状态

	void initTimer();		//初始化定时器，来验证升级功能的实现
    //添加应用部件
	QWidget* addOtherAppExtension(const QString& appPath,const QString& appName);
	void initContactTree();		//初始化联系人树形控件，设置树形控件的内容和结构，例如添加联系人分组、联系人等

private:
	void updateSearchStyle();		//更新搜索框的样式，根据搜索框是否有文本来改变样式，例如显示或隐藏搜索图标等
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, const QString& sDeps);		//添加公司部门到树形控件中，根据部门信息创建树形项，并添加到树形控件中，例如根据部门名称、部门ID等信息来创建树形项
private:
	void resizeEvent(QResizeEvent* event) override;		//重置窗口大小事件，调整控件位置和大小
	bool eventFilter(QObject* watched, QEvent* event) override;		//事件过滤器，处理特定事件，例如鼠标点击等
	void mousePressEvent(QMouseEvent* event) override;		//鼠标按下事件，处理鼠标点击事件，例如拖动窗口等

private slots:
	void onAppIconClicked();		//应用图标被点击的槽函数
	void onItemClicked(QTreeWidgetItem* item, int column);		//树形控件的项被点击的槽函数，处理点击事件，例如显示联系人信息等
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);		//树形控件的项被双击的槽函数，处理双击事件，例如打开聊天窗口等
	void onItemExpanded(QTreeWidgetItem* item);		//树形控件的项被展开的槽函数，处理展开事件，例如加载子项等
	void onItemCollapsed(QTreeWidgetItem* item);		//树形控件的项被折叠的槽函数，处理折叠事件，例如释放子项资源等
	
private:
    Ui::CCMainWindowClass ui;
	QMap<QTreeWidgetItem*, QString>m_groupMap;	//部门树形项与部门ID的映射关系，方便后续通过树形项来获取对应的部门ID，例如在点击树形项时可以通过这个映射关系来获取部门ID，并进行相应的操作，例如显示部门信息等
};

