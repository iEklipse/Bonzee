#include "ai.h"
#include <QDebug>

AIPlayer::AIPlayer(Board *current_board) : board(current_board)
{
    tree = new QTreeWidget();
}

vector<vector<vector<char>>> AIPlayer::getFrontierStates(vector<vector<char> > state, char currentPlayer){

    vector<vector<vector<char>>> possible_states = {};
    
    // iterate across the matrix to find all AI tokens and
    // calculate frontier states depending on the available
    // moves of each token
    for (int i=0; i < board->getWidth(); i++){
        for(int j=0; j < board->getHeight(); j++){
            if (state[i][j] == currentPlayer){

                // temporary board object where we perform
                // move checking and attacks in order to retrieve
                // frontier states
                Board* temporary_board = new Board(state);

                vector<vector<int> > valid_adjancent_tiles = temporary_board->getEmptyAdjacentValidTiles(i,j);
                if (valid_adjancent_tiles.size() != 0){

                    //iterate over all valid adjacent tiles
                    for (auto valid_move : valid_adjancent_tiles) {

                        if (temporary_board->checkMove(i,j,valid_move[0],valid_move[1])){
                            Integer* tempInt = new Integer(0);
                            temporary_board->performAttack(i, j, valid_move[0], valid_move[1], tempInt, tempInt, tempInt, tempInt, tempInt);

                            // if an attack is performed, the resulting matrix is stored
                            // in the frontier states
                            possible_states.push_back(temporary_board->getMatrix());

                            // in order to proceed with the rest of the valid adjacent
                            // tiles, the temporary board must be reset to the current
                            // state
                            delete temporary_board;
                            temporary_board = new Board(state);
                        }

                    }
                }
                delete temporary_board;
            }
        }
    }

    return possible_states;
}

int AIPlayer::naiveHeuristic(vector<vector<char> > state){
    // naive heuristic function described in
    // the project section of the moodle page
    const int WIDTH = board->getWidth();
    const int HEIGHT = board->getHeight();

    int h_green_sum=0, h_red_sum=0, v_green_sum=0, v_red_sum=0;

    for (int i = 0; i < WIDTH; i++){
        for (int j = 0; j < HEIGHT; j++){
            if (state[i][j] == 'G'){
                h_green_sum += i+1;
                v_green_sum += j+1;
            }
            else if (state[i][j] == 'R'){
                h_red_sum += i+1;
                v_red_sum += j+1;
            }
        }
    }

    return 100*v_green_sum+50*h_green_sum-100*v_red_sum-50*h_red_sum;
}

int AIPlayer::countingHeuristic(vector<vector<char> > state){
    const int WIDTH = board->getWidth();
    const int HEIGHT = board->getHeight();

    int greenCtr = 0, redCtr = 0;

    for (int i = 0; i < WIDTH; i++){
        for (int j = 0; j < HEIGHT; j++){
            if (state[i][j] == 'G')
                greenCtr++;
            else if (state[i][j] == 'R')
                redCtr++;
        }
    }

    return greenCtr - redCtr;
}

int AIPlayer::informedHeuristic(vector<vector<char>> previousState, vector<vector<char> > state, char currentPlayer){
    const int WIDTH = board->getWidth();
    const int HEIGHT = board->getHeight();

    int greenCtr = 0, redCtr = 0;

    for (int i = 0; i < WIDTH; i++){
        for (int j = 0; j < HEIGHT; j++){
            if (state[i][j] == 'G') {
                if (board->getTileColor(i, j))
                    greenCtr += 100;
                else
                    greenCtr += 50;
            }
            else if (state[i][j] == 'R') {
                if (board->getTileColor(i, j))
                    redCtr += 100;
                else
                    redCtr += 50;
            }
        }
    }

    int heuristicValue = greenCtr - redCtr;

    char opponentPlayer;

    if (currentPlayer == 'G')
        opponentPlayer = 'R';
    else
        opponentPlayer = 'G';

    vector<vector<int>> move = nextMove(previousState, state, opponentPlayer);

    int x = move[1][0];
    int y = move[1][1];

    int defensiveValue = board->getTokenStreak(x, y, currentPlayer, state);
    int offensiveValue = board->getTokenStreak(x, y, opponentPlayer, state);

    if (currentPlayer == 'G') {
        heuristicValue -= (5 * defensiveValue);
        heuristicValue += (10 * offensiveValue);
    }
    else {
        heuristicValue += (5 * defensiveValue);
        heuristicValue -= (10 * offensiveValue);
    }

    return heuristicValue;
}

