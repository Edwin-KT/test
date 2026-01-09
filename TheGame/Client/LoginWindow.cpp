#include "LoginWindow.h"
#include <QMessageBox>
#include "MainGameWindow.h"
#include "NetworkClient.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

LoginWindow::LoginWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.lineEdit->setFocus();
}

LoginWindow::~LoginWindow()
{}

void LoginWindow::on_loginButton_clicked()
{
	QString username = ui.lineEdit->text().trimmed();
	QString password = ui.lineEdit_2->text().trimmed();

	std::cout << "Login attempt - User: '" << username.toStdString()
		<< "', Pass length: " << password.size() << std::endl;

	if (NetworkClient::loginUser(username.toStdString(), password.toStdString())) {
		QMessageBox::information(this, "Success", "Login successful!");
		auto* mainGameWindow = new MainGameWindow(nullptr);
		mainGameWindow->setAttribute(Qt::WA_DeleteOnClose);
		mainGameWindow->show();
		this->close();
	}
	else {
		QMessageBox::critical(this, "Error", "Invalid username or password!");
	}
}

void LoginWindow::on_registerButton_clicked()
{
	QString username = ui.lineEdit->text().trimmed();  
	QString password = ui.lineEdit_2->text().trimmed(); 

	if (username.isEmpty() || password.isEmpty()) {
		QMessageBox::warning(this, "Error", "Username and password are required!");
		return;
	}

	std::cout << "Client sending username: '" << username.toStdString() << "'" << std::endl;

	if (NetworkClient::registerUser(username.toStdString(), password.toStdString())) {
		QMessageBox::information(this, "Success", "Registration successful!");
	}
	else {
		QMessageBox::critical(this, "Error", "Registration failed. Username may already exist.");
	}
}
void LoginWindow::on_lineEdit_returnPressed()
{
	on_loginButton_clicked();
}

void LoginWindow::on_lineEdit_2_returnPressed()
{
	on_loginButton_clicked();
}

void LoginWindow::on_exitButton_clicked()
{
	close();
}

bool LoginWindow::checkCredentials(const QString& username, const QString& password)
{
	if (username.contains(" "))
		return false;
	bool hasUpper = false;
	bool hasLower = false;
	bool hasDigit = false;
	for (const QChar c : password)
	{
		if(c.isUpper())
			hasUpper = true;
		else if (c.isLower())
			hasLower = true;
		else if (c.isDigit())
			hasDigit = true;
		else if (c.isSpace())
			return false;
	}

	return hasDigit && hasLower && hasUpper;
}

