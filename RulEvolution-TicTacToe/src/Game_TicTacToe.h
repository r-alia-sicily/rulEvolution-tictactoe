#ifndef GAME_TICTACTOE_H
#define GAME_TICTACTOE_H

#include "Board_TicTacToe.h"
#include "Player_TicTacToe.h"
#include "GameHistory.h"
#include "LearningModule.h"
#include <vector>


class Game {
public:
    Game(Player* pX, Player* pO);

    // Plays a single match
    char play(bool verbose = false);

    // Plays a match and updates learning afterwards
    char playAndLearn(GameHistory& history, bool verbose = false);

private:
    Board board;
    Player* playerX;
    Player* playerO;
    char currentTurn;
    GameHistory gameHistory;
};

#endif