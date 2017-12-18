#include "board.h"
#include "game.h"

#include <iostream>
#include <string>
#include <QDebug>

Board::Board() {
    // Resize the first vector dimension to the width
    boardMatrix.resize(WIDTH);

    for (int x = 0; x < WIDTH; ++x) {
        // Resize the second vector dimension to the height
        boardMatrix[x].resize(HEIGHT);

        for (int y = 0; y < HEIGHT; ++y) {
            // If first 2 rows, red
            if (y <= 1)
                boardMatrix[x][y] = 'R';

            // If second row, split
            if (y == 2) {

                /* If first 4 columns, green
                * If middle column, empty
                * If last 4 columns, red */

                if (x <= 3)
                    boardMatrix[x][y] = 'G';
                else if (x >= 5)
                    boardMatrix[x][y] = 'R';
                else
                    boardMatrix[x][y] = 'X';
            }

            // If last 2 rows, green
            if (y >= 3)
                boardMatrix[x][y] = 'G';
        }
    }
}

// constructor initialised by preexisting state. Used by AI
Board::Board(vector<vector<char> > matrix)
{
    boardMatrix = matrix;
}

int Board::getWidth() {
    return WIDTH;
}

int Board::getHeight() {
    return HEIGHT;
}

vector<vector<char>> Board::getMatrix() {
    return boardMatrix;
}

char Board::getValueAt(int x, int y) {
    return boardMatrix[x][y];
}

/**
 * @brief Board::getTileColor, returns the color of tile at x, y coordinates
 * @param x, position x
 * @param y, position y
 * @return boolean, true = white, false = black
 */

bool Board::getTileColor(int x, int y) {

    /* If y is ODD and x is ODD - Black (false)
     * If y is ODD and x is EVEN - White (true)
     * If y is EVEN and x is ODD - White (true)
     * If y is EVEN and x is EVEN - Black (false) */

    if (y % 2 == 0) {
        if (x % 2 == 0)
            return false;
        else
            return true;
    }
    else {
        if (x % 2 == 0)
            return true;
        else
            return false;
    }
}

/**
 * @brief Board::checkMove, checks if the move is a valid, if it is, change it in the boardMatrix
 * @param x1, origin x position
 * @param y1, origin y position
 * @param x2, destination x position
 * @param y2, destination y position
 * @return boolean, true = valid move, false = invalid move
 */
bool Board::checkMove(int x1, int y1, int x2, int y2) {

    /*  NORTH       (0, -1) - (0, 0) = (0, 1)
     *  NORTH-EAST  (1, -1) - (0, 0) = (1, 1)
     *  EAST        (1, 0) - (0, 0) = (1, 0)
     *  SOUTH-EAST  (1, 1) - (0, 0) = (1, -1)
     *  SOUTH       (0, 1) - (0, 0) = (0, -1)
     *  SOUTH-WEST  (-1, 1) - (0, 0) = (-1, -1)
     *  WEST        (-1, 0) - (0, 0) = (-1, 0)
     *  NORTH-WEST  (-1, -1) - (0, 0) = (-1, 1) */

    vector<int> direction = {x2 - x1, y2 - y1};
    bool tileColor = getTileColor(x1, y1);

    // Depending of the token current tile color, if it's on a black tile, diagonal move is not allowed
    if (tileColor && (direction[0] == 1 || direction[0] == -1) && (direction[1] == 1 || direction[1] == -1))
        return false;

    vector<vector<int>> emptyTiles = getEmptyAdjacentValidTiles(x1, y1);

    for (unsigned int i = 0; i < emptyTiles.size(); ++i)
    {
        if (emptyTiles[i][0] == x2 && emptyTiles[i][1] == y2)
        {

            // For Testing --> Prints out in the console if the move is valid
            //qDebug() << "The move" << "[" << x1 << "," << y1 << "]" << "to [" << x2 << "," << y2 << "]" << "is valid" << endl;
            return true;
        }
    }

    return false;
}
// Given a valid move, performs the attack
void Board::performAttack(int x1, int y1, int x2, int y2, Integer* moveCtr, Integer* defensiveMoveCtr, Integer* offensiveMoveCtr, Integer* p1Tokens, Integer* p2Tokens){
    char currentPlayer = boardMatrix[x1][y1];
    vector<int> direction = {x2 - x1, y2 - y1};
    attack(x2, y2, direction, currentPlayer, moveCtr, defensiveMoveCtr, offensiveMoveCtr, p1Tokens, p2Tokens);
    updateBoard(x1, y1, x2, y2);
}

