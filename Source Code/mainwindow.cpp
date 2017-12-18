#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief MainWindow::MainWindow, QWidget constructor
 * @param parent, window application
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    inProgress = false;
    part_1 = true;
    firstStart = true;
    savedCoordinates.resize(2);

    // Connect depth slider and spin
    connect(ui->depthSlider, SIGNAL(valueChanged(int)),
            ui->depthEdit, SLOT(setValue(int)));
    connect(ui->depthEdit, SIGNAL(valueChanged(int)),
            ui->depthSlider, SLOT(setValue(int)));

    Q_INIT_RESOURCE(resources);

    // Store red and green icons
    QPixmap greenCircle(":/images/images/green_circle.png");
    QPixmap redCircle(":/images/images/red_circle.png");
    redIcon = QIcon(redCircle);
    redIcon.addPixmap(redCircle, QIcon::Disabled);
    greenIcon = QIcon(greenCircle);
    greenIcon.addPixmap(greenCircle, QIcon::Disabled);

    // Connect buttons with actions
    connect(ui->startButton, SIGNAL(clicked()), SLOT(startGame()));
    connect(ui->restartButton, SIGNAL(clicked()), SLOT(restartGame()));
    connect(ui->messageClearButton, SIGNAL(clicked()), SLOT(clearMessages()));
    connect(ui->expandButton, SIGNAL(clicked()), SLOT(expand()));
    connect(ui->reduceButton, SIGNAL(clicked()), SLOT(collapse()));
    connect(ui->endButton, SIGNAL(clicked()), SLOT(close()));
}

/**
 * @brief MainWindow::~MainWindow, destructor
 */

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::gameButtonClicked, meat of the game, decides who plays,
 *        performs attacks, calls AIobjects, etc.
 */

void MainWindow::gameButtonClicked() {
    // Only allow for button clicks if game is in progress
    if (inProgress) {
        setButtonsColor();

        // Fetch button position
        string buttonName = sender()->objectName().toUtf8().constData();
        string buttonIndex = buttonName.substr(buttonName.find("_") + 1, buttonName.length());
        int x = std::stoi(buttonIndex.substr(1, 2));
        int y = std::stoi(buttonIndex.substr(0, 1));

        // If first click
        if (part_1) {
            disableAllButtons();
            setClickedButtonColor(x, y);
            setAdjacentColors(x, y);
            savedCoordinates = {x, y};
            part_1 = !part_1;
        }
        // If second click
        else {
            // If second click is same button
            if (savedCoordinates[0] == x && savedCoordinates[1] == y) {
                part_1 = !part_1;
                updateBoard();
                return;
            }
            // If second click is empty tile
            else {
                vector<vector<char>> originalState;
                vector<vector<char>> newState;
                vector<vector<int>> removedTokens;

                originalState = game->getBoard()->getMatrix();
                game->attack(savedCoordinates[0], savedCoordinates[1], x, y);
                newState = game->getBoard()->getMatrix();

                if (game->getTurn())
                    removedTokens = game->getBoard()->getRemovedTokens(originalState, newState, 'G');
                else
                    removedTokens = game->getBoard()->getRemovedTokens(originalState, newState, 'R');

                if (!ui->aiBox->isChecked())
                    setRemovedTokensColors(removedTokens);

                displayMove(savedCoordinates[0], savedCoordinates[1], x, y);
            }

            // AI turn right after player's if enabled
            if (ui->aiBox->isChecked()) {
                if (game->checkGameOver()) {
                    part_1 = !part_1;
                    updateInformation();
                    return;
                }

                performAITurn();
            }
            // Else switch turn
            else {
                game->switchTurn();
                displayPlayerTurn();
            }

            // Update game
            part_1 = !part_1;
            updateBoard();
            updateInformation();
        }
    }
}

/**
 * @brief MainWindow::startGame, starts the game, sets everything up
 */

