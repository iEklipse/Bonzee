#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QIcon>
#include <QDebug>
#include <QMovie>
#include <QDialog>
#include <QThread>
#include <vector>
#include <iostream>
#include <sstream>
#include <ctime>

#include "game.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::vector<std::vector<QPushButton*>> gameButtons;
    std::vector<std::vector<QString>> buttonNames;
    QIcon greenIcon;
    QIcon redIcon;
    Game* game;
    bool inProgress;
    bool part_1;
    bool firstStart;
    std::vector<int> savedCoordinates;

    void updateBoard();
    void setButtonsColor();
    void associateButtons();
    void updateInformation();
    void setButtonNames();
    void disableAllButtons();
    void displayPlayerTurn();
    void setClickedButtonColor(int x, int y);
    void performAITurn();
    void displayMove(int x1, int y1, int x2, int y2);
    void setAdjacentColors(int x, int y);
    void setMenuButtonsColors(bool isStart);
    void setRemovedTokensColors(vector<vector<int>> removedTokens);

private slots:
    void gameButtonClicked();
    void startGame();
    void restartGame();
    void clearMessages();
    void expand();
    void collapse();
};

#endif // MAINWINDOW_H
