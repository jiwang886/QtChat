#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include <QListWidget>
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏	
	initControl();
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;		//删除表情窗口对象，释放内存
	m_emotionWindow = NULL;		//将表情窗口指针置空，避免悬空指针
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, GroupType grouptype)
{
	ui.rightStackedWidget->addWidget(talkWindow);		//将新的聊天窗口添加到右侧的堆叠窗口中
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);		//创建一个新的聊天窗口列表项
	m_talkwindowItemMap.insert(aItem, talkWindow);		//将聊天窗口列表项和对应的聊天窗口添加到映射关系中

	aItem->setSelected(1);		//设置新添加的聊天窗口列表项为选中状态，表示当前显示的聊天窗口

	talkWindowItem->setHeadPixmap(QPixmap(":"));	//设置聊天窗口列表项的头像图片，可以根据实际需求传入不同的图片路径或资源
	ui.listWidget->addItem(aItem);		//将新的聊天窗口列表项添加到左侧的聊天窗口列表中
	ui.listWidget->setItemWidget(aItem, talkWindowItem);		//将聊天窗口列表项设置为新的聊天窗口列表项对象，显示在聊天窗口列表中

	onTalkWindowItemClicked(aItem);		//调用聊天窗口列表项被点击的槽函数，切换到新添加的聊天窗口，显示该聊天窗口的内容

	connect(talkWindowItem, &TalkWindowItem::signalCloseBtnClicked, [talkWindowItem, talkWindow,aItem,this]() {		//连接聊天窗口列表项的关闭按钮被点击的信号和一个lambda函数，当关闭按钮被点击时执行这个函数，删除对应的聊天窗口和聊天窗口列表项
		m_talkwindowItemMap.remove(aItem);		//从映射关系中删除对应的聊天窗口列表项和聊天窗口的映射关系
		talkWindow->close();		//关闭对应的聊天窗口，触发窗口的关闭事件，执行相应的清理操作，例如释放资源等
		ui.listWidget->takeItem(ui.listWidget->row(aItem));	//从聊天窗口列表中移除对应的聊天窗口列表项，更新聊天窗口列表的显示
		delete talkWindowItem;		//删除聊天窗口列表项对象，释放内存
		ui.rightStackedWidget->removeWidget(talkWindow);		//从右侧的堆叠窗口中移除对应的聊天窗口，更新堆叠窗口的显示
		if (ui.rightStackedWidget->count() < 1)
			close();
		});

}

void TalkWindowShell::setCurrentTalkWindow(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);		//将右侧的堆叠窗口切换到指定的聊天窗口，显示该聊天窗口的内容
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap()
{
	return m_talkwindowItemMap;		//返回聊天窗口列表项和对应的聊天窗口的映射关系，供外部使用，例如在其他类中获取当前聊天窗口等操作
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");		//加载样式表，设置窗口的外观和风格
	setWindowTitle("聊天窗口");		//设置窗口标题
	
	m_emotionWindow = new EmotionWindow(this);		//创建表情窗口对象，并将当前窗口作为父窗口
	m_emotionWindow->hide();		//默认隐藏表情窗口

	QList<int> leftWidgetSize;	//创建一个整数列表，用于设置左侧控件的大小
	leftWidgetSize << 154 << width() - 154;		//将左侧控件的大小设置为154像素，右侧控件的大小为窗口宽度减去154像素
	ui.splitter->setSizes(leftWidgetSize);	//将设置好的大小列表应用到分割器控件上，调整左右控件的大小比例
	
	ui.leftWidget->setStyle(new CustomPoxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	//连接聊天窗口列表项被点击的信号和槽函数

	connect(m_emotionWindow,SIGNAL(signalEmotionItemClicked(int)), this,SLOT(onEmotionBtnClicked(int)));
	//连接表情窗口的表情项被点击的信号和槽函数，当表情项被点击时会触发这个槽函数，执行相应的操作，例如将表情添加到当前聊天窗口中等
}

void TalkWindowShell::onEmotionBtnClicked(int emotionNum)
{
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());		//获取当前显示的聊天窗口，并将其转换为TalkWindow类型的指针
	if (curTalkWindow)
	{
		curTalkWindow->addEmotionImage(emotionNum);		//如果当前聊天窗口存在，则调用其addEnotionImage函数，添加表情图片到聊天窗口中
	}

}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());		//切换表情窗口的可见状态，如果当前不可见则显示，反之则隐藏
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));		//获取当前窗口在屏幕上的全局位置，转换为全局坐标系中的点

	emotionPoint.setX(emotionPoint.x() + 170);		//调整表情窗口的水平位置，使其靠右对齐当前窗口
	emotionPoint.setY(emotionPoint.y() + 220);		//调整表情窗口的垂直位置，使其位于当前窗口的下方
	m_emotionWindow->move(emotionPoint);		//将表情窗口移动到计算得到的位置，使其出现在当前窗口的右下方
}
void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	QWidget* talkWindow = m_talkwindowItemMap.find(item).value();		//根据被点击的聊天窗口列表项，从映射关系中获取对应的聊天窗口
	ui.rightStackedWidget->setCurrentWidget(talkWindow);		//根据被点击的聊天窗口列表项，从映射关系中获取对应的聊天窗口，并将右侧的堆叠窗口切换到该聊天窗口，显示该聊天窗口的内容
}