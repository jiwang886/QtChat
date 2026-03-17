//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>

#include "UserLogin.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    /*CCMainWindow window;
    window.show();*/

	UserLogin* userLogin = new UserLogin();		//创建一个用户登录窗口对象
	userLogin->show();						//显示用户登录窗口

    return app.exec();
}
