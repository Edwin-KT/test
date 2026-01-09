#pragma once

#include <QMainWindow>
#include "ui_GameWindow.h"
#include <qtimer.h>
#include <QPushButton>
#include <QLabel>

class GameWindow : public QMainWindow
{
	Q_OBJECT

public:
	GameWindow(std::string gameId, QWidget* parent = nullptr);
	~GameWindow();

private:
	Ui::GameWindowClass ui;
	QTimer* m_chatUpdateTimer;
	std::string m_currentGameId;
	QList<QPushButton*> m_handButtons;

	QPushButton* m_endTurnButton;
	QPushButton* m_startGameButton;
	QLabel* m_gameInfoLabel;

	int m_selectedCardValue = -1;

private slots:
	void on_sendButton_clicked();
	void updateChatMessages();
	void on_HandCard_Clicked();
	void on_Stack_Clicked();
	void on_EndTurn_Clicked();
	void on_StartGame_Clicked();
	void updateGameState();
	void updateHand(const std::vector<int>& cardsReceived);
	void setCardImage(QPushButton* button, int cardValue);
};