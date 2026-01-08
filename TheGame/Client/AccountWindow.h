#pragma once

#include <QMainWindow>
#include "ui_AccountWindow.h"
#include <memory>

class AccountWindow : public QMainWindow
{
	Q_OBJECT

public:
	AccountWindow(QWidget *parent = nullptr);
	~AccountWindow();

private slots:
	void on_pushButton_clicked();

private:
	Ui::AccountWindowClass ui;
};

