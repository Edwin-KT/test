#pragma once

#include <QMainWindow>
#include <memory>
#include "ui_MainGameWindow.h"

class MainGameWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainGameWindow(QWidget *parent = nullptr);
	~MainGameWindow() override = default;

private:
	std::unique_ptr<Ui::MainGameWindowClass> ui;
private slots:
	void on_logoutButton_clicked();
	void on_account_clicked();
	void on_createGame_clicked();
	void on_findGame_clicked();
	void on_randomGame_clicked();
};

