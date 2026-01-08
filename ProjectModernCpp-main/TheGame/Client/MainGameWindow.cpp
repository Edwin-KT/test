#include "MainGameWindow.h"
#include <qtimer.h>
#include <qdatetime.h>
#include <LoginWindow.h>
#include <AccountWindow.h>
#include <GameWindow.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

MainGameWindow::MainGameWindow(QWidget *parent)
	: QMainWindow(parent), 
	ui(std::make_unique<Ui::MainGameWindowClass>())
{
	ui->setupUi(this);

	auto* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this]() {
		const auto now = QDateTime::currentDateTime();
		const QString dateTimeText = now.toString("yyyy-MM-dd HH:mm:ss");
		ui->timeDate->setText(dateTimeText);
		});
	timer->start(1000);
}

void MainGameWindow::on_account_clicked()
{
	auto* accountWindow = new AccountWindow(nullptr);
	accountWindow->setAttribute(Qt::WA_DeleteOnClose);
	accountWindow->show();
	this->close();
}

void MainGameWindow::on_createGame_clicked()
{
	bool ok;
	QString pin = QInputDialog::getText(this, tr("Create Game"),tr("Enter a PIN for the game (optional):"), QLineEdit::Normal,"", &ok);
	if (ok) {
		std::string pinStd = pin.toStdString();
		QMessageBox::information(this, tr("Game Created"), tr("Game created successfully with PIN: %1").arg(pin.isEmpty() ? "No PIN" : pin));
		QMessageBox::information(this, tr("Joining Game"), tr("Joining the created game..."));
		auto* gameWindow = new GameWindow(pinStd, nullptr);
		gameWindow->setAttribute(Qt::WA_DeleteOnClose);
		gameWindow->showMaximized();
		this->close();
	}
}

void MainGameWindow::on_findGame_clicked()
{
	bool ok;
	QString pin = QInputDialog::getText(this, tr("Find Game"), tr("Enter the PIN for the game (if any):"), QLineEdit::Normal, "", &ok);
	if (ok) {
		std::string pinStd = pin.toStdString();
		QMessageBox::information(this, tr("Game Found"), tr("Joined game with PIN: %1").arg(pin.isEmpty() ? "No PIN" : pin));
		QMessageBox::information(this, tr("Joining Game"), tr("Joining the game..."));
		auto* gameWindow = new GameWindow(pinStd, nullptr);
		gameWindow->setAttribute(Qt::WA_DeleteOnClose);
		gameWindow->showMaximized();
		this->close();
	}
}

void MainGameWindow::on_randomGame_clicked()
{
	//to do the pin generation and matching logic

	QMessageBox::information(this, tr("Random Game"), tr("Searching for a random game..."));
	QMessageBox::information(this, tr("Game Found"), tr("Joined a random game successfully."));
	auto* gameWindow = new GameWindow(nullptr);
	gameWindow->setAttribute(Qt::WA_DeleteOnClose);
	gameWindow->showMaximized();
	this->close();
}

void MainGameWindow::on_logoutButton_clicked()
{
	auto* loginWindow = new LoginWindow(nullptr);
	loginWindow->setAttribute(Qt::WA_DeleteOnClose);
	QMessageBox::information(this, "Logged Out", "You have been logged out successfully.");
	loginWindow->show();
	this->close();
}