/**
 * @brief Board::getEmptyAdjacentValidTiles, Gets and returns a vector of vec2[x, y] of empty adjacent valid tiles of the given tile's coordinates
 * @param x, x position of the chosen token to move
 * @param y, y position of the chosen token to move
 * @return emptyValidTiles, vector of vec2[x, y]
 */
vector<vector<int>> Board::getEmptyAdjacentValidTiles(int x, int y) {
    bool tileColor = getTileColor(x, y);
    vector<vector<int>> emptyValidTiles;
    vector<int> vec(2);

    if (y - 1 >= 0 && boardMatrix[x][y - 1] == 'X') // NORTH
    {
        vec[0] = x;
        vec[1] = y - 1;
        emptyValidTiles.push_back(vec);
    }
    if (x + 1 < WIDTH && y - 1 >= 0 && boardMatrix[x + 1][y - 1] == 'X' && !tileColor) // NORTH-EAST
    {
        vec[0] = x + 1;
        vec[1] = y - 1;
        emptyValidTiles.push_back(vec);
    }
    if (x + 1 < WIDTH && boardMatrix[x + 1][y] == 'X') // EAST
    {
        vec[0] = x + 1;
        vec[1] = y;
        emptyValidTiles.push_back(vec);
    }
    if (x + 1 < WIDTH && y + 1 < HEIGHT && boardMatrix[x + 1][y + 1] == 'X' && !tileColor) // SOUTH-EAST
    {
        vec[0] = x + 1;
        vec[1] = y + 1;
        emptyValidTiles.push_back(vec);
    }
    if (y + 1 < HEIGHT && boardMatrix[x][y + 1] == 'X') // SOUTH
    {
        vec[0] = x;
        vec[1] = y + 1;
        emptyValidTiles.push_back(vec);
    }
    if (x - 1 >= 0 && y + 1 < HEIGHT && boardMatrix[x - 1][y + 1] == 'X' && !tileColor) // SOUTH-WEST
    {
        vec[0] = x - 1;
        vec[1] = y + 1;
        emptyValidTiles.push_back(vec);
    }
    if (x - 1 >= 0 && boardMatrix[x - 1][y] == 'X') // WEST
    {
        vec[0] = x - 1;
        vec[1] = y;
        emptyValidTiles.push_back(vec);
    }
    if (x - 1 >= 0 && y - 1 >= 0 && boardMatrix[x - 1][y - 1] == 'X' && !tileColor) // NORTH-WEST
    {
        vec[0] = x - 1;
        vec[1] = y - 1;
        emptyValidTiles.push_back(vec);
    }

    /* For Testing --> To print out in the console the the vector of empty adjacent tiles
    for (int i = 0; i < emptyTiles.size(); ++i)
    {
        qDebug() << "[" << emptyTiles[i][0] << "," << emptyTiles[i][1] << "]" << endl;
    }
    */

    return emptyValidTiles;
}

/**
 * @brief Board::getEmptyAdjacentInvalidTiles, Gets and returns a vector of vec2[x, y] of empty adjacent invalid tiles of the given tile's coordinates
 * @param x, x position of the chosen token to move
 * @param y, y position of the chosen token to move
 * @return emptyInvalidTiles, vector of vec2[x, y]
 */
