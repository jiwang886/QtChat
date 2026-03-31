#include "SendFile.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QFileDialog>
#include <QMessageBox>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent), m_filePath("")
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);	//设置窗口关闭时自动删除对象，避免内存泄漏
	initTitleBar();
	setTitleBarTitle("发送文件",":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	this->move(100, 400);

	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);


}

SendFile::~SendFile()
{}

void SendFile::on_openBtn_clicked()		//打开文件按钮被点击的槽函数
{
	m_filePath = QFileDialog::getOpenFileName(this, "选择要发送的文件", "", "All Files (*.*)");		//打开文件对话框，获取用户选择的文件路径
	ui.lineEdit->setText(m_filePath);		//将选择的文件路径显示在文本框中
}
void SendFile::on_sendBtn_clicked()		//发送文件按钮被点击的槽函数
{
	if (!m_filePath.isEmpty())
	{
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			int msgType = 2;		//消息类型，2表示文件消息
			QString str = file.readAll();		//读取文件内容
			//文件名称
			QFileInfo fileInfo(m_filePath);	//获取文件所有信息
			QString fileName = fileInfo.fileName();	//获取文件名称
			emit sendFileClicked(str, msgType, fileName);		//发送文件消息，参数包括消息类型、文件内容和文件名称
			file.close();		//关闭文件
		}
		else
		{
			QMessageBox::information(this, "提示", QString("无法打开文件%1，请检查文件路径是否正确！").arg(m_filePath));		//如果无法打开文件，弹出一个消息框提示用户检查文件路径是否正确等
			
			return;
		}
		m_filePath.clear();
		ui.lineEdit->clear();
		this->close();		//关闭发送文件窗口
	}
}