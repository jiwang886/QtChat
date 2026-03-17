#pragma once
#include "BasicWindow.h"
#include <QWidget>
#include "ui_UserLogin.h"

class UserLogin : public BasicWindow		//用户登录窗口，继承自BasicWindow，具有BasicWindow的功能和特性
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = nullptr);
	~UserLogin();


private slots:
	void onLoginBtnClicked();		//登录按钮被点击的槽函数

private:
	void initControl();		//初始化控件

private:
	Ui::UserLogin ui;
};