vector<vector<int>> Board::getEmptyAdjacentInvalidTiles(int x, int y)
{
    bool tileColor = getTileColor(x, y);
    vector<vector<int>> emptyInvalidTiles;
    vector<int> vec(2);

    if (tileColor)
    {
        if (x + 1 < WIDTH && y - 1 >= 0 && boardMatrix[x + 1][y - 1] == 'X') // NORTH-EAST
        {
            vec[0] = x + 1;
            vec[1] = y - 1;
            emptyInvalidTiles.push_back(vec);
        }
        if (x + 1 < WIDTH && y + 1 < HEIGHT && boardMatrix[x + 1][y + 1] == 'X') // SOUTH-EAST
        {
            vec[0] = x + 1;
            vec[1] = y + 1;
            emptyInvalidTiles.push_back(vec);
        }
        if (x - 1 >= 0 && y + 1 < HEIGHT && boardMatrix[x - 1][y + 1] == 'X') // SOUTH-WEST
        {
            vec[0] = x - 1;
            vec[1] = y + 1;
            emptyInvalidTiles.push_back(vec);
        }
        if (x - 1 >= 0 && y - 1 >= 0 && boardMatrix[x - 1][y - 1] == 'X') // NORTH-WEST
        {
            vec[0] = x - 1;
            vec[1] = y - 1;
            emptyInvalidTiles.push_back(vec);
        }

        return emptyInvalidTiles;
    }

    return emptyInvalidTiles;
}

int Board::getTokenStreak(int x, int y, char player, vector<vector<char>> currentState) {
    int total = 0;
    int count = 0;
    int currentX = x - 1;
    int currentY = y + 1;

    // WEST Check
    while (currentX >= 0) {
        if (currentState[currentX][y] == player)
            count++;
        else
            break;
        currentX--;
    }

    if (count > total)
        total = count;

    currentX = x + 1;
    count = 0;

    // EAST Check
    while (currentX < WIDTH) {
        if (currentState[currentX][y] == player)
            count++;
        else
            break;
        currentX++;
    }

    if (count > total)
        total = count;

    currentX = x - 1;
    count = 0;

    // NORTH Check
    while (currentY < HEIGHT) {
        if (currentState[x][currentY] == player)
            count++;
        else
            break;
        currentY++;
    }

    if (count > total)
        total = count;

    currentY = y - 1;
    count = 0;

    // SOUTH Check
    while (currentY >= 0) {
        if (currentState[x][currentY] == player)
            count++;
        else
            break;
        currentY--;
    }

    if (count > total)
        total = count;

    currentY = y - 1;
    count = 0;

    // SOUTH WEST Check
    while (currentY >= 0 && currentX >= 0) {
        if (currentState[currentX][currentY] == player)
            count++;
        else
            break;
        currentY--;
        currentX--;
    }

    if (count > total)
        total = count;

    currentY = y - 1;
    currentX = x + 1;
    count = 0;

    // SOUTH EAST Check
    while (currentY >= 0 && currentX < WIDTH) {
        if (currentState[currentX][currentY] == player)
            count++;
        else
            break;
        currentY--;
        currentX++;
    }

    if (count > total)
        total = count;

    currentY = y + 1;
    currentX = x + 1;
    count = 0;

    // NORTH EAST Check
    while (currentY < HEIGHT && currentX < WIDTH) {
        if (currentState[currentX][currentY] == player)
            count++;
        else
            break;
        currentY++;
        currentX++;
    }

    if (count > total)
        total = count;

    currentY = y + 1;
    currentX = x - 1;
    count = 0;

    // NORTH WEST Check
    while (currentY < HEIGHT && currentX >= 0) {
        if (currentState[currentX][currentY] == player)
            count++;
        else
            break;
        currentY++;
        currentX--;
    }

    if (count > total)
        total = count;

    return total;
}

int Board::getTokenAmount(char player) {
    int count = 0;

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (boardMatrix[i][j] == player)
                count++;
        }
    }

    return count;
}

void Board::updateBoard(int x1, int y1, int x2, int y2)
{
    char playerToken = boardMatrix[x1][y1];

    boardMatrix[x1][y1] = 'X';
    boardMatrix[x2][y2] = playerToken;
}

void Board::printBoard()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            qDebug() << boardMatrix[x][y];
        }
        qDebug() << endl;
    }
}

/**
 * @brief Board::attack, removes the opponent tokens with forward/backward attack
 * @param x, x position of the current player's token landing position
 * @param y, y position of the current player's token landing position
 * @param direction, the direction of the current player's token
 * @param currentPlayer, the current player's token color
 */
