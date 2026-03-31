#pragma once

#include <QTextEdit>

class QMsgTextEdit  : public QTextEdit
{
	Q_OBJECT

		
public:
	QMsgTextEdit(QWidget *parent = NULL);
	~QMsgTextEdit();

private slots:
	void onEmotionImageChage(int frame);	//表情图片改变的槽函数

public:
	void addEmotionUrl(int emotionNum);		//添加表情图片的URL到消息输入框中
	void deletAllEmotionImage();		//删除消息输入框中的所有表情图片

private:
	QList<QString>m_listEmotionUrl;			//创建表情链表
	QMap<QMovie*, QString> m_emotionMap;	//表情图片映射表，使用一个QMap来存储表情图片的信息

};

