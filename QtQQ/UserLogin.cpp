#include "UserLogin.h"
#include "CCMainWindow.h"
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
}


UserLogin::~UserLogin()
{}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);		//获取头像标签
	headLabel->setFixedSize(68, 68);	//设置头像标签的大小
	QPixmap pixmap(":/Resources/MainWindow/head_mask.png");		//空的圆形头像图片
	//调用getRoundImage函数，获取圆形头像图片，并设置到头像标签上
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pixmap, headLabel->size()));
	//将头像标签移动到标题栏的中心位置，距离标题栏底部34像素，计算结果为正中间
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);		//连接登录按钮的点击信号到槽函数
}

void UserLogin::onLoginBtnClicked()		//登录按钮被点击的槽函数
{
	//这里可以添加登录逻辑，例如验证用户名和密码，显示登录结果等
	//暂时只是关闭登录窗口
	close();
	CCMainWindow* mainWindow = new CCMainWindow();		//创建主窗口对象
	mainWindow->show();			//显示主窗口
}