int AIPlayer::minimax(vector<vector<char>> previousState, vector<vector<char>> state, char currentPlayer, int level, int depth, bool min_level, int heuristicIndex, QTreeWidgetItem *root){
    if (root == nullptr) {
        root = new QTreeWidgetItem();
        tree->clear();
        tree->addTopLevelItem(root);
    }

    // if level is zero, then return the heuristic
    // value given by the heuristic function
    if (level == 1) {
        int tempValue;

        switch(heuristicIndex) {
        case 0:
            tempValue = naiveHeuristic(state);
            break;
        case 1:
            tempValue = countingHeuristic(state);
            break;
        case 2:
            tempValue = informedHeuristic(previousState, state, currentPlayer);
            break;
        }

        root->setText(0, QString::number(tempValue));
        return tempValue;
    }

    //if level is not zero, recursively call
    // minimax() on all frontier states
    else {
        vector<vector<vector<char> > > current_level_states = getFrontierStates(state, currentPlayer);

        // call getHeuristic on first element of frontier states
        // to perform the initial comparison
        int return_heuristic = min_level ? -999999 : 999999;
        QTreeWidgetItem *leaf;

        // compare each state and determine greatest or
        // smallest heuristic depending on whether level
        // is min or max
        for (auto currentState : current_level_states) {
            leaf = new QTreeWidgetItem(root);
            int current_state_heuristic = minimax(state, currentState, currentPlayer, level-1, depth, !min_level, heuristicIndex, leaf);

            if (level == depth)
                frontierValues.push_back(current_state_heuristic);

            if (min_level){
                return_heuristic = return_heuristic > current_state_heuristic ? return_heuristic : current_state_heuristic;
            }
            else {
                return_heuristic = return_heuristic < current_state_heuristic ? return_heuristic : current_state_heuristic;
            }
        }

        root->setText(0, QString::number(return_heuristic));

        return return_heuristic;
    }
}

int AIPlayer::alphabeta(vector<vector<char>> previousState, vector<vector<char>> state, char currentPlayer, int level, int depth, int alpha, int beta, bool min_level, int heuristicIndex, QTreeWidgetItem *root){
    if (root == nullptr) {
        root = new QTreeWidgetItem();
        tree->clear();
        tree->addTopLevelItem(root);
    }

    if (level == 1) {
        int tempValue;

        switch(heuristicIndex) {
        case 0:
            tempValue = naiveHeuristic(state);
            break;
        case 1:
            tempValue = countingHeuristic(state);
            break;
        case 2:
            tempValue = informedHeuristic(previousState, state, currentPlayer);
            break;
        }

        root->setText(0, QString::number(tempValue));
        return tempValue;
    }
    else {
        QTreeWidgetItem *leaf;
        vector<vector<vector<char>>> current_level_states = getFrontierStates(state, currentPlayer);

        if (min_level) {
            int return_heuristic = 999999;

            for (auto currentState: current_level_states) {
                leaf = new QTreeWidgetItem(root);
                int tempHeuristic = alphabeta(state, currentState, currentPlayer, level - 1, depth, alpha, beta, !min_level, heuristicIndex, leaf);
                return_heuristic = min(return_heuristic, tempHeuristic);
                beta = min(beta, return_heuristic);

                if (beta <= alpha)
                    break;
            }

            root->setText(0, QString::number(return_heuristic));

            return return_heuristic;
        }
        else {
            int return_heuristic = -999999;

            for (auto currentState: current_level_states) {
                leaf = new QTreeWidgetItem(root);
                int tempHeuristic = alphabeta(state, currentState, currentPlayer, level - 1, depth, alpha, beta, !min_level, heuristicIndex, leaf);
                return_heuristic = max(return_heuristic, tempHeuristic);
                alpha = max(alpha, return_heuristic);

                if (beta <= alpha)
                    break;
            }

            root->setText(0, QString::number(return_heuristic));

            return return_heuristic;
        }
    }
}

