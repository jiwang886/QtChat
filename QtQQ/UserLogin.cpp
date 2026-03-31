#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

QString gLoginEmployeeID;		//全局变量，保存当前登录用户的ID，例如可以在用户登录成功后将用户的ID保存到这个变量中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等


UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this); 
	ui.titleWidget->setProperty("titleskin", true);		//启用标题栏皮肤
	ui.bodyWidget->setProperty("bottomskin", true);		//启用底部皮肤
	setAttribute(Qt::WA_DeleteOnClose);		//设置窗口关闭时自动删除对象，避免内存泄漏
	initTitleBar();			//初始化标题栏
	setTitleBarTitle("",":/Resources/MainWindow/qqlogoclassic.png");		//设置标题栏内容
	loadStyleSheet("UserLogin");		//加载样式表
	initControl();			//初始化控件

	connect(m_TitleBar, &TitleBar::signalButtonCloseClicked, [=](){
		qApp->quit();
		});
}


UserLogin::~UserLogin()
{}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);		//获取头像标签
	headLabel->setFixedSize(68, 68);	//设置头像标签的大小
	QPixmap pixmap(":/Resources/MainWindow/head_mask.png");		//空的圆形头像图片
	//调用getRoundImage函数，获取圆形头像图片，并设置到头像标签上
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pixmap, headLabel->size()));
	//将头像标签移动到标题栏的中心位置，距离标题栏底部34像素，计算结果为正中间
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);		//连接登录按钮的点击信号到槽函数
	
	//连接数据库
	if (!connectMySql())
	{
		QMessageBox::information(this, "提示", "连接数据库失败，请检查数据库连接设置！");		//如果连接数据库失败，弹出一个消息框提示用户检查数据库连接设置
		close();		//关闭登录窗口
	}

}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");		//添加MySQL数据库驱动
	db.setDatabaseName("qtqq");		//设置数据库名称
	db.setHostName("localhost");		//设置数据库主机地址
	db.setUserName("root");		//设置数据库用户名
	db.setPassword("Y12396300y");		//设置数据库密码
	db.setPort(3306);		//设置数据库端口号
	if (db.open())
	{
		return true;		//如果成功连接数据库，返回true
	}
	else
		return false;		//如果连接数据库失败，返回false
}

bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)
{
	QString strAccountInput = ui.editUserAccount->text();		//获取用户输入的账号
	QString strCodeInput = ui.editPassword->text();		//获取用户输入的密码

	//输入员工号(QQ号)
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput);		//构造SQL查询语句，查询用户输入的账号是否存在于数据库中

	QSqlQuery queryEmployeeID(strSqlCode);		//创建SQL查询对象
	queryEmployeeID.exec();		//执行SQL查询
	//QQ号登录
	if (queryEmployeeID.first())		//将查询结果指针移动到第一条记录
	{
		QString strCode = queryEmployeeID.value(0).toString();		//获取查询结果中的code字段值，即数据库中存储的密码
		if (strCode == strCodeInput)
		{
			gLoginEmployeeID = strAccountInput;		//将当前登录用户的ID保存到全局变量中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等
			isAccountLogin = false;		//设置登录方式为QQ号登录
			strAccount = strAccountInput;		//将用户输入的账号保存到输出参数中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等
			return true;		//如果用户输入的密码与数据库中存储的密码相同，返回true
		}
		else
		{
			return false;
		}
	}

	//账号登录
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);
	QSqlQuery queryAccount(strSqlCode);		//创建SQL查询对象
	queryAccount.exec();		//执行SQL查询
	if (queryAccount.first())		//将查询结果指针移动到第一条记录
	{
		QString strCode = queryAccount.value(0).toString();		//获取查询结果中的code字段值，即数据库中存储的密码
		if (strCode == strCodeInput)
		{
			gLoginEmployeeID = queryAccount.value(1).toString();		//将当前登录用户的ID保存到全局变量中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等
			isAccountLogin = true;		//设置登录方式为账号登录
			strAccount = strAccountInput;		//将用户输入的账号保存到输出参数中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等
			return true;		//如果用户输入的密码与数据库中存储的密码相同，返回true
		}
		else
		{
			return false;
		}
	}


	return false;
}


void UserLogin::onLoginBtnClicked()		//登录按钮被点击的槽函数
{
	bool isAccountLogin;		//定义一个布尔变量，表示登录方式，默认为false，即QQ号登录
	QString strAccount;		//定义一个字符串变量，保存用户输入的账号，例如可以在验证账号和密码的函数中将用户输入的账号保存到这个变量中，以便在后续的操作中使用，例如在聊天窗口中显示当前用户的昵称、头像等信息等
	if(veryfyAccountCode(isAccountLogin,strAccount))
	{
	//这里可以添加登录逻辑，例如验证用户名和密码，显示登录结果等
	//暂时只是关闭登录窗口
	close();
	CCMainWindow* mainWindow = new CCMainWindow(strAccount,isAccountLogin);		//创建主窗口对象
	mainWindow->show();			//显示主窗口
	}
	else
		QMessageBox::information(this, "提示", "账号或密码错误，请重新输入！");		//如果验证失败，弹出一个消息框提示用户账号或密码错误
	//ui.editUserAccount->clear();		//清空账号输入框
	//ui.editPassword->clear();		//清空密码输入框
}