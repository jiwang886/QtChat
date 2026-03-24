#pragma once

#include <QWidget>
#include "ui_ContactItem.h"

class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = nullptr);
	~ContactItem();
	void setUserName(const QString& userName);		//设置用户名
	void setSignName(const QString& signName);		//设置签名
	void setHeadPixmap(const QPixmap& headPath);		//设置用户头像
	QString getUserName() const;		//获取用户名
	QSize getHeadLabelSize() const;		//获取头像的大小

private:
	void initControl();		//初始化控件，设置控件的属性和布局，例如设置标签的文本、设置头像的大小等

private:
	Ui::ContactItem ui;
};

