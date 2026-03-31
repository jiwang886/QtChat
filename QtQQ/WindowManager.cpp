#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>

Q_GLOBAL_STATIC(WindowManager,theInstance)		//定义一个全局静态变量，用于存储WindowManager的单例实例，确保在整个程序中只有一个WindowManager对象存在

WindowManager* WindowManager::getInstance()		//获取WindowManager单例实例的静态函数，返回一个指向WindowManager对象的指针
{
	return theInstance();		//返回全局静态变量theInstance的值，即WindowManager的单例实例
}

WindowManager::WindowManager() :QObject(NULL), m_talkWindowShell(NULL)
{
}

WindowManager::~WindowManager()
{}

QWidget* WindowManager::findWindowName(const QString & qsWindowName)
{
	if (m_windowMap.contains(qsWindowName))	//检查窗口名称是否存在于窗口映射关系中
	{
		return m_windowMap[qsWindowName];		//如果窗口名称存在于窗口映射关系中，返回对应的窗口对象
	}
	return nullptr;
}

void WindowManager::deleteWindow(const QString& qsWindowName)
{
	m_windowMap.remove(qsWindowName);		//从窗口映射关系中删除指定窗口名称的映射关系
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);		//如果窗口名称不存在于窗口映射关系中，添加新的窗口名称和对应的窗口对象的映射关系
	}
}

QString WindowManager::getCreatingTalkId()
{
	return m_strCreatingTalkId;
}

void WindowManager::addNewTalkWindow(const QString& uid/*, GroupType groupType, const QString& strPeople*/)
{
	if (m_talkWindowShell == nullptr)
	{
		m_talkWindowShell = new TalkWindowShell();		//如果聊天窗口容器指针为空，创建一个新的TalkWindowShell对象
		connect(m_talkWindowShell, &TalkWindowShell::destroyed, [this]() { m_talkWindowShell = nullptr; });		//连接聊天窗口容器的销毁信号和一个lambda函数，当聊天窗口容器被销毁时将聊天窗口容器指针置空，避免悬空指针
	}
	QWidget* widget = findWindowName(uid);		//通过窗口名称获取对应的窗口对象
	if (!widget)
	{
		m_strCreatingTalkId = uid;		
		TalkWindow* talkwindow = new TalkWindow(m_talkWindowShell,uid/*,groupType*/);		//如果窗口对象不存在，创建一个新的TalkWindow对象，并传入相关参数，例如用户ID、群组类型、人员信息等
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);		//创建一个新的TalkWindowItem对象
		
		m_strCreatingTalkId = "";		//将正在创建的聊天窗口的ID置空，表示当前没有正在创建的聊天窗口

		//判断是群聊还是私聊
		QSqlQueryModel sqlDepMpodel;
		QString strSql = QString("SELECT department_name,sign FROM tab_department WHERE departmentID = %1").arg(uid);
		sqlDepMpodel.setQuery(strSql);
		int rows = sqlDepMpodel.rowCount();
		QString strWindowName, strMsgLabel;
		if (rows == 0)//如果查询结果为空，说明是私聊
		{
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1").arg(uid);
			sqlDepMpodel.setQuery(sql);	//查询员工表，获取员工姓名和签名
		}
		QModelIndex indexDepIndex, signIndex;
		indexDepIndex = sqlDepMpodel.index(0, 1);		//获取查询结果的第一行第一列的索引，即部门名称或员工姓名
		signIndex = sqlDepMpodel.index(0, 0);		//获取查询结果的第一行第二列的索引，即部门签名或员工签名
		strWindowName = sqlDepMpodel.data(indexDepIndex).toString();		//获取部门名称或员工姓名
		strMsgLabel = sqlDepMpodel.data(signIndex).toString();		//获取部门签名或员工签名
		
		talkwindow->setWindowName(strWindowName);		//设置聊天窗口的标题为部门名称或员工姓名
		talkwindowItem->setMsgLabelContent(strMsgLabel);		//设置聊天窗口列表项的内容为部门签名或员工签名

		m_talkWindowShell->addTalkWindow(talkwindow, talkwindowItem,uid);

	//	switch (groupType) {
	//	case COMPANY:
	//	{
	//		talkwindow->setWindowName("神人科技--创建神奇内容");
	//		talkwindowItem->setMsgLabelContent("公司群");
	//		break;
	//	}
	//	case PERSONELGROUP:
	//	{
	//		talkwindow->setWindowName("用最低的待遇找人才");
	//		talkwindowItem->setMsgLabelContent("人事");
	//		break;
	//	}
	//	case DEVELOPMENTGROUP:
	//	{
	//		talkwindow->setWindowName("研发神奇内容");
	//		talkwindowItem->setMsgLabelContent("研发");
	//		break;
	//	}
	//	case MARKETGROUP:
	//	{
	//		talkwindow->setWindowName("寻找神奇内容");
	//		talkwindowItem->setMsgLabelContent("市场");
	//		break;
	//	}
	//	case PTOP:
	//	{
	//		talkwindow->setWindowName("天天骂老板");
	//		talkwindowItem->setMsgLabelContent(strPeople);
	//		break;
	//	}
	//	default:
	//		break;
	//	}
	//	m_talkWindowShell->addTalkWindow(talkwindow, talkwindowItem, groupType);
	}
	else
	{	//左侧聊天窗口列表项被点击时，获取对应的聊天窗口对象，并将其设置为当前显示的聊天窗口
		QListWidgetItem* item = m_talkWindowShell->getTalkWindowItemMap().key(widget);		//如果窗口对象存在，将对应的聊天窗口列表项设置为选中状态，表示当前显示的聊天窗口
		item->setSelected(1);
		
		//将右侧的堆叠窗口切换到指定的聊天窗口，显示该聊天窗口的内容
		m_talkWindowShell->setCurrentTalkWindow(widget);		//如果窗口对象存在，将其设置为当前显示的聊天窗口
	}
	m_talkWindowShell->show();		//显示聊天窗口容器
	m_talkWindowShell->activateWindow();		//激活聊天窗口容器，使其成为当前活动窗口
}
