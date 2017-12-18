#ifndef PLAYER_H
#define PLAYER_H

class Player
{
private:
    int tokenAmt;
    bool color;
public:
    Player(bool color);
    int getTokenAmt();
    bool getColor();
    void setTokenAmt(int amt);
    void setColor(bool color);
    void Move();
};

#endif // PLAYER_H
