#include "AccountWindow.h"
#include <MainGameWindow.h>

AccountWindow::AccountWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

AccountWindow::~AccountWindow()
{}

void AccountWindow::on_pushButton_clicked()
{
	auto* mainGameWindow = new MainGameWindow(nullptr);
	mainGameWindow->setAttribute(Qt::WA_DeleteOnClose);
	mainGameWindow->show();
	this->close();
}