void MainWindow::startGame() {
    // Create a new game
    game = new Game();

    if (ui->aiBox->isChecked())
        game->createAI();

    // Initialize the board's UI
    if (firstStart) {
        firstStart = !firstStart;
        associateButtons();
    }

    setButtonsColor();
    updateBoard();
    updateInformation();

    QString message = QString::fromStdString(" >>> Board generated\n"
                                             " >>> Good luck");
    ui->messageText->append(message);

    inProgress = true;
    setButtonNames();

    // If AI is green, perform first move
    if (ui->aiBox->isChecked()) {
        if (ui->greenRadio->isChecked()) {
            performAITurn();
            updateBoard();
            message = QString::fromStdString("");
        }
        else
            message = QString::fromStdString(" >>>\n >>> Player 1 turn");
    }
    else
        message = QString::fromStdString(" >>>\n >>> Player 1 turn");

    ui->messageText->append(message);

    setMenuButtonsColors(true);
}

/**
 * @brief MainWindow::restartGame, resets the game to its idle state, allowing for a rematch
 */

void MainWindow::restartGame() {
    disableAllButtons();
    inProgress = false;

    // Reset the style of all buttons beck to idle state
    for (unsigned int i = 0; i < gameButtons.size(); i++) {
        for (unsigned int j = 0; j < gameButtons[i].size(); j++) {
            if (game->getBoard()->getTileColor(i, j))
                gameButtons[i][j]->setStyleSheet("border: 1px solid gray;"
                                                 "background: white;");
            else
                gameButtons[i][j]->setStyleSheet("border: 1px solid gray;"
                                                 "background: black;");

            gameButtons[i][j]->setIcon(QIcon());
            gameButtons[i][j]->setEnabled(false);
        }
    }

    setMenuButtonsColors(false);
}

/**
 * @brief MainWindow::clearMessages, clears all messages in the log
 */

void MainWindow::clearMessages() {
    ui->messageText->clear();

    QString message;

    if (game->getTurn())
        message = QString::fromStdString(" >>> Player 1 turn");
    else
        message = QString::fromStdString(" >>> Player 2 turn");

    ui->messageText->append(message);
}

/**
 * @brief MainWindow::expand, expands all nodes of the AI tree at once
 */

void MainWindow::expand() {
    ui->tree->expandAll();
}

/**
 * @brief MainWindow::collapse, collapses all nodes of the AI tree at once
 */

void MainWindow::collapse() {
    ui->tree->collapseAll();
}

/**
 * @brief MainWindow::updateBoard, updates token icons on the board depending on who owns them
 */

void MainWindow::updateBoard() {

    int redIconSize = 30;
    int greenIconSize = 25;

    for (int i = 0; i < game->getBoard()->getWidth(); i++) {
        for (int j = 0; j < game->getBoard()->getHeight(); j++) {
            if (ui->aiBox->isChecked()) {
                switch(game->getBoard()->getValueAt(i, j)) {
                case 'R':
                    if (part_1) {
                        if (ui->greenRadio->isChecked())
                            gameButtons[i][j]->setEnabled(true);
                        else
                            gameButtons[i][j]->setEnabled(false);
                    }
                    else {
                        gameButtons[i][j]->setEnabled(false);
                    }

                    gameButtons[i][j]->setIcon(redIcon);
                    gameButtons[i][j]->setIconSize(QSize(redIconSize, redIconSize));
                    break;
                case 'G':
                    if (part_1) {
                        if (ui->redRadio->isChecked())
                            gameButtons[i][j]->setEnabled(true);
                        else
                            gameButtons[i][j]->setEnabled(false);
                    }
                    else {
                        gameButtons[i][j]->setEnabled(false);
                    }

                    gameButtons[i][j]->setIcon(greenIcon);
                    gameButtons[i][j]->setIconSize(QSize(greenIconSize, greenIconSize));
                    break;
                case 'X':
                    gameButtons[i][j]->setEnabled(false);
                    gameButtons[i][j]->setIcon(QIcon());
                    gameButtons[i][j]->setIconSize(QSize(0, 0));
                    break;
                }
            }
            else {
                switch(game->getBoard()->getValueAt(i, j)) {
                case 'R':
                    if (game->getTurn() || !part_1)
                        gameButtons[i][j]->setEnabled(false);
                    else {
                        gameButtons[i][j]->setEnabled(true);
                    }

                    gameButtons[i][j]->setIcon(redIcon);
                    gameButtons[i][j]->setIconSize(QSize(redIconSize, redIconSize));
                    break;
                case 'G':
                    if (!(game->getTurn()) || !part_1)
                        gameButtons[i][j]->setEnabled(false);
                    else {
                        gameButtons[i][j]->setEnabled(true);
                    }

                    gameButtons[i][j]->setIcon(greenIcon);
                    gameButtons[i][j]->setIconSize(QSize(greenIconSize, greenIconSize));
                    break;
                case 'X':
                    gameButtons[i][j]->setEnabled(false);
                    gameButtons[i][j]->setIcon(QIcon());
                    gameButtons[i][j]->setIconSize(QSize(0, 0));
                    break;
                }
            }
        }
    }
}

