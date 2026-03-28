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
	bool connectMySql();	//连接MySQL数据库，验证用户输入的账号和密码是否正确，例如可以使用Qt提供的QSqlDatabase类来连接MySQL数据库，并执行相应的SQL查询来验证用户输入的账号和密码是否正确，如果连接成功并且查询结果正确则返回true，否则返回false
	bool veryfyAccountCode(bool &isAccountLogin,QString &strAccount);	//验证账号和密码的正确性，例如可以通过连接数据库来验证用户输入的账号和密码是否正确，如果验证成功则返回true，否则返回false

private:
	Ui::UserLogin ui;
};

