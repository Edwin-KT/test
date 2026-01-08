#include "GameWindow.h"
#include "NetworkClient.h"
#include <QMessageBox>

GameWindow::GameWindow(std::string gameId, QWidget* parent)
	: QMainWindow(parent), m_currentGameId(gameId)
{
	ui.setupUi(this);
	m_chatUpdateTimer = new QTimer(this);
	connect(m_chatUpdateTimer, &QTimer::timeout, this, &GameWindow::updateGameState);

	// --- End Turn Button ---
	m_endTurnButton = new QPushButton("End Turn", this);
	m_endTurnButton->setGeometry(800, 500, 100, 40);
	m_endTurnButton->hide(); // Hidden until game starts
	connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::on_EndTurn_Clicked);

	// --- NEW: Start Game Button ---
	m_startGameButton = new QPushButton("Start Game", this);
	m_startGameButton->setGeometry(800, 450, 100, 40); // Placed above End Turn
	m_startGameButton->show(); // Visible initially
	connect(m_startGameButton, &QPushButton::clicked, this, &GameWindow::on_StartGame_Clicked);

	// --- NEW: Game Info Label ---
	m_gameInfoLabel = new QLabel(this);
	m_gameInfoLabel->setGeometry(20, 50, 200, 30); // Adjust position as needed
	m_gameInfoLabel->setStyleSheet("color: white; font-weight: bold; background: rgba(0,0,0,0.5);");
	m_gameInfoLabel->show();

	m_handButtons = { ui.card1, ui.card2, ui.card3, ui.card4,ui.card5,  ui.card6,  ui.card7,  ui.card8 };
	for (QPushButton* button : m_handButtons)
	{
		connect(button, &QPushButton::clicked, this, &GameWindow::on_HandCard_Clicked);
	}

	// [Keep setup for stack buttons...]
	ui.deckAscending1->setProperty("stackIndex", 0);
	ui.deckAscending2->setProperty("stackIndex", 1);
	ui.deckDescending1->setProperty("stackIndex", 2);
	ui.deckDescending2->setProperty("stackIndex", 3);

	connect(ui.deckAscending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckAscending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckDescending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckDescending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);

	m_chatUpdateTimer->start(1000);

	updateGameState();
	updateChatMessages();
}

GameWindow::~GameWindow()
{
}

// NEW: Handle Start Click
void GameWindow::on_StartGame_Clicked()
{
	bool success = NetworkClient::startGame(m_currentGameId);
	if (success) {
		m_startGameButton->hide();
		m_endTurnButton->show();
		QMessageBox::information(this, "Info", "Game Started!");
		updateGameState();
	}
	else {
		QMessageBox::warning(this, "Error", "Failed to start game (Maybe already started?)");
	}
}

void GameWindow::on_EndTurn_Clicked()
{
	if (NetworkClient::endTurn(m_currentGameId)) {
		updateGameState();
		m_selectedCardValue = -1;
	}
	else {
		QMessageBox::warning(this, "Action Failed", "Cannot end turn yet.");
	}
}

void GameWindow::updateChatMessages()
{
	// Safety check for UI element
	if (!ui.messages) return;

	std::vector<std::string> messages = NetworkClient::getMessages(m_currentGameId);

	// Only update if we have new messages
	if (messages.size() > (size_t)ui.messages->count())
	{
		for (size_t i = ui.messages->count(); i < messages.size(); ++i)
		{
			ui.messages->addItem(QString::fromStdString(messages[i]));
		}
		ui.messages->scrollToBottom();
	}
}

void GameWindow::on_HandCard_Clicked()
{
	QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
	if (clickedButton->text().isEmpty() && clickedButton->icon().isNull()) return;

	QVariant val = clickedButton->property("cardValue");
	if (val.isValid()) m_selectedCardValue = val.toInt();

	// Simple visual feedback: clear others, highlight this
	for (auto btn : m_handButtons) btn->setStyleSheet("");
	clickedButton->setStyleSheet("border: 2px solid pink;");
}

void GameWindow::on_Stack_Clicked()
{
	if (m_selectedCardValue == -1) return;
	QPushButton* stackButton = qobject_cast<QPushButton*>(sender());
	int stackIndex = stackButton->property("stackIndex").toInt();
	bool isSucces = NetworkClient::playCard(m_currentGameId, m_selectedCardValue, stackIndex);
	if (isSucces)
	{
		m_selectedCardValue = -1;
		updateGameState();
		for (auto btn : m_handButtons) btn->setStyleSheet("");
	}
}

