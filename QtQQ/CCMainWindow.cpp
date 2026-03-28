#include "CCMainWindow.h"
#include <QProxyStyle>
#include <QPainter>
#include "SkinWindow.h"
#include <QTimer>
#include <QEvent>
#include "SysTray.h"
#include "NotifyManager.h"
#include <QTreeWidgetItem>
#include "RootContactItem.h"
#include "ContactItem.h"
#include "WindowManager.h"
#include "TalkWindowShell.h"
#include <QApplication>
#include <QSqlQuery>

extern QString gLoginEmployeeID;		//全局变量，保存登录员工的ID，例如在登录成功后将员工ID保存到这个变量中，以便在后续的操作中使用，例如在获取员工信息时可以通过这个变量来查询数据库等

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

CCMainWindow::CCMainWindow(QString account, bool isAccountLOgin, QWidget *parent)
	: BasicWindow(parent), m_account(account), m_isAccountLogin(isAccountLOgin)
{
    ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool);		//设置窗口为工具窗口，工具窗口通常具有较小的标题栏和边框，并且在任务栏中不显示
	loadStyleSheet("CCMainWindow");					//加载样式表，设置窗口的外观和风格
    initTitleBar(MIN_BUTTON); // 只显示最小化+关闭
	setHeadPixmap(getHeadPicturePath());
	
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

	//setHeadPixmap(QPixmap(":/Resources/MainWindow/girl.png"));
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

	initContactTree();//初始化联系人树形控件，设置树形控件的内容和结构，例如添加联系人分组、联系人等

	ui.lineEdit->installEventFilter(this);		//安装事件过滤器，使得主窗口能够捕获lineEdit的事件，例如鼠标点击事件等
	ui.searchLineEdit->installEventFilter(this);	//安装事件过滤器，使得主窗口能够捕获searchWidget的事件，例如鼠标点击事件等

	connect(ui.sysmin, &QPushButton::clicked, this, &CCMainWindow::onShowMin);
	connect(ui.sysclose, &QPushButton::clicked,this, &CCMainWindow::onShowClose);

	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [=]() {
		updateSearchStyle();		//连接NotifyManager的signalSkinChanged信号到updateSearchStyle槽函数，当皮肤改变时更新搜索框的样式
		});//不连接会有bug，不会立即改变搜索框的样式，只有在下一次触发事件时才会改变

	SysTray* sysTray = new SysTray(this);		//创建一个系统托盘对象，作为主窗口的子对象

}

QString CCMainWindow::getHeadPicturePath()
{
	QString strPicturePath;
	if (!m_isAccountLogin)	//qq
	{
		QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(m_account));
		queryPicture.exec();
		queryPicture.next();
		strPicturePath = queryPicture.value(0).toString();		//从数据库中查询员工的头像路径，并将其保存到strPicturePath变量中
	}
	else	//账号
	{
		QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'").arg(m_account));
		queryEmployeeID.exec();
		queryEmployeeID.next();
		int employeeID = queryEmployeeID.value(0).toInt();		//从数据库中查询员工的头像路径，并将其保存到strPicturePath变量中

		QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));
		queryPicture.exec();
		queryPicture.next();
		strPicturePath = queryPicture.value(0).toString();		//从数据库中查询员工的头像路径，并将其保存到strPicturePath变量中
	}

	return strPicturePath;
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
	QString UserName;
	if (!m_isAccountLogin)	//qq
	{
		QSqlQuery queryName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(m_account));
		queryName.exec();
		queryName.next();
		UserName = queryName.value(0).toString();		
	}
	else	//账号
	{
		QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'").arg(m_account));
		queryEmployeeID.exec();
		queryEmployeeID.next();
		int employeeID = queryEmployeeID.value(0).toInt();		//从数据库中查询员工的头像路径，并将其保存到strPicturePath变量中

		QSqlQuery queryName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
		queryName.exec();
		queryName.next();
		UserName = queryName.value(0).toString();		//从数据库中查询员工的头像路径，并将其保存到strPicturePath变量中
	}
	setUserName(UserName);		//设置用户名
	BasicWindow::resizeEvent(event);		//调用父类的resizeEvent函数，进行默认的窗口大小调整操作
}


