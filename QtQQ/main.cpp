//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>

#include "UserLogin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    /*CCMainWindow window;
    window.show();*/
	app.setQuitOnLastWindowClosed(false);		//设置当最后一个窗口关闭时不退出应用程序，保持应用程序在后台运行

	UserLogin* userLogin = new UserLogin();		//创建一个用户登录窗口对象
	userLogin->show();						//显示用户登录窗口

    return app.exec();
}