/**
 * @brief MainWindow::setButtonsColor, resets the tile colors according to black/white retrictions
 */

void MainWindow::setButtonsColor() {
    for (int i = 0; i < (signed)gameButtons.size(); i++) {
        for (int j = 0; j < (signed)gameButtons[i].size(); j++) {
            if (game->getBoard()->getTileColor(i, j)) {
                gameButtons[i][j]->setStyleSheet("QPushButton{"
                                                 "  background: white;"
                                                 "  border: 1px solid gray;"
                                                 "}"
                                                 "QPushButton:hover{"
                                                 "  border: 5px solid gray;"
                                                 "}");
            }
            else {
                gameButtons[i][j]->setStyleSheet("QPushButton{"
                                                 "  background: black;"
                                                 "  border: 1px solid gray;"
                                                 "}"
                                                 "QPushButton:hover{"
                                                 "  border: 5px solid gray;"
                                                 "}");
            }
        }
    }
}

/**
 * @brief MainWindow::associateButtons, associates UI buttons with an easy to iterate 2 dimensional vector
 */

void MainWindow::associateButtons() {
    gameButtons.resize(game->getBoard()->getWidth());

    for (unsigned int i = 0; i < gameButtons.size(); i++)
        gameButtons[i].resize(game->getBoard()->getHeight());

    for (unsigned int i = 0; i < gameButtons.size(); i++) {
        for (unsigned int j = 0; j < gameButtons[i].size(); j++) {
            QLayoutItem* item = ui->gameLayout->itemAtPosition(j + 1, i + 1);
            QWidget* widget = item->widget();
            gameButtons[i][j] = dynamic_cast<QPushButton*>(widget);

            // Associate clicked action to clicked function
            connect(gameButtons[i][j], SIGNAL(clicked()), SLOT(gameButtonClicked()));
        }
    }
}

/**
 * @brief MainWindow::setButtonNames, sets the name of the buttons according to their (x, y) coordinates
 */

void MainWindow::setButtonNames() {
    QString name;

    for (int i = 0; i < game->getBoard()->getWidth(); i++) {
        buttonNames.push_back(std::vector<QString>());

        for (int j = 0; j < game->getBoard()->getHeight(); j++) {
            switch(j) {
            case 0:
                name = QString::fromStdString("A") + QString::number(i + 1);
                break;
            case 1:
                name = QString::fromStdString("B") + QString::number(i + 1);
                break;
            case 2:
                name = QString::fromStdString("C") + QString::number(i + 1);
                break;
            case 3:
                name = QString::fromStdString("D") + QString::number(i + 1);
                break;
            case 4:
                name = QString::fromStdString("E") + QString::number(i + 1);
                break;
            }

            buttonNames[i].push_back(name);
        }
    }
}

/**
 * @brief MainWindow::updateInformation, updates menu information and checks for different game over states
 */