vector<vector<int>> AIPlayer::nextMove(vector<vector<char>> state_original, vector<vector<char>> state_new, char opponentPlayer){
    vector<int> originPos;
    vector<int> destPos;
    vector<vector<int>> nextMove;

    const int WIDTH = board->getWidth();
    const int HEIGHT = board->getHeight();

    bool searchOver = false;

    // Iterate through the two states in order to find the token that moved
    for (int x = 0; x < WIDTH; ++x)
    {
        for (int y = 0; y < HEIGHT; ++y)
        {
            // If at (x,y) on both state doesn't match, that means a token moved from/to there
            if (state_original[x][y] != state_new[x][y])
            {
				// If the token at (x,y) is not the ai token in either states, continue to search (For now, R = AI token, G = Player Token)
                if (state_original[x][y] == opponentPlayer || state_new[x][y] == opponentPlayer)
					continue;
				
                // If on the original state we get a 'X' at (x,y), the token will move to that (x,y)
                if (state_original[x][y] == 'X')
                {
                    destPos.push_back(x);
                    destPos.push_back(y);
                }

                // If on the new state we get a 'X' at (x,y), the token was originally at that (x,y)
                if (state_new[x][y] == 'X')
                {
                    originPos.push_back(x);
                    originPos.push_back(y);
                }

                // If the search has found both coordinates, stop all loops
                if (originPos.size() == 2 && destPos.size() == 2)
                {
                    searchOver = true;
                    nextMove.push_back(originPos);
                    nextMove.push_back(destPos);
                    break;
                }
            }
        }

        // Stop the loop, search is over
        if (searchOver)
            break;
    }

    return nextMove;
}

vector<vector<int> > AIPlayer::getNextMoveFromAI(int level, char currentPlayer, bool isMiniMax, int heuristicIndex) {
    // vector of frontier states of current state of the game board
    vector<vector<vector<char> > > frontier_states;

    int best_heuristic, best_heuristic_index = 0;

    frontier_states = getFrontierStates(board->getMatrix(), currentPlayer);

    char opponentPlayer;

    if (currentPlayer == 'G')
        opponentPlayer = 'R';
    else
        opponentPlayer = 'G';

    if (isMiniMax) {
        if (currentPlayer == 'R')
            best_heuristic = minimax(board->getMatrix(), board->getMatrix(), currentPlayer, level, level, false, heuristicIndex, nullptr);
        else
            best_heuristic = minimax(board->getMatrix(), board->getMatrix(), currentPlayer, level, level, true, heuristicIndex, nullptr);
    }
    else {
        if (currentPlayer == 'R')
            best_heuristic = alphabeta(board->getMatrix(), board->getMatrix(), currentPlayer, level, level, -999999, 999999, true, heuristicIndex, nullptr);
        else
            best_heuristic = alphabeta(board->getMatrix(), board->getMatrix(), currentPlayer, level, level, -999999, 999999, false, heuristicIndex, nullptr);
    }

    if (isMiniMax) {
        for (unsigned int i = 0; i < frontierValues.size(); i++) {
            if (best_heuristic == frontierValues[i]) {
                best_heuristic_index = i;
            }
        }
    }
    else {
        for (int i = 0; i < tree->topLevelItem(0)->childCount(); i++) {
            if (best_heuristic == tree->topLevelItem(0)->child(i)->text(0).toInt()) {
                best_heuristic_index = i;
                break;
            }
        }
    }

    frontierValues.clear();

    return nextMove(board->getMatrix(), frontier_states[best_heuristic_index], opponentPlayer);
}

void AIPlayer::setTree(QTreeWidget* uiTree) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, tree->topLevelItem(0)->text(0));

    QTreeWidgetItem* currentItem = item;

    for (int i = 0; i < tree->topLevelItem(0)->childCount(); i++) {
        QTreeWidgetItem* nextItem = new QTreeWidgetItem(currentItem);
        nextItem->setText(0, tree->topLevelItem(0)->child(i)->text(0));
        currentItem = nextItem;

        for (int j = 0; j < tree->topLevelItem(0)->child(i)->childCount(); j++) {
            QTreeWidgetItem* nextItem2 = new QTreeWidgetItem(currentItem);
            nextItem2->setText(0, tree->topLevelItem(0)->child(i)->child(j)->text(0));
            currentItem = nextItem2;

            for (int k = 0; k < tree->topLevelItem(0)->child(i)->child(j)->childCount(); k++) {
                QTreeWidgetItem* nextItem3 = new QTreeWidgetItem(currentItem);
                nextItem3->setText(0, tree->topLevelItem(0)->child(i)->child(j)->child(k)->text(0));
            }

            currentItem = nextItem;
        }

        currentItem = item;
    }

    uiTree->addTopLevelItem(item);
}
