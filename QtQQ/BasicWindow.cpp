#include "BasicWindow.h"
#include <QFile>
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>
#include <QApplication>
#include <QMouseEvent>
#include "CommonUtils.h"
#include "NotifyManager.h"
BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent), mousePressed(false), m_TitleBar(nullptr)
{
	m_backGroundColor = CommonUtils::getDefaultSkinColor();		//获取默认皮肤颜色
	//setAttribute(Qt::WA_TranslucentBackground, true);	//设置窗口背景透明
	setWindowFlags(Qt::FramelessWindowHint);		//设置窗口无边框
	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor &)), this, SLOT(onSignalSkinChanged(const QColor &)));
}

BasicWindow::~BasicWindow()
{
}
void BasicWindow::initTitleBar(ButtonType buttontype)	//初始化标题栏
{
	m_TitleBar = new TitleBar(this);
	m_TitleBar->setButtonType(buttontype);
	m_TitleBar->move(0, 0);
	//m_TitleBar->setTitleWidth(this->width());
	connect(m_TitleBar, &TitleBar::signalButtonMinClicked, this, &BasicWindow::onButtonMinClicked);
	connect(m_TitleBar, &TitleBar::signalButtonMaxClicked, this, &BasicWindow::onButtonMaxClicked);
	connect(m_TitleBar, &TitleBar::signalButtonRestoreClicked, this, &BasicWindow::onButtonRestoreClicked);
	connect(m_TitleBar, &TitleBar::signalButtonCloseClicked, this, &BasicWindow::onButtonCloseClicked);
}
void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	m_TitleBar->setTitleIcon(icon);			//设置标题栏图标
	m_TitleBar->setTitleContent(title);		//设置标题栏内容
}

void BasicWindow::onSignalSkinChanged(const QColor& color)	//接收信号，改变窗口背景颜色的槽函数
{
	m_backGroundColor = color;				//改变背景颜色
	loadStyleSheet(m_styleName);			//加载样式表
}
void BasicWindow::loadStyleSheet(const QString& sheetName)	//加载样式表，用于设置窗口的外观和风格
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	if (file.open(QFile::ReadOnly)) {
		QString qsstyleSheet = QLatin1String(file.readAll());
		QString r = QString::number(m_backGroundColor.red());		//将背景颜色的RGB值转换为字符串
		QString g = QString::number(m_backGroundColor.green());	
		QString b = QString::number(m_backGroundColor.blue());

		qsstyleSheet += QString("QWidget[titleskin=true]\
								{background-color:rgb(%1,%2,%3);\
								border-top-left-radius:4px;}\
								QWidget[bottomskin=true]\
								{border-top:1px solid rgba(%1,%2,%3,100);\
								background-color:rgba(%1,%2,%3,50);\
								border-bottom-left-radius:4px;\
								border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		setStyleSheet(qsstyleSheet);		//设置样式表样
	}
	file.close();
}
//初始画背景图	
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;		//创建一个QStyleOption对象，用于描述绘制选项
	opt.initFrom(this);		//初始化选项对象，使用当前窗口作为参数，设置选项的状态和属性
	QPainter p(this);		//创建一个QPainter对象，使用当前窗口作为绘制设备
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);		
	//调用当前窗口的样式对象的drawPrimitive函数，绘制基本元素，参数分别是：元素类型（PE_Widget表示绘制一个普通的窗口部件），选项对象，绘制设备和当前窗口
}
//子类化部件时，需要重写paintEvent函数，在其中调用initBackGroundColor函数来绘制背景颜色
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();				//初始化背景颜色
	QDialog::paintEvent(event);			//调用父类的paintEvent函数，进行默认的绘制操作
}
//获取圆头像
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap &mask, QSize masksize)
{
	//QPixmap dest(src.size());			//创建一个与源图像大小相同的目标图像
	//dest.fill(Qt::transparent);			//将目标图像填充为透明
	//QPainter painter(&dest);				//创建一个QPainter对象，使用目标图像作为绘制设备
	//painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);	//设置抗锯齿和高质量的图像变换渲染提示
	//QPainterPath path;					//创建一个QPainterPath对象，用于定义绘制路径
	//path.addEllipse(dest.rect());		//在路径中添加一个椭圆，覆盖整个目标图像区域
	//painter.setClipPath(path);			//将绘制区域限制为路径定义的形状，即圆形区域
	//painter.drawPixmap(0, 0, src);		//将源图像绘制到目标图像上，位置为(0, 0)
	//mask = dest.createMaskFromColor(Qt::transparent);	//从目标图像中创建一个掩码，使用透明颜色作为掩码颜色
	//return dest;						//返回处理后的圆形图像
	if (masksize == QSize(0, 0))	
	{
		masksize = mask.size();			//如果掩码大小为(0, 0)，则使用源图像的大小作为掩码大小
	}
	else
	{
		mask = mask.scaled(masksize,Qt::KeepAspectRatio,Qt::SmoothTransformation);				
		//否则，将掩码缩放到指定的大小
	}

	QImage resultImage(masksize, QImage::Format_ARGB32_Premultiplied);	//创建一个与掩码大小相同的结果图像，使用ARGB32格式
	QPainter painter(&resultImage);				//创建一个QPainter对象，使用结果图像作为绘制设备
	painter.setCompositionMode(QPainter::CompositionMode_Source);	//设置绘制模式为源模式，即直接覆盖目标图像
	painter.fillRect(resultImage.rect(), Qt::transparent);	//将结果图像填充为透明
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);	//设置绘制模式为源覆盖，即在目标图像上绘制源图像
	painter.drawPixmap(0, 0, mask);		//将源图像绘制到结果图像上，位置为(0, 0)
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);	//设置绘制模式为目标内，即将结果图像与掩码进行合成，保留掩码区域内的像素
	painter.drawPixmap(0, 0, src.scaled(masksize,Qt::KeepAspectRatio, Qt::SmoothTransformation));	//将掩码缩放到结果图像大小，并绘制到结果图像上，位置为(0, 0)
	painter.end();						//结束绘制操作
	return QPixmap::fromImage(resultImage);	//将结果图像转换为QPixmap并返回
}
void BasicWindow::onShowClose(bool)
{
	close();					//关闭窗口
}
void BasicWindow::onShowMin(bool)
{
	this->showMinimized();				//将窗口最小化
}
void BasicWindow::onShowHide(bool)
{
	this->hide();					//隐藏窗口
}
void BasicWindow::onShowNormal(bool)
{
	show();
	activateWindow();					//显示窗口并激活它
}
void BasicWindow::onShowQuit(bool)
{
	QApplication::quit();				//退出应用程序
}

