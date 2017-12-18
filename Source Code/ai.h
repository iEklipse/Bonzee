#ifndef AI_H
#define AI_H

#include "board.h"
#include <vector>
#include <QTreeView>
#include <QTreeWidgetItem>

using namespace std;

class AIPlayer{
private:
    Board* board; //Refers to the current game
    QTreeWidget *tree;
    vector<int> frontierValues;

public:
    AIPlayer(Board* current_board);

    // return a vector of matrices representing all
    // the possible states that the AI can choose
    vector<vector<vector<char>>> getFrontierStates(vector<vector<char> > state, char currentPlayer);

    // return heuristic associated to a given state
    int naiveHeuristic(vector<vector<char>> state);
    int countingHeuristic(vector<vector<char>> state);
    int informedHeuristic(vector<vector<char>> previousState, vector<vector<char>> state, char currentPlayer);

    // recursively calculate the heuristic value
    // of a minimax node and return the minmax
    // value at the leaves
    int minimax(vector<vector<char>> previousState, vector<vector<char>> state, char currentPlayer, int level, int depth, bool min_level, int heuristicIndex, QTreeWidgetItem *root);
    int alphabeta(vector<vector<char>> previousState, vector<vector<char>> state, char currentPlayer, int level, int depth, int alpha, int beta, bool min_level, int heuristicIndex, QTreeWidgetItem *root);

    // return a vector of 2 vec2 (x,y)
    // nextMove[origPos, destPos]
    // origPos[x0, y0] --> original position of the token that moved
    // destPos[x1, y1] -->
    vector<vector<int>> nextMove(vector<vector<char> > state_original, vector<vector<char> > state_new, char opponentPlayer);
    vector<vector<int>> getNextMoveFromAI(int level, char currentPlayer, bool isMinimax, int heuristicIndex);

    void setTree(QTreeWidget* tree);
};

#endif // AI_H

