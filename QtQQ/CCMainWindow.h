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
private:
	void resizeEvent(QResizeEvent* event) override;		//重置窗口大小事件，调整控件位置和大小

private slots:
	void onAppIconClicked();		//应用图标被点击的槽函数


private:
    Ui::CCMainWindowClass ui;
};

