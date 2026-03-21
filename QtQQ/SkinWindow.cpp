#include "SkinWindow.h"
#include "QClickLabel.h"
#include "NotifyManager.h"
SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	loadStyleSheet("SkinWindow");		//加载样式表，设置窗口的外观和风格
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏
	initControl();		//初始化控件
}

SkinWindow::~SkinWindow()
{}

void SkinWindow::initControl()
{
	QList<QColor> colorList =		//定义一个QList，存储12种颜色的QColor对象，这些颜色将用于皮肤选择
	{
		QColor(22,154,218),QColor(40,138,221),QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),QColor(89,92,160),QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77),QColor(240,188,189)
	};
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			QClickLabel* label = new QClickLabel(this);	//创建一个QClickLabel对象，作为皮肤颜色选择的标签
			label->setCursor(Qt::PointingHandCursor);	//设置标签的鼠标指针为手形，表示可点击
			//lambda表达式捕获row、column和colorList变量，定义点击事件的处理逻辑
			connect(label, &QClickLabel::clicked, [row, column, colorList]() {
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList[row * 4 + column]);		
				//通过NotifyManager的单例实例，调用notifyOtherWindowChangeSkin函数，通知其他窗口改变皮肤颜色
				});		//连接点击信号到槽函数，改变窗口背景颜色
			label->setFixedSize(84, 84);		//设置标签的大小

			QPalette palette;
			palette.setColor(QPalette::Window, colorList[row * 4 + column]);		
			//设置标签的背景颜色为colorList中的颜色
			label->setAutoFillBackground(true);		//启用自动填充背景
			label->setPalette(palette);		//将调色板应用到标签上，使其显示为指定的颜色
			ui.gridLayout_2->addWidget(label, row, column);		
			//将标签添加到网格布局中，位置由row和column确定
		}
	}
	connect(ui.sysmin, &QPushButton::clicked, this, &SkinWindow::onShowMin);		//连接最小化按钮的点击信号到槽函数
	connect(ui.sysclose, &QPushButton::clicked, this, &SkinWindow::onShowClose);	//连接关闭按钮的点击信号到槽函数
}

void SkinWindow::onShowClose()
{
	this->close();		//关闭窗口
}


