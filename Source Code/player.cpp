#include "player.h"

Player::Player(bool color)
{
    /* Row 1 = 9
     * Row 2 = 9
     * Row 3 = 4
     * ---------
     *        22 */

    tokenAmt = 22;
    this->color = color;
}

int Player::getTokenAmt() {
    return tokenAmt;
}

bool Player::getColor() {
    return color;
}

void Player::setTokenAmt(int amt) {
    tokenAmt += amt;
}

void Player::setColor(bool color) {
    this->color = color;
}

void Player::Move() {
    // TO DO
}
