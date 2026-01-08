#include "GameWindow.h"
#include "NetworkClient.h"
#include <QMessageBox>
#include <vector>
#include <algorithm> // Required for std::min

GameWindow::GameWindow(std::string gameId, QWidget* parent)
    : QMainWindow(parent), m_currentGameId(gameId)
    // Initialize pointers to null to be safe
    , m_endTurnButton(nullptr), m_startGameButton(nullptr), m_gameInfoLabel(nullptr)
{
    ui.setupUi(this);

    // 1. Create the manual buttons
    m_endTurnButton = new QPushButton("End Turn", this);
    m_endTurnButton->setGeometry(800, 500, 100, 40);
    m_endTurnButton->hide();
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::on_EndTurn_Clicked);

    m_startGameButton = new QPushButton("Start Game", this);
    m_startGameButton->setGeometry(800, 450, 100, 40);
    m_startGameButton->show();
    connect(m_startGameButton, &QPushButton::clicked, this, &GameWindow::on_StartGame_Clicked);

    m_gameInfoLabel = new QLabel(this);
    m_gameInfoLabel->setGeometry(20, 50, 200, 30);
    m_gameInfoLabel->setStyleSheet("color: white; font-weight: bold; background: rgba(0,0,0,0.5);");
    m_gameInfoLabel->show();

    // 2. Safely populate hand buttons from UI
    std::vector<QPushButton*> potentialButtons = {
        ui.card1, ui.card2, ui.card3, ui.card4,
        ui.card5, ui.card6, ui.card7, ui.card8
    };

    m_handButtons.clear();
    for (auto* btn : potentialButtons) {
        if (btn) {
            m_handButtons.push_back(btn);
            connect(btn, &QPushButton::clicked, this, &GameWindow::on_HandCard_Clicked);
        }
    }

    // 3. Setup Stack Buttons (Safe Check)
    if (ui.deckAscending1) {
        ui.deckAscending1->setProperty("stackIndex", 0);
        connect(ui.deckAscending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
    }
    if (ui.deckAscending2) {
        ui.deckAscending2->setProperty("stackIndex", 1);
        connect(ui.deckAscending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
    }
    if (ui.deckDescending1) {
        ui.deckDescending1->setProperty("stackIndex", 2);
        connect(ui.deckDescending1, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
    }
    if (ui.deckDescending2) {
        ui.deckDescending2->setProperty("stackIndex", 3);
        connect(ui.deckDescending2, &QPushButton::clicked, this, &GameWindow::on_Stack_Clicked);
    }

    // 4. Start Timer
    m_chatUpdateTimer = new QTimer(this);
    connect(m_chatUpdateTimer, &QTimer::timeout, this, &GameWindow::updateGameState);
    m_chatUpdateTimer->start(1000);

    // Initial update
    updateGameState();
    updateChatMessages();
}

GameWindow::~GameWindow()
{
}

void GameWindow::on_StartGame_Clicked()
{
    bool success = NetworkClient::startGame(m_currentGameId);
    if (success) {
        if (m_startGameButton) m_startGameButton->hide();
        if (m_endTurnButton) m_endTurnButton->show();
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
    if (!ui.messages) return;

    std::vector<std::string> messages = NetworkClient::getMessages(m_currentGameId);
    if (messages.empty()) return;

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
    if (!clickedButton) return;
    if (clickedButton->text().isEmpty() && clickedButton->icon().isNull()) return;

    QVariant val = clickedButton->property("cardValue");
    if (val.isValid()) m_selectedCardValue = val.toInt();

    for (auto btn : m_handButtons) {
        if (btn) btn->setStyleSheet("");
    }
    clickedButton->setStyleSheet("border: 2px solid pink;");
}

void GameWindow::on_Stack_Clicked()
{
    if (m_selectedCardValue == -1) return;
    QPushButton* stackButton = qobject_cast<QPushButton*>(sender());
    if (!stackButton) return;

    int stackIndex = stackButton->property("stackIndex").toInt();
    bool isSucces = NetworkClient::playCard(m_currentGameId, m_selectedCardValue, stackIndex);
    if (isSucces)
    {
        m_selectedCardValue = -1;
        updateGameState();
        for (auto btn : m_handButtons) {
            if (btn) btn->setStyleSheet("");
        }
    }
}

void GameWindow::updateGameState()
{
    auto gameState = NetworkClient::getGameState(m_currentGameId);

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

    bool isRunning = (gameState.stacks.size() >= 4);

    if (isRunning) {
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
        if (m_startGameButton) m_startGameButton->show();
        if (m_endTurnButton) m_endTurnButton->hide();
        this->setWindowTitle("The Game - Lobby");
        if (m_gameInfoLabel) m_gameInfoLabel->setText("Waiting to start...");
    }

    updateChatMessages();
}

void GameWindow::updateHand(const std::vector<int>& cardsReceived)
{
    // CRASH FIX: Cast to size_t to solve signed/unsigned mismatch in std::min
    size_t safeLimit = (std::min)(static_cast<size_t>(cardsReceived.size()), static_cast<size_t>(m_handButtons.size()));

    // 1. Update cards we have
    for (size_t i = 0; i < safeLimit; ++i)
    {
        QPushButton* btn = m_handButtons[i];
        if (!btn) continue;

        int val = cardsReceived[i];
        QString imagePath = QString(":/cards/GameImages/GameCards/ResizedCards/GameCard%1.jpg").arg(val);
        btn->setIcon(QIcon(imagePath));
        btn->setIconSize(QSize(80, 120));
        btn->setText("");
        btn->setProperty("cardValue", val);
        btn->setEnabled(true);
        btn->show();
    }

    // 2. Hide remaining slots
    for (size_t i = safeLimit; i < m_handButtons.size(); ++i)
    {
        QPushButton* btn = m_handButtons[i];
        if (!btn) continue;

        btn->setText("");
        btn->setProperty("cardValue", -1);
        btn->setEnabled(false);
        btn->hide();
    }
}

void GameWindow::setCardImage(QPushButton* button, int cardValue)
{
    if (!button || cardValue < 0) return;

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
    if (!ui.messageCurrent) return;
    QString message = ui.messageCurrent->text();
    if (!message.isEmpty())
    {
        if (NetworkClient::sendMessage(m_currentGameId, message.toStdString()))
        {
            ui.messageCurrent->clear();
            updateChatMessages();
        }
    }
}