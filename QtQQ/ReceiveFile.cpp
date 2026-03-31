#include "ReceiveFile.h"
#include <QFileDialog>
#include <QMessageBox>

extern QString gfileName;
extern QString gfileData;

ReceiveFile::ReceiveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏
	initTitleBar();
	setTitleBarTitle("接收文件", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(100, 400);
	connect(m_TitleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);
}

ReceiveFile::~ReceiveFile()
{}

void ReceiveFile::setMsg(QString & msgLabel)
{
	ui.label->setText(msgLabel);	//设置接收文件窗口的消息标签的文本为传入的消息内容，例如可以将发送文件的用户昵称、文件名称等信息作为消息内容，提示用户接收文件的相关信息等
}

void ReceiveFile::on_cannelBtn_clicked()
{
	emit refuseFile();	//发送拒绝文件的信号，通知聊天窗口等相关组件进行相应的处理，例如关闭接收文件窗口、发送拒绝文件的消息等
	this->close();	//关闭接收文件窗口，结束接收文件的操作等
}

void ReceiveFile::on_okBtn_clicked()
{
	//获取保存位置
	QString fileDirPath = QFileDialog::getExistingDirectory(NULL, "选择保存位置", QDir::homePath());	//打开文件夹选择对话框，让用户选择文件的保存位置，例如可以将默认的保存位置设置为用户的主目录等
	QString filePath = fileDirPath + "/" + gfileName;	//根据用户选择的保存位置和文件名称来构建完整的文件路径，例如可以将文件名称保存在一个全局变量中，并在接收文件时使用该变量来获取文件名称等

	QFile file(filePath);	//创建一个文件对象，表示要保存的文件，例如可以使用QFile类来创建一个文件对象，并设置文件路径等
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, "错误", "无法打开文件进行写入！");	//如果无法打开文件进行写入，则显示一个错误消息框，提示用户无法保存文件等
	}
	else
	{
		file.write(gfileData.toUtf8());	//将接收到的文件数据写入到文件中，例如可以将接收到的文件数据保存在一个全局变量中，并在接收文件完成后使用该变量来写入文件等
		file.close();	//关闭文件，完成文件的保存操作等
		QMessageBox::information(this, "提示", "文件保存成功！");	//如果文件保存成功，则显示一个提示消息框，告知用户文件保存成功等
	}

	this->close();	//关闭接收文件窗口，结束接收文件的操作等
}

