#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
enum ButtonType					//用于自定义消息对话框时定义的枚举
{
	MIN_BUTTON = 0,				//最小化和关闭按纽
	MIN_MAX_BUTTON,					//最小化、最大化、关闭按钮
	ONLY_CLOSE_BUTTON				//只有关闭按钮 
};

//自定义标题栏
class TitleBar  : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget* parent = NULL);		//将父窗口指针传入标题栏构造函数，方便在标题栏中操作父窗口
	~TitleBar();

	void setTitleIcon(const QString& filePath);		//设置标题栏图标，传入图标文件路径
	void setTitleContent(const QString& titleContent);			//设置标题栏内容，传入标题内容字符串
	void setTitleWidth(int widget);					//设置标题栏宽度
	void setButtonType(ButtonType buttonType);	//设置标题栏按纽类型
	//保存窗口大小(最大化前的位置和大小)
	void saveRestoreInfo(const QPoint &point,const QSize &size);	
	//获取窗口大小(最大化前的位置和大小)
	void getRestoreInfo(QPoint& point, QSize& size);					
	
private:
	void paintEvent(QPaintEvent* event);				//重绘事件
	void mousePressEvent(QMouseEvent* event);		//鼠标按下事件
	void mouseMoveEvent(QMouseEvent* event);		//鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event);	//鼠标释放事件
	void mouseDoubleClickEvent(QMouseEvent* event);	//鼠标双击事件

	void initControl();					//初始化标题栏控件
	void initConnections();				//初始化标题栏控件信号与槽的连接
	void loadStyleSheet(const QString& sheetName);		//加载标题栏样式表，传入样式表文件名

signals:
	//以下是标题栏按纽被点击的信号，供外部使用
	void signalButtonMinClicked();			//最小化按纽被点击的信号
	void signalButtonMaxClicked();			//最大化按纽被点击的信号
	void signalButtonRestoreClicked();		//还原按纽被点击的信号
	void signalButtonCloseClicked();		//关闭按纽被点击的信号

private slots:
	//以下是标题栏按纽被点击的槽函数，供内部使用
	void onButtonMinClicked();			//最小化按纽被点击的槽函数
	void onButtonMaxClicked();			//最大化按纽被点击的槽函数
	void onButtonRestoreClicked();		//还原按纽被点击的槽函数
	void onButtonCloseClicked();		//关闭按纽被点击的槽函数

private:
	QLabel* m_pIcon;				//标题栏图标，存在标签里	m表示member，p表示pointer指针
	QLabel* m_pTitleContent;		//标题栏内容
	QPushButton* m_pButtonMin;			//最小化按纽
	QPushButton* m_pButtonMax;			//最大化按纽
	QPushButton* m_pButtonRestore;		//还原按纽
	QPushButton* m_pButtonClose;			//关闭按纽

	//最大化还原按钮变量，用于保存位置及大小
	QPoint m_restorePos;				//保存最大化前的位置
	QSize m_restoreSize;				//保存最大化前的大小

	//移动窗口变量
	bool m_isPressed;				//鼠标是否被按下
	QPoint m_startMovePos;			//鼠标开始移动的位置
	
	QString m_titleContent;			//标题栏内容
	ButtonType m_buttonType;				//标题栏按纽类型

};

