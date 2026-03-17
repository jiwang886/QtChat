#include "TitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFile>
#define BUTTON_HEIGHT 27				//标题栏按纽的高度
#define BUTTON_WIDTH  27				//标题栏按纽的宽度
#define TITLE_HEIGHT  27				//标题栏的高度


TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent), m_isPressed(false), m_buttonType(MIN_MAX_BUTTON) //默认是最小化、最大化、关闭按钮
{
	initControl();					//初始化标题栏控件
	loadStyleSheet("Title");		//加载标题栏样式表	
	initConnections();				//初始化标题栏控件信号与槽的连接
}
TitleBar::~TitleBar()
{}

void TitleBar::initControl()			//初始化标题栏控件
{
	m_pIcon = new QLabel(this);				//初始化标题栏图标
	m_pTitleContent = new QLabel(this);		//初始化标题栏内容

	m_pButtonMin = new QPushButton(this);			//最小化按纽
	m_pButtonMax = new QPushButton(this);			//最大化按纽
	m_pButtonRestore = new QPushButton(this);		//还原按纽
	m_pButtonClose = new QPushButton(this);			//关闭按纽

	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));		//设置最小化按纽的大小
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));		//设置最大化按纽的大小
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));	//设置还原按纽的大小
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));	//设置关闭按纽的大小

	//给控件起一个能在代码其他地方被找到的名字
	m_pTitleContent->setObjectName("TitleContent");		//设置标题栏内容的对象名称，用于样式表设置
	m_pButtonMin->setObjectName("ButtonMin");			//设置最小化按纽的对象名称，用于样式表设置
	m_pButtonMax->setObjectName("ButtonMax");			//设置最大化按纽的对象名称，用于样式表设置
	m_pButtonRestore->setObjectName("ButtonRestore");	//设置还原按纽的对象名称，用于样式表设置
	m_pButtonClose->setObjectName("ButtonClose");		//设置关闭按纽的对象名称，用于样式表设置

	QHBoxLayout* mylayout = new QHBoxLayout(this);		//水平布局
	mylayout->addWidget(m_pIcon);				//添加标题栏图标到布局
	mylayout->addWidget(m_pTitleContent);		//添加标题栏内容到布局
	mylayout->addWidget(m_pButtonMin);			//添加最小化按纽到布局
	mylayout->addWidget(m_pButtonMax);			//添加最大化按纽到布局
	mylayout->addWidget(m_pButtonRestore);		//添加还原按纽到布局
	mylayout->addWidget(m_pButtonClose);			//添加关闭按纽到布局

	mylayout->setContentsMargins(5, 0, 0, 0);	//设置布局边距，左边距为5，其他为0
	mylayout->setSpacing(0);					//设置布局间距为0,使标题栏内容和按纽紧密排列

	//设置标题栏内容的大小策略，使其在水平上扩展，在垂直上固定
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);	
	setFixedHeight(TITLE_HEIGHT);					//设置标题栏的固定高度
	setWindowFlags(Qt::FramelessWindowHint);			//设置窗口无边框
}

void TitleBar::initConnections()		//初始化标题栏控件信号与槽的连接
{
	connect(m_pButtonMin, &QPushButton::clicked, this, &TitleBar::onButtonMinClicked);			//连接最小化按纽的点击信号到槽函数
	connect(m_pButtonMax, &QPushButton::clicked, this, &TitleBar::onButtonMaxClicked);			//连接最大化按纽的点击信号到槽函数
	connect(m_pButtonRestore, &QPushButton::clicked, this, &TitleBar::onButtonRestoreClicked);	//连接还原按纽的点击信号到槽函数
	connect(m_pButtonClose, &QPushButton::clicked, this, &TitleBar::onButtonCloseClicked);		//连接关闭按纽的点击信号到槽函数
}

void TitleBar::setTitleIcon(const QString &filePath)		//设置标题栏图标
{
	QPixmap pixmap(filePath);			//加载图标文件
	m_pIcon->setPixmap(pixmap);		//设置标题栏图标
}	

void TitleBar::setTitleContent(const QString& titleContent)			//设置标题栏内容
{
	m_pTitleContent->setText(titleContent);	//设置标题栏内容
	m_titleContent = titleContent;			//保存标题栏内容
}

void TitleBar::setTitleWidth(int widget)					//设置标题栏长度
{
	setFixedWidth(widget);				//设置标题栏的固定宽度
}

void TitleBar::setButtonType(ButtonType buttonType)	//设置标题栏按纽类型
{
	m_buttonType = buttonType;			//保存标题栏按纽类型
	switch (buttonType)
	{
	case MIN_BUTTON:					//最小化和关闭按纽
		m_pButtonMax->setVisible(0);			//隐藏最大化按纽
		m_pButtonRestore->setVisible(0);		//隐藏还原按纽
		break;
	case MIN_MAX_BUTTON:					//最小化、最大化、关闭按钮
		m_pButtonRestore->setVisible(0);		//隐藏还原按纽
		break;
	case ONLY_CLOSE_BUTTON:				//只有关闭按钮 
		m_pButtonMin->setVisible(0);			//隐藏最小化按纽
		m_pButtonMax->setVisible(0);			//隐藏最大化按纽
		m_pButtonRestore->setVisible(0);		//隐藏还原按纽
	default:
		break;
	}
}

