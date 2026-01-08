#include "MainGameWindow.h"
#include <qtimer.h>
#include <qdatetime.h>
#include <LoginWindow.h>
#include <AccountWindow.h>
#include <GameWindow.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include "NetworkClient.h" // Include this!

MainGameWindow::MainGameWindow(QWidget* parent)
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
	QString pin = QInputDialog::getText(this, tr("Create Game"), tr("Enter a PIN for the game (optional):"), QLineEdit::Normal, "", &ok);
	if (ok) {
		std::string pinStd = pin.toStdString();

		// --- FIX START ---
		int gameId = NetworkClient::createGame(pinStd);
		if (gameId != -1) {
			QMessageBox::information(this, tr("Game Created"), tr("Game created! ID: %1").arg(gameId));
			// Pass the numeric ID as string to GameWindow
			auto* gameWindow = new GameWindow(std::to_string(gameId), nullptr);
			gameWindow->setAttribute(Qt::WA_DeleteOnClose);
			gameWindow->showMaximized();
			this->close();
		}
		else {
			QMessageBox::warning(this, tr("Error"), tr("Failed to create game."));
		}
		// --- FIX END ---
	}
}

void MainGameWindow::on_findGame_clicked()
{
	bool ok;
	QString pin = QInputDialog::getText(this, tr("Find Game"), tr("Enter the PIN for the game:"), QLineEdit::Normal, "", &ok);
	if (ok && !pin.isEmpty()) {
		std::string pinStd = pin.toStdString();

		// --- FIX START ---
		int gameId = NetworkClient::joinGame(pinStd);
		if (gameId != -1) {
			QMessageBox::information(this, tr("Success"), tr("Joined game ID: %1").arg(gameId));
			auto* gameWindow = new GameWindow(std::to_string(gameId), nullptr);
			gameWindow->setAttribute(Qt::WA_DeleteOnClose);
			gameWindow->showMaximized();
			this->close();
		}
		else {
			QMessageBox::warning(this, tr("Error"), tr("Game not found or full."));
		}
		// --- FIX END ---
	}
}

void MainGameWindow::on_randomGame_clicked()
{
	QMessageBox::information(this, tr("Random Game"), tr("Searching for a random game..."));

	// --- FIX START ---
	int gameId = NetworkClient::joinRandomGame();
	if (gameId != -1) {
		QMessageBox::information(this, tr("Game Found"), tr("Joined game ID: %1").arg(gameId));
		auto* gameWindow = new GameWindow(std::to_string(gameId), nullptr);
		gameWindow->setAttribute(Qt::WA_DeleteOnClose);
		gameWindow->showMaximized();
		this->close();
	}
	else {
		QMessageBox::warning(this, tr("Error"), tr("No public games available. Create one!"));
	}
	// --- FIX END ---
}

void MainGameWindow::on_logoutButton_clicked()
{
	auto* loginWindow = new LoginWindow(nullptr);
	loginWindow->setAttribute(Qt::WA_DeleteOnClose);
	QMessageBox::information(this, "Logged Out", "You have been logged out successfully.");
	loginWindow->show();
	this->close();
}