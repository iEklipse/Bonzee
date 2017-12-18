#include "game.h"

Game::Game()
{
    board = new Board();
    p1Tokens = new Integer(22);
    p2Tokens = new Integer(22);
    defensiveMoveCtr = new Integer(0);
    offensiveMoveCtr = new Integer(0);
    moveCtr = new Integer(0);
    turn = true;
    isGameOver = false;
}

Game::~Game() {
    delete board;
    delete AI_board;
}

Board* Game::getBoard() {
    return board;
}

void Game::attack(int x1, int y1, int x2, int y2) {
    board->performAttack(x1, y1, x2, y2, moveCtr, defensiveMoveCtr, offensiveMoveCtr, p1Tokens, p2Tokens);
}

int Game::getPlayerTokens(int player) {
    if (player <= 0)
        return p1Tokens->getValue();
    else
        return p2Tokens->getValue();
}

int Game::getDefensiveMoveCtr()
{
    return defensiveMoveCtr->getValue();
}

int Game::getOffensiveMoveCtr() {
    return offensiveMoveCtr->getValue();
}

int Game::getMoveCtr() {
    return moveCtr->getValue();
}

bool Game::getTurn() {
    return turn;
}

AIPlayer* Game::getAI() {
    return ai;
}

bool Game::checkGameOver() {
    if (p1Tokens->getValue() <= 0 || p2Tokens->getValue() <= 0)
        isGameOver = true;

    return isGameOver;
}

void Game::switchTurn() {
    turn = !turn;
}

void Game::createAI() {
    ai = new AIPlayer(board);
}

void Game::updateDefensiveMoveCtr(int amt) {
    if (amt == 0)
        defensiveMoveCtr->setValue(0);
    else
        defensiveMoveCtr->setValue(defensiveMoveCtr->getValue() + amt);
}

void Game::updateOffensiveMoveCtr(int amt) {
    if (amt == 0)
        offensiveMoveCtr->setValue(0);
    else
        offensiveMoveCtr->setValue(offensiveMoveCtr->getValue() + amt);
}

void Game::updateMoveCtr(int amt) {
    moveCtr->setValue(moveCtr->getValue() + amt);
}

void Game::setPlayerTokens(int player, int tokens) {
    if (player <= 0)
        p1Tokens->setValue(p1Tokens->getValue() - tokens);
    else
        p2Tokens->setValue(p2Tokens->getValue() - tokens);
}

bool Game::checkStalemate() {
    if (defensiveMoveCtr->getValue() >= 10)
        return isGameOver = true;

    return false;
}

void Game::restart() {
    delete board;

    board = new Board();
    p1Tokens->setValue(22);
    p2Tokens->setValue(22);
    offensiveMoveCtr->setValue(0);
    defensiveMoveCtr->setValue(0);
    moveCtr->setValue(0);
    turn = true;
    isGameOver = false;
}