void MainWindow::updateInformation() {
    ui->offensiveMovesCounter->setText(QString::number(game->getOffensiveMoveCtr()));
    ui->defMovesCounter->setText(QString::number(game->getDefensiveMoveCtr()));
    ui->totalCounter->setText(QString::number(game->getMoveCtr()));
    ui->p1TokenCounter->setText(QString::number(game->getPlayerTokens(0)));
    ui->p2TokenCounter->setText(QString::number(game->getPlayerTokens(1)));

    // Check if the game is a stalemate
    if (game->checkStalemate()) {
        ui->messageText->append(QString::fromStdString(" >>>\n >>> Game over - Stalemate\n"
                                                       " >>> Press restart to play again"));

        disableAllButtons();

        // Display stalemate popup
        QLabel *stalemateLabel = new QLabel();
        stalemateLabel->setText("Stalemate!");

        QDialog* popup = new QDialog();
        popup->setWindowTitle("Game Over");
        popup->setWindowIcon(QIcon(QPixmap(":/images/images/checkers.png")));
        popup->setStyleSheet("border: 0px;"
                             "background: black;"
                             "color: green;");

        delete popup->layout();

        QVBoxLayout* vLayout = new QVBoxLayout();
        popup->setLayout(vLayout);

        stalemateLabel->setAlignment(Qt::AlignCenter);
        stalemateLabel->setFont(QFont(QString::fromStdString("arial"), 20, 1, false));
        popup->layout()->addWidget(stalemateLabel);

        QMovie *gif = new QMovie(":images/images/stalemate.gif");
        QLabel *gifLabel = new QLabel(this);
        gifLabel->setMovie(gif);
        gif->start();
        popup->layout()->addWidget(gifLabel);

        popup->show();
    }
    else if (game->checkGameOver()) {
        QLabel *winnerLabel = new QLabel();

        disableAllButtons();

        if (game->getPlayerTokens(0) > game->getPlayerTokens(1)) {
            ui->messageText->append(QString::fromStdString(" >>>\n >>> Game over - Player 1 wins\n"
                                                           " >>> Press restart to play again"));
            winnerLabel->setText("Player 1 wins!");
        }
        else {
            ui->messageText->append(QString::fromStdString(" >>>\n >>> Game over - Player 2 wins\n"
                                                           " >>> Press restart to play again"));
            winnerLabel->setText("Player 2 wins!");
        }

        // Display game over popup
        QDialog* popup = new QDialog();
        popup->setWindowTitle("Game Over");
        popup->setWindowIcon(QIcon(QPixmap(":/images/images/checkers.png")));
        popup->setStyleSheet("border: 0px;"
                             "background: black;"
                             "color: green;");

        delete popup->layout();

        QVBoxLayout* vLayout = new QVBoxLayout();
        popup->setLayout(vLayout);

        winnerLabel->setAlignment(Qt::AlignCenter);
        winnerLabel->setFont(QFont(QString::fromStdString("arial"), 20, 1, false));
        popup->layout()->addWidget(winnerLabel);

        QMovie *gif = new QMovie(":images/images/fireworks.gif");
        QLabel *gifLabel = new QLabel(this);
        gifLabel->setMovie(gif);
        gif->start();
        popup->layout()->addWidget(gifLabel);

        popup->show();
    }
}

/**
 * @brief MainWindow::disableAllButtons, disables all buttons on the UI board
 */

void MainWindow::disableAllButtons() {
    for (unsigned i = 0; i < gameButtons.size(); i++) {
        for (unsigned j = 0; j < gameButtons[i].size(); j++) {
            gameButtons[i][j]->setEnabled(false);
        }
    }
}

/**
 * @brief MainWindow::displayPlayerTurn, displays whose player turn it is on the log
 */

void MainWindow::displayPlayerTurn() {
    QString message;

    if (game->getTurn())
        message = QString::fromStdString(" >>>\n >>> Player 1 turn");
    else
        message = QString::fromStdString(" >>>\n >>> Player 2 turn");

    ui->messageText->append(message);
}

/**
 * @brief MainWindow::setClickedButtonColor, changes the color of clicked button
 * @param x, x coordinate of button
 * @param y, y coordinate of button
 */

void MainWindow::setClickedButtonColor(int x, int y) {
    gameButtons[x][y]->setEnabled(true);

    if (game->getBoard()->getTileColor(x, y)) {
        gameButtons[x][y]->setStyleSheet("QPushButton{"
                              "  background: white;"
                              "  border: 5px solid #3B42F7;"
                              "}");
    }
    else {
        gameButtons[x][y]->setStyleSheet("QPushButton{"
                              "  background: black;"
                              "  border: 5px solid #3B42F7;"
                              "}");
    }
}

/**
 * @brief MainWindow::performAITurn, performs the AI turn and displays the action it took
 */

