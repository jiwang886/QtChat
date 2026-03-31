#pragma once

#include "BasicWindow.h"
#include "ui_ReceiveFile.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = nullptr);
	~ReceiveFile();

	void setMsg(QString& msgLabel);
	
signals:
	void refuseFile();	//拒绝文件的信号 

private slots:
	void on_okBtn_clicked();
	void on_cannelBtn_clicked();

private:
	Ui::ReceiveFile ui;
};

