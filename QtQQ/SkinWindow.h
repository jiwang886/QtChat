#pragma once

#include "BasicWindow.h"
#include "ui_SkinWindow.h"

class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = nullptr);
	~SkinWindow();

public:
	void initControl();		//初始化控件

	void onShowClose();		//关闭窗口的槽函数

private:
	Ui::SkinWindow ui;
};

