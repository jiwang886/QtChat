#include "CCMainWindow.h"
#include <QProxyStyle>
#include <QPainter>
#include "SkinWindow.h"
#include <QTimer>
#include "SysTray.h"
class CoustomProxyStyle :public QProxyStyle		//自定义样式类，继承自QProxyStyle，可以重写其中的绘制方法来实现自定义的界面风格
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
		if (element == QStyle::PE_FrameFocusRect)    
        //去掉windows部件默认的边框或虚线框，部件获取焦点时直接返回，不进行绘制
        {
            return;
        }
        else
        {
			//调用父类的drawPrimitive函数，进行默认的绘制操作
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }
	}
};

CCMainWindow::CCMainWindow(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool);		//设置窗口为工具窗口，工具窗口通常具有较小的标题栏和边框，并且在任务栏中不显示
	loadStyleSheet("CCMainWindow");					//加载样式表，设置窗口的外观和风格
    initTitleBar(MIN_BUTTON); // 只显示最小化+关闭
    initControl();
	initTimer();
}

CCMainWindow::~CCMainWindow()
{}

void CCMainWindow::initControl()		//初始化控件
{
	//设置树形控件的样式，使用自定义的CoustomProxyStyle类来改变默认部件的外观和行为，例如去掉焦点框等
    ui.treeWidget->setStyle(new CoustomProxyStyle);
    //setLevelPixmap(0);

	setHeadPixmap(QPixmap(":/Resources/MainWindow/girl.png"));
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	QHBoxLayout* appupLayout = new QHBoxLayout;			//创建一个水平布局，用于放置应用部件
	appupLayout->setContentsMargins(0, 0, 0, 0);		//设置布局的边距为0
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png","app_7"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appupLayout->addStretch();		//添加一个弹性空间，使得其他应用部件靠左对齐
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
	appupLayout->setSpacing(2);		//设置布局中部件之间的间距为2像素
	ui.appWidget->setLayout(appupLayout);		//将应用部件的布局设置为appupLayout布局


	//在底部布局的上半部分添加其他应用部件，使用addOtherAppExtension函数来创建应用部件，并将其添加到布局中
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_1.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
	ui.bottomLayout_up->addStretch();//添加一个弹性空间，使得其他应用部件靠左对齐

	connect(ui.sysmin, &QPushButton::clicked, this, &CCMainWindow::onShowMin);
	connect(ui.sysclose, &QPushButton::clicked,this, &CCMainWindow::onShowClose);

	SysTray* sysTray = new SysTray(this);		//创建一个系统托盘对象，作为主窗口的子对象

}

void CCMainWindow::setUserName(const QString& name)
{
	ui.nameLabel->adjustSize();		//调整用户名标签的大小，使其适应文本内容
	//文本过长时，设置用户名标签的宽度为100像素，并将文本显示为省略号形式，保持界面整洁
	QString sname = ui.nameLabel->fontMetrics().elidedText(name, Qt::ElideRight,ui.nameLabel->width());
	ui.nameLabel->setText(sname);	//设置用户名标签的文本为传入的用户名
}