void TitleBar::saveRestoreInfo(const QPoint &point,const QSize &size)	//保存窗口大小(最大化前的位置和大小)
{
	m_restorePos = point;			//保存窗口位置
	m_restoreSize = size;			//保存窗口大小
}
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)	//获取窗口大小(最大化前的位置和大小)
{
	point = m_restorePos;			//获取窗口位置
	size = m_restoreSize;			//获取窗口大小
}

void TitleBar::paintEvent(QPaintEvent* event)				//重绘事件
{
	QPainter painter(this);			//创建QPainter对象，指定绘制设备为当前窗口
	QPainterPath pathBace;					//创建QPainterPath对象，用于绘制路径
	pathBace.setFillRule(Qt::WindingFill);		//设置填充规则为非零环绕填充
	pathBace.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);	//添加一个圆角矩形到路径，参数为矩形位置和大小，以及圆角半径
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);	//启用平滑像素变换渲染提示，使图像在缩放时更平滑

	if (width() != parentWidget()->width())	//如果标题栏宽度不等于父窗口宽度
	{
		setFixedWidth(parentWidget()->width());		//如果标题栏宽度不等于父窗口宽度，则设置标题栏宽度为父窗口宽度
	}
	QWidget::paintEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)	//鼠标双击事件
{
	if (m_buttonType == MIN_MAX_BUTTON)		//如果标题栏按纽类型是最小化、最大化、关闭按钮
	{
		//if (parentWidget()->isMaximized())	//如果父窗口已经最大化
		//{
		//	onButtonRestoreClicked();			//调用还原按纽的槽函数，恢复窗口到最大化前的状态
		//}
		//else								//如果父窗口没有最大化
		//{
		//	onButtonMaxClicked();				//调用最大化按纽的槽函数，将窗口最大化
		//}
		if(m_pButtonMax->isVisible())
			onButtonRestoreClicked();				//调用关闭按纽的槽函数，恢复窗口到最大化前的状态
		else
			onButtonMaxClicked();				//调用最大化按纽的槽函数，将窗口最大化
	}
	return QWidget::mouseDoubleClickEvent(event);	
	//调用基类的鼠标双击事件处理函数，确保其他事件处理正常进行
}
void TitleBar::mousePressEvent(QMouseEvent* event)		//鼠标按下事件
{
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		if (m_pButtonMax->isVisible())
		{
			m_isPressed = true;				//设置鼠标按下状态为true
			m_startMovePos = event->globalPosition().toPoint();	//记录鼠标按下时，鼠标在屏幕上的位置，全局坐标
		}
	}
	else
	{
		m_isPressed = true;				//设置鼠标按下状态为true
		m_startMovePos = event->globalPosition().toPoint();	//记录鼠标按下时，鼠标在屏幕上的位置，全局坐标
	}
	return QWidget::mousePressEvent(event);
}
void TitleBar::mouseMoveEvent(QMouseEvent* event)		//鼠标移动事件
{
	if (m_isPressed)					//如果鼠标按下状态为true
	{
		QPoint movePos = event->globalPosition().toPoint() - m_startMovePos;	//计算鼠标移动的距离，当前鼠标位置减去开始移动时的鼠标位置
		QPoint widgetPos = parentWidget()->pos();				//获取父窗口的位置
		parentWidget()->move(widgetPos + movePos);			//将父窗口移动到新的位置，原位置加上移动距离
		m_startMovePos = event->globalPosition().toPoint();					//更新开始移动时的鼠标位置为当前鼠标位置
	}
	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)	//鼠标释放事件
{
	m_isPressed = false;				//设置鼠标按下状态为false
	return QWidget::mouseReleaseEvent(event);
}

void TitleBar::loadStyleSheet(const QString &sheetName)				//加载标题栏样式表
{
	QFile file(":/Resources/QSS/" + sheetName + ".css");		//创建QFile对象，指定样式表文件路径
	if (file.open(QFile::ReadOnly))			//尝试以只读方式打开文件，如果成功
	{
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());	//读取文件内容，并转换为QString对象
		this->setStyleSheet(styleSheet);			//将样式表应用到当前窗口
		file.close();						//关闭文件
	}
}

void TitleBar::onButtonMinClicked()		//最小化按纽的槽函数
{
	emit signalButtonMinClicked();			//发出最小化按纽点击信号，通知父窗口进行最小化操作
}

void TitleBar::onButtonMaxClicked()		//最大化按纽的槽函数
{
	emit signalButtonMaxClicked();			//发出最大化按纽点击信号，通知父窗口进行最大化操作
	m_pButtonMax->setVisible(0);			//隐藏最大化按纽
	m_pButtonRestore->setVisible(1);		//显示还原按纽
}
void TitleBar::onButtonRestoreClicked()	//还原按纽的槽函数
{
	emit signalButtonRestoreClicked();		//发出还原按纽点击信号，通知父窗口进行还原操作
	m_pButtonMax->setVisible(1);			//显示最大化按纽
	m_pButtonRestore->setVisible(0);		//隐藏还原按纽
}
void TitleBar::onButtonCloseClicked()		//关闭按纽的槽函数
{
	emit signalButtonCloseClicked();		//发出关闭按纽点击信号，通知父窗口进行关闭操作
}