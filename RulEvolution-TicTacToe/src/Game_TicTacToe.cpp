#include "Game_TicTacToe.h"
#include "RulEvolutionPlayer_TicTacToe.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

extern LearningModule learner;

Game::Game(Player* pX, Player* pO)
    : board(), playerX(pX), playerO(pO), currentTurn('X') {
}

char Game::play(bool verbose) {
    board.reset();
    currentTurn = (std::rand() % 2 == 0) ? 'X' : 'O';
    gameHistory.clear();

    if (verbose) {
        std::cout << "--- Match ---" << std::endl;
        std::cout << "Random draw: Player " << currentTurn << " starts!" << std::endl;
    }

    int moveCount = 0;
    while (true) {
        if (verbose) {
            board.print();
            std::cout << "Move " << (moveCount + 1)
                << ": Player " << currentTurn
                << " is choosing..." << std::endl;
        }

        int move = -1;
        std::vector<RuleType> rulesUsed;

        // Se il player è di tipo RulEvolution, usa la versione che restituisce le regole usate
        if (currentTurn == 'X') {
            if (auto rp = dynamic_cast<RulEvolutionPlayer*>(playerX))
                move = rp->chooseMove(board, rulesUsed);
            else
                move = playerX->chooseMove(board);
        }
        else {
            if (auto rp = dynamic_cast<RulEvolutionPlayer*>(playerO))
                move = rp->chooseMove(board, rulesUsed);
            else
                move = playerO->chooseMove(board);
        }

        if (!board.place(move, currentTurn)) {
            std::cout << "Invalid move by " << currentTurn
                << " at cell " << move << std::endl;
            return (currentTurn == 'X') ? 'O' : 'X';
        }

        // Registra la mossa nel gameHistory
        gameHistory.addMove(move, rulesUsed);
        moveCount++;

        char winner = board.winner();

        if (winner != ' ') {
            if (verbose) {
                board.print();
                std::cout << "Winner: " << winner << std::endl;
            }
            return winner;
        }

        if (board.isFull()) {
            if (verbose) {
                board.print();
                std::cout << "It's a draw!" << std::endl;
            }
            return ' ';
        }

        currentTurn = (currentTurn == 'X') ? 'O' : 'X';
    }
}

char Game::playAndLearn(GameHistory& history, bool verbose) {
    char winner = play(verbose);

    bool rulevWon = false;
    bool rulevLost = false;
    bool isDraw = (winner == ' ');

    if (!isDraw) {
        if (dynamic_cast<RulEvolutionPlayer*>(playerX) && winner == 'X')
            rulevWon = true;
        else if (dynamic_cast<RulEvolutionPlayer*>(playerO) && winner == 'O')
            rulevWon = true;
        else if (dynamic_cast<RulEvolutionPlayer*>(playerX) && winner == 'O')
            rulevLost = true;
        else if (dynamic_cast<RulEvolutionPlayer*>(playerO) && winner == 'X')
            rulevLost = true;
    }

    if (isDraw) {
        std::cout << "[LEARN] Draw detected -> no weight change." << std::endl;
        history = gameHistory;
        return winner;
    }

    // Aggiorna il learning module
    if (rulevWon)
        learner.updateFromGame(gameHistory, true);
    else if (rulevLost)
        learner.updateFromGame(gameHistory, false);

    // Esporta la history al chiamante
    history = gameHistory;

    return winner;
}