void Board::attack(int x, int y, vector<int> direction, char currentPlayer, Integer* moveCtr, Integer* defensiveMoveCtr, Integer* offensiveMoveCtr, Integer* p1Tokens, Integer* p2Tokens)
{
    bool keepRemovingForward = true;
    bool keepRemovingBackward = false;
    int forwardTileX = x + direction[0];
    int forwardTileY = y + direction[1];
    int backwardTileX = x - direction[0] * 2;
    int backwardTileY = y - direction[1] * 2;
    char opponentToken;
    int numTokenRemoved = 0;

    if (currentPlayer == 'G')
        opponentToken = 'R';
    else
        opponentToken = 'G';

    // Foward Attack
    while (keepRemovingForward && forwardTileX >= 0 && forwardTileX < WIDTH && forwardTileY >= 0 && forwardTileY < HEIGHT)
    {
        // If forward attack did not remove any token, initiates backward attack
        if ((boardMatrix[forwardTileX][forwardTileY] == 'X' || boardMatrix[forwardTileX][forwardTileY] == currentPlayer)
                && numTokenRemoved == 0)
            keepRemovingForward = false;
        else if (boardMatrix[forwardTileX][forwardTileY] == opponentToken)
        {
            boardMatrix[forwardTileX][forwardTileY] = 'X';

            forwardTileX += direction[0];
            forwardTileY += direction[1];

            numTokenRemoved++;
        }
        else if (boardMatrix[forwardTileX][forwardTileY] == currentPlayer || boardMatrix[forwardTileX][forwardTileY] == 'X')
            keepRemovingForward = false;
    }

    // If forward attack did not remove anything, go for a backward attack
    if (numTokenRemoved == 0)
        keepRemovingBackward = true;

    // Backward Attack
    while (keepRemovingBackward && backwardTileX >= 0 && backwardTileX < WIDTH && backwardTileY >= 0 && backwardTileY < HEIGHT)
    {
        // If backward attack did not remove any token, then it was a defensive move
        if ((boardMatrix[backwardTileX][backwardTileY] == 'X' || boardMatrix[backwardTileX][backwardTileY] == currentPlayer)
                && numTokenRemoved == 0)
            keepRemovingBackward = false;
        else if (boardMatrix[backwardTileX][backwardTileY] == opponentToken)
        {
            boardMatrix[backwardTileX][backwardTileY] = 'X';

            backwardTileX -= direction[0];
            backwardTileY -= direction[1];

            numTokenRemoved++;
        }
        else if (boardMatrix[backwardTileX][backwardTileY] == currentPlayer || boardMatrix[backwardTileX][backwardTileY] == 'X')
            keepRemovingBackward = false;
    }

    // Defensive Move, increment the defensive move counter
    if (numTokenRemoved == 0) {
        defensiveMoveCtr->setValue(defensiveMoveCtr->getValue() + 1);
        offensiveMoveCtr->setValue(0);
    }
    else {
        defensiveMoveCtr->setValue(0);
        offensiveMoveCtr->setValue(offensiveMoveCtr->getValue() + 1);

        if (opponentToken == 'G')
            p1Tokens->setValue(p1Tokens->getValue() - numTokenRemoved);
        else
            p2Tokens->setValue(p2Tokens->getValue() - numTokenRemoved);
    }

    moveCtr->setValue(moveCtr->getValue() + 1);
}

void Board::setMatrix(vector<vector<char>> new_matrix){
    boardMatrix = new_matrix;
}

vector<vector<int>> Board::getRemovedTokens(vector<vector<char> > state_original, vector<vector<char> > state_new, char currentPlayer) {
    vector<int> position;
    vector<vector<int>> positions;

    // Iterate through the two states in order to find the token that moved
    for (int x = 0; x < WIDTH; ++x)
    {
        for (int y = 0; y < HEIGHT; ++y)
        {
            // If at (x,y) on both state doesn't match, that means a token moved from/to there
            if (state_original[x][y] != state_new[x][y])
            {
                // If the token at (x,y) is not the ai token in either states, continue to search (For now, R = AI token, G = Player Token)
                if (state_original[x][y] == currentPlayer || state_new[x][y] == currentPlayer)
                    continue;

                // If on the original state we get a 'X' at (x,y), the token will move to that (x,y)
                if (state_new[x][y] == 'X')
                {
                    position.push_back(x);
                    position.push_back(y);
                    positions.push_back(position);
                    position.clear();
                }
            }
        }
    }

    return positions;
}
