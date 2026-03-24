#include "QMsgTextEdit.h"
#include <QMovie>
#include <QUrl>
QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{}

QMsgTextEdit::~QMsgTextEdit()
{
	deletAllEmotionImage();		//在析构函数中调用删除表情图片的函数，确保在销毁消息输入框时，所有的表情图片都被正确地删除，避免内存泄漏等问题
}

void QMsgTextEdit::onEmotionImageChage(int frame)	//表情图片改变的槽函数
{
	QMovie* movie = qobject_cast<QMovie*>(sender());	//获取发送信号的对象，即表情图片的QMovie对象
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
	//将表情图片的当前帧的图像资源添加到消息输入框的文档中
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);	//根据表情编号构建表情图片的URL
	const QString& flagName = QString("%1").arg(imageName);
	insertHtml(QString("<img src='%1'/>").arg(flagName));//将表情图片的URL插入到消息输入框中，使用HTML的img标签来显示表情图片
	if (m_listEmotionUrl.contains(imageName))	//检查表情图片的URL是否已经存在于表情图片列表中
	{
		return;	//如果表情图片的URL已经存在于表情图片列表中，则直接返回，避免重复添加同一个表情图片
	}
	else
	{
		m_listEmotionUrl.append(imageName);	//如果表情图片的URL不存在于表情图片列表中，则将其添加到表情图片列表中，记录已经添加的表情图片信息
	}
	QMovie* apngMovie = new QMovie(imageName,"apng",this);	//创建一个QMovie对象，加载表情图片的URL，支持动态表情图片的显示
	m_emotionMap.insert(apngMovie, flagName);	//将QMovie对象和表情图片的URL添加到表情图片映射表中，方便后续的管理和删除表情图片等操作
	
	//数据帧改变发送信号
	connect(apngMovie, &QMovie::frameChanged, this, &QMsgTextEdit::onEmotionImageChage);	//连接QMovie对象的frameChanged信号和表情图片改变的槽函数，当表情图片的帧改变时，触发槽函数来更新表情图片的显示等操作
	apngMovie->start();	//启动QMovie对象，开始播放表情图片的动画效果
	updateGeometry();	//更新消息输入框的几何形状，确保表情图片的显示位置和大小正确，适应消息输入框的布局等要求
}

void QMsgTextEdit::deletAllEmotionImage()
{
	for (auto itor = m_emotionMap.begin(); itor != m_emotionMap.end(); ++itor)//map左闭右开
	{
		delete itor.key();	//删除表情图片对象，释放内存资源
	}
	m_emotionMap.clear();	//清空表情图片映射表，移除所有的表情图片信息
}