void MainWindow::performAITurn() {
    ui->messageText->append(QString::fromStdString(" >>>\n >>> Player AI turn"));
    bool isMinimax = ui->algoRadio_1->isChecked();
    int heuristicIndex;

    if (ui->heuristicRadio_1->isChecked())
        heuristicIndex = 0;
    else if (ui->heuristicRadio_2->isChecked())
        heuristicIndex = 1;
    else if (ui->heuristicRadio_3->isChecked())
        heuristicIndex = 2;
    else
        heuristicIndex = 0;

    // Begin timer
    clock_t begin = clock();

    vector<vector<int>> nextMove;
    vector<vector<int>> removedTokens;
    vector<vector<char>> currentState = game->getBoard()->getMatrix();

    // Check whether AI is red or green
    if (ui->redRadio->isChecked())
        nextMove = game->getAI()->getNextMoveFromAI(ui->depthSlider->value() + 1, 'R', isMinimax, heuristicIndex);
    else {
        nextMove = game->getAI()->getNextMoveFromAI(ui->depthSlider->value() + 1, 'G', isMinimax, heuristicIndex);
    }

    int x1 = nextMove[0][0];
    int y1 = nextMove[0][1];
    int x2 = nextMove[1][0];
    int y2 = nextMove[1][1];

    game->attack(x1, y1, x2, y2);

    clock_t end = clock();
    double elapsedTime = double(end - begin) / CLOCKS_PER_SEC;

    vector<vector<char>> newState = game->getBoard()->getMatrix();

    // Check whether AI is red or green
    if (ui->redRadio->isChecked())
        removedTokens = game->getBoard()->getRemovedTokens(currentState, newState, 'R');
    else {
        removedTokens = game->getBoard()->getRemovedTokens(currentState, newState, 'G');
    }

    setRemovedTokensColors(removedTokens);

    // Display AI move in log
    QString message = QString::fromStdString(" >>> Player AI moves token ")
            + buttonNames[x1][y1]
            + QString::fromStdString(" to ")
            + buttonNames[x2][y2]
            + QString::fromStdString("\n >>> Time elapsed: ")
            + QString::number(elapsedTime)
            + QString::fromStdString("\n >>>\n >>> Player 1 turn");

    ui->messageText->append(message);

    // Clear and repopulate AI tree
    ui->tree->clear();
    game->getAI()->setTree(ui->tree);

    if (game->getBoard()->getTileColor(x1, y1)) {
        gameButtons[x1][y1]->setStyleSheet("QPushButton{"
                              "  background: white;"
                              "  border: 5px solid #E06104;"
                              "}");
    }
    else {
        gameButtons[x1][y1]->setStyleSheet("QPushButton{"
                              "  background: black;"
                              "  border: 5px solid #E06104;"
                              "}");
    }

    if (game->getBoard()->getTileColor(x2, y2)) {
        gameButtons[x2][y2]->setStyleSheet("QPushButton{"
                              "  background: white;"
                              "  border: 5px solid #E06104;"
                              "}");
    }
    else {
        gameButtons[x2][y2]->setStyleSheet("QPushButton{"
                              "  background: black;"
                              "  border: 5px solid #E06104;"
                              "}");
    }
}

/**
 * @brief MainWindow::displayMove, displays player move in the log
 * @param x1, original x coordinate
 * @param y1, original y coordinate
 * @param x2, destination x coordinate
 * @param y2, destination y coordinate
 */

void MainWindow::displayMove(int x1, int y1, int x2, int y2) {
    QString message;

    if (game->getTurn()) {
        message = QString::fromStdString(" >>> Player 1 moves token ");
        message += buttonNames[x1][y1];
        message += QString::fromStdString(" to ");
        message += buttonNames[x2][y2];
    }
    else {
        message = QString::fromStdString(" >>> Player 2 moves token ");
        message += buttonNames[x1][y1];
        message += QString::fromStdString(" to ");
        message += buttonNames[x2][y2];
    }

    ui->messageText->append(message);
}

/**
 * @brief MainWindow::setAdjacentColors, sets the color of selected button adjacent tiles
 * @param x, x coordinate of selected button
 * @param y, y coordinate of selected button
 */

