#ifndef BOARD_H
#define BOARD_H

#include <vector>

#include "integer.h"

using namespace std;

class Board
{
private:
    const int WIDTH = 9;
    const int HEIGHT = 5;
    vector<vector<char>> boardMatrix;

public:
    Board();
    Board(vector<vector<char> >);
    int getWidth();
    int getHeight();
    vector<vector<char>> getMatrix();
    char getValueAt(int x, int y);
    bool getTileColor(int x, int y);
    bool checkMove(int x1, int y1, int x2, int y2);
    void performAttack(int x1, int y1, int x2, int y2, Integer* moveCtr, Integer* defensiveMoveCtr, Integer* offensiveMoveCtr, Integer* p1Tokens, Integer* p2Tokens);
    vector<vector<int>> getEmptyAdjacentValidTiles(int x, int y);
    vector<vector<int>> getEmptyAdjacentInvalidTiles(int x, int y);
    int getTokenStreak(int x, int y, char player, vector<vector<char>> currentState);
    int getTokenAmount(char player);
    void updateBoard(int x1, int y1, int x2, int y2);
    void printBoard();
    void attack(int x, int y, vector<int> direction, char currentPlayer, Integer* moveCtr, Integer* defensiveMoveCtr, Integer* offensiveMoveCtr, Integer* p1Tokens, Integer* p2Tokens);
    void setMatrix(vector<vector<char>> new_matrix);
    vector<vector<int>> getRemovedTokens(vector<vector<char>> state_original, vector<vector<char>> state_new, char currentPlayer);
};

#endif // BOARD_H