void CCMainWindow::updateSearchStyle()
{
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50); border-bottom:1px solid rgba(%1,%2,%3,30)}\
											QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png);}")
											.arg(m_backGroundColor.red())
											.arg(m_backGroundColor.green())
											.arg(m_backGroundColor.blue()));
}


bool CCMainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (ui.searchLineEdit == watched)
	{
		if (event->type() == QEvent::FocusIn)	//如果事件类型是获取焦点事件，即用户点击了搜索输入框
		{
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255); border-bottom:1px solid rgba(%1,%2,%3,100)}\
													QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png);}\
													QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png);}\
													QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png);}")
													.arg(m_backGroundColor.red())
													.arg(m_backGroundColor.green())
													.arg(m_backGroundColor.blue()));
			//将搜索部件的样式表设置为白色背景，表示搜索输入框处于激活状态,并且设置底部边框为1像素，突出显示搜索输入框
		}
		else if (event->type() == QEvent::FocusOut)
		{
			updateSearchStyle();		//调用updateSearchStyle函数，更新搜索部件的样式，使其恢复到默认状态
		}
	}
	return false;
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

void CCMainWindow::initContactTree()
{
	//初始化联系人树形控件，设置树形控件的内容和结构，例如添加联系人分组、联系人等
	//展开与收缩的信号
	connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &CCMainWindow::onItemClicked);	
	//当树形控件中的项目被点击时会触发这个槽函数，执行相应的操作，例如显示联系人信息等
	connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &CCMainWindow::onItemDoubleClicked);
	//当树形控件中的项目被双击时会触发这个槽函数，执行相应的操作，例如打开聊天窗口等
	connect(ui.treeWidget, &QTreeWidget::itemExpanded, this, &CCMainWindow::onItemExpanded);	
	//当树形控件中的项目被展开时会触发这个槽函数，执行相应的操作，例如加载子项目等
	connect(ui.treeWidget, &QTreeWidget::itemCollapsed, this, &CCMainWindow::onItemCollapsed);	
	//当树形控件中的项目被收缩时会触发这个槽函数，执行相应的操作，例如释放子项目等
	
	//初始化根节点
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem(ui.treeWidget);
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);		//设置根节点显示展开指示器，即使没有子节点也显示展开指示器
	pRootGroupItem->setData(0, Qt::UserRole, 0);	//设置根节点的数据，使用Qt::UserRole作为数据的角色，值为0，表示根节点的类型或标识

	RootContactItem* pItemName = new RootContactItem(true,ui.treeWidget);

	//获取部门ID
	QSqlQuery queryComDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg("公司群"));		//执行SQL查询，获取部门ID
	queryComDepID.exec();
	queryComDepID.first();
	int ComDepID = queryComDepID.value(0).toInt();		//将查询结果的第一行第一列的值转换为整数，作为部门ID

	//登录者所在部门的ID(部门群号)
	QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));		//执行SQL查询，获取部门ID
	querySelfDepID.exec();
	querySelfDepID.first();		//第一行
	int SelfDepID = querySelfDepID.value(0).toInt();		//将查询结果的第一行的第一列的值转换为整数，作为部门ID


	addCompanyDeps(pRootGroupItem, ComDepID);
	addCompanyDeps(pRootGroupItem, SelfDepID);

	QString strGroupName = "神人科技";		//设置联系人分组的名称
	pItemName->setText(strGroupName);		//设置联系人分组节点的文本
	
	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootGroupItem);		//将联系人分组节点插入到树形控件的顶层位置，位置索引为0，即第一个位置
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);		
	//将联系人分组节点的文本设置为pItemName，将其显示在树形控件中

	//QStringList sCompDeps;	
	//sCompDeps << "公司" << "人事" << "研发" << "市场";
	//for (int i = 0; i < sCompDeps.size(); i++)
	//{
	//	addCompanyDeps(pRootGroupItem,sCompDeps.at(i));
	//}

	
}
void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;
	

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");

	pChild->setData(0, Qt::UserRole, 1);	//设置子节点的数据，使用Qt::UserRole作为数据的角色，值为1，表示部门节点的类���或标识
	pChild->setData(0, Qt::UserRole + 1, DepID);//设置子节点的额外数据，使用Qt::UserRole + 1作为数据的角色，值为部门节点的唯一标识，这里使用部门节点的内存地址转换为字符串来表示

	//获取部门头像
	QPixmap groupPix;
	QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID));		//执行SQL查询，获取部门头像路径
	queryPicture.exec();
	queryPicture.first();
	groupPix.load(queryPicture.value(0).toString());		//加载部门头像图像
	//获取部门名称
	QString strDepName;
	QSqlQuery queryDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(DepID));
	queryDepName.exec();
	queryDepName.first();
	strDepName = queryDepName.value(0).toString();		//将查询结果的第一行第一列的值转换为字符串，作为部门名称

	ContactItem* pContactItem = new ContactItem(ui.treeWidget);
	pContactItem->setHeadPixmap(getRoundImage(groupPix,pix,pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);		//设置部门节点的文本为部门名称

	pRootGroupItem->addChild(pChild);		//将部门节点添加到根节点的子节点列表中
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);		//将部门节点的文本设置为pContactItem，将其显示在树形控件中

	//m_groupMap.insert(pChild, strDepName);		//将部门节点和部门名称的映射关系插入到m_groupMap中，方便后续通过部门节点来获取部门名称
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();		//判断被点击的项目是否是子节点，即部门节点，通过检查项目的数据来确定
	if (!bIsChild)
	{
		item->setExpanded(!item->isExpanded());	//如果被点击的项目不是子节点，即根节点，则切换其展开状态，即如果当前是展开状态则收缩，如果当前是收缩状态则展开

	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();		//判断被点击的项目是否是子节点，即部门节点，通过检查项目的数据来确定
	if (!bIsChild)
	{
		//dynamic_cast是C++中的一个运算符，用于在类层次结构中进行安全的类型转换。它可以将一个指针或引用转换为另一个类型的指针或引用，如果转换不合法则返回nullptr或抛出异常。
		//在这里，使用dynamic_cast将被点击的项目的文本转换为RootContactItem类型的指针，以获取根节点的文本对象，从而设置其展开状态
		//将基类对象指针转换为派生类对象指针
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));		//将被点击的项目的文本转换为RootContactItem类型的指针，获取根节点的文本对象
		if(pRootItem)
			pRootItem->setExpanded(true);		//如果被点击的项目不是子节点，即根节点，则设置其展开状态为true，即展开
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();		//判断被点击的项目是否是子节点，即部门节点，通过检查项目的数据来确定
	if (!bIsChild)
	{
		RootContactItem* pRootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));		//将被点击的项目的文本转换为RootContactItem类型的指针，获取根节点的文本对象
		pRootItem->setExpanded(false);		//如果被点击的项目不是子节点，即根节点，则设置其展开状态为false，即收缩
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();		//判断被双击的项目是否是子节点，即部门节点，通过检查项目的数据来确定
	if (bIsChild)
	{
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());

		//QString strGroup = m_groupMap.value(item);		//获取被双击的项目的父节点的数据，即部门节点的数据，作为分组ID
		//if (strGroup == "公司")
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole+1).toString(),COMPANY);
		//	//调用WindowManager的getInstance函数获取WindowManager的单例对象，并调用函数来显示聊天窗口，参数为分组ID，即部门名称
		//}
		//else if (strGroup == "人事")
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		//}
		//else if (strGroup == "研发")
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		//}
		//else if (strGroup == "市场")
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		//}

	}
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus())
	{
		ui.searchLineEdit->clearFocus();	//当用户点击了窗口的其他区域时，清除搜索输入框的焦点，使其失去激活状态
	}
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
	{
		ui.lineEdit->clearFocus();		//当用户点击了窗口的其他区域时，清除用户名输入框的焦点，使其失去激活状态
	}
	BasicWindow::mousePressEvent(event);		//调用父类的mousePressEvent函数，进行默认的鼠标点击事件处理操作，例如拖动窗口等
}

