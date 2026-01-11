#pragma once

#include <QMainWindow>
#include "ui_LoginWindow.h"
#include <memory>

class MainGameWindow;

class LoginWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = nullptr);
	~LoginWindow();

private slots:
	void on_loginButton_clicked();
	void on_registerButton_clicked();
	void on_lineEdit_returnPressed();
	void on_lineEdit_2_returnPressed();
	void on_exitButton_clicked();

private:
	bool checkCredentials(const QString& username, const QString& password);
	bool actionProgress = false;
	Ui::LoginWindowClass ui;
};

