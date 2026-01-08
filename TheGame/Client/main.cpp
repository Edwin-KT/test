#include "Client.h"
#include "LoginWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Client window;
	LoginWindow loginWindow;
	loginWindow.show();
    return app.exec();
}