void CCMainWindow::setHeadPixmap(const QPixmap& headPath)
{
    QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");	//空的圆形头像图片
	//调用getRoundImage函数，获取圆形头像图片，并设置到头像标签上
	ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

void CCMainWindow::setLevelPixmap(int level)
{
	QPixmap levelPixmap(ui.levelBtn->size());   //创建一个与等级按钮大小相同的图像
	levelPixmap.fill(Qt::transparent);  //创建一个与等级按钮大小相同的透明图像

	QPainter painter(&levelPixmap);  //创建一个QPainter对象，使用等级图像作为绘制设备
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));  
    //将等级图像绘制到等级图像上，位置为(0, 0)

	int unitNum = level % 10;   //计算等级的个位数
	int tenNum = level / 10;    //计算等级的十位数

	//根据等级的个位数和十位数，计算出对应的数字图像在等级图像中的位置，并将其绘制到等级图像上
	//等级图像中数字图像的宽度为6像素，高度为7像素，十位数的数字图像在等级图像中的位置为(tenNum*6, 0)，个位数的数字图像在等级图像中的位置为(unitNum*6, 0)
	//参数说明：10和4分别是数字图像在等级图像上的位置，6和7分别是数字图像的宽度和高度
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),tenNum*6,0,6,7);
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

	ui.levelBtn->setIcon(levelPixmap);		//将等级图像设置为等级按钮的图标
	ui.levelBtn->setIconSize(levelPixmap.size());		//将等级图像的大小设置为等级按钮的图标大小
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	QPixmap statusBtnPixmap(ui.stausBtn->size());		
	statusBtnPixmap.fill(Qt::transparent);		//创建一个与状态按钮大小相同的透明图像
	QPainter painter(&statusBtnPixmap);		//创建一个QPainter对象，使用状态按钮图像作为绘制设备
	painter.drawPixmap(4, 4, QPixmap(statusPath));	//将状态图像绘制到状态按钮图像上，位置为(4, 4)，使状态图像在状态按钮的中心位置
	ui.stausBtn->setIcon(statusBtnPixmap);	//将状态图像设置为状态按钮的图标
	ui.stausBtn->setIconSize(statusBtnPixmap.size());	//将状态图像设置为状态按钮的图标，并将状态图像的大小设置为状态按钮的图标大小
}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);		//创建一个新的定时器对象，作为主窗口的子对象
	timer->setInterval(500);		//设置定时器的时间间隔为1000毫秒，即1秒钟
	connect(timer, &QTimer::timeout, [=]() {
		static int level = 0;
		if (level == 99)
		{
			level = 0;
		}
		setLevelPixmap(level);		//调用setLevelPixmap函数，更新等级图像
		level++;
		});
	timer->start();		//启动定时器，使其开始计时，并在每次时间间隔到达时触发timeout信号，执行连接的槽函数
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	QPushButton* btn = new QPushButton(this);		//创建一个新的按钮对象，作为应用部件的图标
	btn->setFixedSize(20, 20);

	QPixmap pixmap(btn->size());		
	pixmap.fill(Qt::transparent);		//创建一个与按钮大小相同的透明图像

	QPainter painter(&pixmap);		//创建一个QPainter对象，使用按钮图像作为绘制设备
	QPixmap appPixmap(appPath);		//加载应用图像
	painter.drawPixmap((btn->width() - appPixmap.width())/2, (btn->height() - appPixmap.height()) / 2,appPixmap);

	btn->setIcon(QPixmap(appPath));		//将应用图像设置为按钮的图标
	btn->setIconSize(btn->size());
	btn->setObjectName(appName);		//设置按钮的对象名称为应用名称，方便后续通过对象名称来访问和操作按钮
	btn->setProperty("hasborder", true);	
	//设置按钮的属性hasborder为true，表示按钮有边框，可以在样式表中根据这个属性来设置按钮的样式，例如显示边框或隐藏边框等", 1);

	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);
	//连接按钮的点击信号到主窗口的槽函数onAppIconClicked，当按钮被点击时会触发这个槽函数，执行相应的操作，例如打开应用窗口等

	return btn;
}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
	setUserName("极望-绿毛虫会超进化成裂空座");		//设置用户名
	BasicWindow::resizeEvent(event);		//调用父类的resizeEvent函数，进行默认的窗口大小调整操作
}

void CCMainWindow::onAppIconClicked()
{
	//获取发送信号的按钮对象，通过sender()函数来获取当前发送信号的对象，并将其转换为QPushButton类型的指针
	if (sender()->objectName() == "app_skin")
	{
		SkinWindow* skinWindow = new SkinWindow;		//创建一个皮肤窗口对象，作为主窗口的子窗口
		skinWindow->show();
	}
}
