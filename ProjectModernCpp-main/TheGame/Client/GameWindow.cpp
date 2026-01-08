#include "GameWindow.h"
#include "NetworkClient.h"

GameWindow::GameWindow(std::string gameId, QWidget* parent)
	: QMainWindow(parent), m_currentGameId(gameId)
{
	ui.setupUi(this);
	m_chatUpdateTimer = new QTimer(this);
	connect(m_chatUpdateTimer, &QTimer::timeout, this, &GameWindow::updateGameState);
	m_chatUpdateTimer->start(2000);

	m_handButtons = {ui.card1, ui.card2, ui.card3, ui.card4,ui.card5,  ui.card6,  ui.card7,  ui.card8};
	for (QPushButton* button : m_handButtons) 
	{
		connect(button, &QPushButton::clicked, this, &GameWindow::on_HandCard_Clicked);
	}

	ui.deckAscending1->setProperty("stackIndex", 0);
	ui.deckAscending2->setProperty("stackIndex", 1);
	ui.deckDescending1->setProperty("stackIndex", 2);
	ui.deckDescending2->setProperty("stackIndex", 3);

	connect(ui.deckAscending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckAscending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckDescending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
	connect(ui.deckDescending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);

}

GameWindow::~GameWindow()
{

}

void GameWindow::updateChatMessages()
{
	std::vector<std::string> messages = NetworkClient::getMessages(m_currentGameId);
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
	m_selectedCardValue = clickedButton->text().toInt();
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
	}
}

void GameWindow::updateGameState()
{
	auto gameState = NetworkClient::getGameState(m_currentGameId);

	if (gameState.stacks.size() >= 4) {
		ui.deckAscending1->setText(QString::number(gameState.stacks[0]));
		ui.deckAscending2->setText(QString::number(gameState.stacks[1]));
		ui.deckDescending1->setText(QString::number(gameState.stacks[2]));
		ui.deckDescending2->setText(QString::number(gameState.stacks[3]));

		setCardImage(ui.deckAscending1, gameState.stacks[0]);
		setCardImage(ui.deckAscending2, gameState.stacks[1]);
		setCardImage(ui.deckDescending1, gameState.stacks[2]);
		setCardImage(ui.deckDescending2, gameState.stacks[3]);
	}

	if (!gameState.playerHand.empty()) {
		updateHand(gameState.playerHand);
	}
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
			m_handButtons[i]->setEnabled(true);
			m_handButtons[i]->show();
		}
		else 
		{
			m_handButtons[i]->setText("");
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
	if(!cardIcon.isNull())
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
			ui.messages->addItem("Me: " + message);
			ui.messages->scrollToBottom();
			ui.messageCurrent->clear();
		}
	}
}