void GameWindow::updateGameState()
{
	// 1. Fetch data
	auto gameState = NetworkClient::getGameState(m_currentGameId);

	// 2. Handle Game Over
	if (gameState.status == "Won") {
		if (m_chatUpdateTimer) m_chatUpdateTimer->stop();
		QMessageBox::information(this, "Game Over", "You Won!");
		return;
	}
	if (gameState.status == "Lost") {
		if (m_chatUpdateTimer) m_chatUpdateTimer->stop();
		QMessageBox::information(this, "Game Over", "You Lost!");
		return;
	}

	// 3. Check if game is running
	bool isRunning = (gameState.stacks.size() >= 4);

	if (isRunning) {
		// --- SAFEGUARDED POINTER ACCESS ---
		if (m_startGameButton) m_startGameButton->hide();
		if (m_endTurnButton) {
			m_endTurnButton->show();
			m_endTurnButton->setEnabled(gameState.isMyTurn);
		}

		if (gameState.isMyTurn) this->setWindowTitle("The Game - YOUR TURN");
		else this->setWindowTitle("The Game - Waiting for others...");

		if (m_gameInfoLabel) {
			m_gameInfoLabel->setText("Deck: " + QString::number(gameState.deckCount) + " cards");
		}

		// Update Stacks (ui pointers are managed by Qt, usually safe if setupUi ran)
		if (ui.deckAscending1) {
			ui.deckAscending1->setText(QString::number(gameState.stacks[0]));
			setCardImage(ui.deckAscending1, gameState.stacks[0]);
		}
		if (ui.deckAscending2) {
			ui.deckAscending2->setText(QString::number(gameState.stacks[1]));
			setCardImage(ui.deckAscending2, gameState.stacks[1]);
		}
		if (ui.deckDescending1) {
			ui.deckDescending1->setText(QString::number(gameState.stacks[2]));
			setCardImage(ui.deckDescending1, gameState.stacks[2]);
		}
		if (ui.deckDescending2) {
			ui.deckDescending2->setText(QString::number(gameState.stacks[3]));
			setCardImage(ui.deckDescending2, gameState.stacks[3]);
		}

		updateHand(gameState.playerHand);

	}
	else {
		// Lobby State
		if (m_startGameButton) m_startGameButton->show();
		if (m_endTurnButton) m_endTurnButton->hide();
		this->setWindowTitle("The Game - Lobby");

		if (m_gameInfoLabel) m_gameInfoLabel->setText("Waiting to start...");
	}

	updateChatMessages();
}

void GameWindow::updateHand(const std::vector<int>& cardsReceived)
{
	for (int i = 0; i < m_handButtons.size(); ++i)
	{
		if (i < cardsReceived.size())
		{
			int val = cardsReceived[i];
			QString imagePath = QString(":/cards/GameImages/GameCards/ResizedCards/GameCard%1.jpg").arg(val);
			m_handButtons[i]->setIcon(QIcon(imagePath));
			m_handButtons[i]->setIconSize(QSize(80, 120));
			m_handButtons[i]->setText("");
			m_handButtons[i]->setProperty("cardValue", val); // Store value for click event
			m_handButtons[i]->setEnabled(true);
			m_handButtons[i]->show();
		}
		else
		{
			m_handButtons[i]->setText("");
			m_handButtons[i]->setProperty("cardValue", -1);
			m_handButtons[i]->setEnabled(false);
			m_handButtons[i]->hide();
		}
	}
}

void GameWindow::setCardImage(QPushButton* button, int cardValue)
{
	if (cardValue < 0)
		return;
	QString imagePath = QString(":/cards/GameImages/GameCards/ResizedCards/GameCard%1.jpg").arg(cardValue);
	QIcon cardIcon(imagePath);
	if (!cardIcon.isNull())
	{
		button->setIcon(cardIcon);
		button->setIconSize(QSize(80, 120));
		button->setText("");
		button->setStyleSheet("border: none; background : transparent;");
	}
	else
	{
		button->setText(QString::number(cardValue));

	}
}

void GameWindow::on_sendButton_clicked()
{
	QString message = ui.messageCurrent->text();
	if (!message.isEmpty())
	{
		if (NetworkClient::sendMessage(m_currentGameId, message.toStdString()))
		{
			// Removed local add, let the timer fetch it to avoid duplicates or implement local echo with care
			ui.messageCurrent->clear();
			updateChatMessages();
		}
	}
}