void BasicWindow::mousePressEvent(QMouseEvent* event)		//鼠标按下事件
{
	if (event->buttons() == Qt::LeftButton)
	{
		mousePressed = true;				//设置鼠标按下标志为true
		//event->globalPosition()获取事件发生时鼠标在屏幕(0,0)上的位置，toPoint()将其转换为QPoint类型
		//this->pos()获取窗口在屏幕上的位置，计算鼠标按下时的位置与窗口位置的差值，存储在mousePoint变量中
		mousePoint = event->globalPosition().toPoint() - this->pos();	//计算鼠标按下时的位置与窗口位置的差值，存储在mousePoint变量中
		//获取的时窗口左上角的位置
		event->accept();					//接受事件，防止事件继续传播
	}
}
void BasicWindow::mouseMoveEvent(QMouseEvent* event)		//鼠标移动事件
{
	if (mousePressed && (event->buttons() && Qt::LeftButton))
	{
		//event->globalPosition()获取事件发生时鼠标在屏幕(0,0)上的位置，toPoint()将其转换为QPoint类型
		//相减得到新的窗口位置，即鼠标当前位置减去鼠标按下时的位置，调用move()函数移动窗口到新的位置
		move(event->globalPosition().toPoint() - mousePoint);	//移动窗口，新的位置为鼠标当前位置减去鼠标按下时的位置
		event->accept();					//接受事件，防止事件继续传播
	}
}
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)	//鼠标释放事件
{
	mousePressed = false;				//设置鼠标按下标志为false
}
void BasicWindow::mouseDoubleClickEvent(QMouseEvent* event)	//鼠标双击事件
{
	if (this->isMaximized())	//如果父窗口已经最大化
		{
			onButtonRestoreClicked();			//调用还原按纽的槽函数，恢复窗口到最大化前的状态
		}
		else								//如果父窗口没有最大化
		{
			onButtonMaxClicked();				//调用最大化按纽的槽函数，将窗口最大化
		}
}


void BasicWindow::onButtonMinClicked()			//最小化按纽被点击的槽函数
{
	if (Qt::Tool == (windowFlags() & Qt::Tool))
	{
		hide();
	}
	else
	{
		showMinimized();				//将窗口最小化
	}
}
void BasicWindow::onButtonMaxClicked()			//最大化按纽被点击的槽函数
{
	m_TitleBar->saveRestoreInfo(pos(), QSize(width(), height()));
	//保存窗口还原时的位置和大小信息，使用当前窗口的位置和大小

	QRect desktopRect = QApplication::primaryScreen()->availableGeometry();	//获取桌面可用区域的几何信息
	QRect factRact = QRect(desktopRect.x()-3, desktopRect.y()-3, desktopRect.width()+6, desktopRect.height()+6);	//创建一个矩形，表示窗口最大化时的几何信息，使用桌面可用区域的坐标和大小
	setGeometry(factRact);					//将窗口的几何形状设置为最大化时的几何信息
}
void BasicWindow::onButtonRestoreClicked()		//还原按纽被点击的槽函数
{
	QPoint windowPos;
	QSize windowSize;
	m_TitleBar->getRestoreInfo(windowPos, windowSize);	//获取窗口还原时的位置和大小信息
	setGeometry(QRect(windowPos, windowSize));		//将窗口的几何形状设置为还原时的位置和大小
}
void BasicWindow::onButtonCloseClicked()		//关闭按纽被点击的槽函数
{
	this->close();					//关闭窗口
}