#pragma once

#include "BasicWindow.h"
#include "ui_SendFile.h"

class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = nullptr);
	~SendFile();

signals:		//发送文件信号，参数分别是：数据内容，数据类型，文件名
	void sendFileClicked(QString& strData, int& msgType, QString fileName);

private slots:
	void on_openBtn_clicked();		//打开文件按钮被点击的槽函数
	void on_sendBtn_clicked();		//发送文件按钮被点击的槽函数

private:
	Ui::SendFile ui;
	QString m_filePath;				//文件路径，使用一个QString来存储选择的文件路径
};

