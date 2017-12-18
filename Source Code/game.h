#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"
#include "ai.h"
#include "integer.h"

class Game
{
private:
    Board* board;
    Board* AI_board;
    AIPlayer* ai;
    Integer* p1Tokens;
    Integer* p2Tokens;
    bool turn;
    bool isGameOver;
    Integer* defensiveMoveCtr;
    Integer* offensiveMoveCtr;
    Integer* moveCtr;

public:
    Game();
    ~Game();
    Board *getBoard();
    void attack(int x1, int y1, int x2, int y2);
    int getPlayerTokens(int player);
    int getDefensiveMoveCtr();
    int getOffensiveMoveCtr();
    int getMoveCtr();
    bool getTurn();
    AIPlayer* getAI();
    bool checkGameOver();
    void switchTurn();
    void createAI();
    void setPlayerTokens(int player, int tokens);
    void updateDefensiveMoveCtr(int amt);
    void updateOffensiveMoveCtr(int amt);
    void updateMoveCtr(int amt);
    bool checkStalemate();
    void restart();
};

#endif // GAME_H