void MainWindow::setAdjacentColors(int x, int y) {
    vector<vector<int>> validCoordinates = game->getBoard()->getEmptyAdjacentValidTiles(x, y);
    vector<vector<int>> invalidCoordinates = game->getBoard()->getEmptyAdjacentInvalidTiles(x, y);

    for (unsigned int i = 0; i < validCoordinates.size(); i++) {
        x = validCoordinates[i][0];
        y = validCoordinates[i][1];

        if (game->getBoard()->getTileColor(x, y)) {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: white;"
                                  "  border: 5px solid #1ABC9C;"
                                  "}");
        }
        else {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: black;"
                                  "  border: 5px solid #1ABC9C;"
                                  "}");
        }
        gameButtons[x][y]->setEnabled(true);
    }

    for (unsigned int i = 0; i < invalidCoordinates.size(); i++) {
        x = invalidCoordinates[i][0];
        y = invalidCoordinates[i][1];

        if (game->getBoard()->getTileColor(x, y)) {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: white;"
                                  "  border: 5px solid #FF4674;"
                                  "}");
        }
        else {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: black;"
                                  "  border: 5px solid #FF4674;"
                                  "}");
        }
    }
}

/**
 * @brief MainWindow::setMenuButtonsColors, sets the color of enabled/disabled AI options
 * @param isStart, checks if it's the start or reset button clicked
 */

void MainWindow::setMenuButtonsColors(bool isStart) {
    if (isStart) {
        ui->redRadio->setEnabled(false);
        ui->redRadio->setStyleSheet("color:gray;");
        ui->greenRadio->setEnabled(false);
        ui->greenRadio->setStyleSheet("color:gray;");
        ui->aiBox->setEnabled(false);
        ui->aiBox->setStyleSheet("color:gray;"
                                 "border:0px");
        ui->startButton->setEnabled(false);
        ui->startButton->setStyleSheet("border:1px solid gray;"
                                         "color:gray;");
        ui->restartButton->setEnabled(true);
        ui->restartButton->setStyleSheet("QPushButton{"
                                         "  border:1px solid green;"
                                         "  color:green;"
                                         "}"
                                         "QPushButton::hover{"
                                         "  background:gray;"
                                         "  color:black;"
                                         "}");
        ui->colorLabel->setStyleSheet("color:gray;"
                                      "border:0px;");
    }
    else {
        ui->aiBox->setEnabled(true);
        ui->aiBox->setStyleSheet("color:green;"
                                 "border:0px;");
        ui->redRadio->setEnabled(true);
        ui->redRadio->setStyleSheet("color:green;");
        ui->greenRadio->setEnabled(true);
        ui->greenRadio->setStyleSheet("color:green;");
        ui->restartButton->setEnabled(false);
        ui->restartButton->setStyleSheet("QPushButton{"
                                         "  border:1px solid gray;"
                                         "  color:gray;"
                                         "}");
        ui->startButton->setEnabled(true);
        ui->startButton->setStyleSheet("QPushButton{"
                                       "  border:1px solid green;"
                                       "  color:green;"
                                       "}"
                                       "QPushButton::hover{"
                                       "  background:gray;"
                                       "  color:black;"
                                       "}");
        ui->colorLabel->setStyleSheet("color:green;"
                                      "border:0px;");
        ui->messageText->clear();
        ui->tree->clear();
    }
}

/**
 * @brief MainWindow::setRemovedTokensColors, sets the color of removed token as a result of a given move
 * @param removedTokens, a list of all removed tokens
 */

void MainWindow::setRemovedTokensColors(vector<vector<int>> removedTokens) {
    for (unsigned int i = 0; i < removedTokens.size(); i++) {

        int x = removedTokens[i][0];
        int y = removedTokens[i][1];

        if (game->getBoard()->getTileColor(x, y)) {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: white;"
                                  "  border: 5px solid #AC35FC;"
                                  "}");
        }
        else {
            gameButtons[x][y]->setStyleSheet("QPushButton{"
                                  "  background: black;"
                                  "  border: 5px solid #AC35FC;"
                                  "}");
        }
    }
}
