//
//  ManyPlayMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ManyPlayMonteCarloAI.h"

#include "Logging.h"

using namespace std;

ManyPlayMonteCarloAI::ManyPlayMonteCarloAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

void ManyPlayMonteCarloAI::receiveState(Direction d, BoardState const & newState) {};
void ManyPlayMonteCarloAI::prepareDirection() {};

Direction ManyPlayMonteCarloAI::getDirection() const {
    unsigned long bestScore = 0;
    Direction bestDirection = Direction::LEFT;
    for (Direction d : this->currentState()->validMoves()) {
        unsigned int playsRemaining = this->numPlays;
        unsigned long currentDirectionTotalScore = 0;
        while (playsRemaining--) {
            shared_ptr<BoardState> boardCopy = make_shared<BoardState>(BoardState::CopyType::WITH_DIFFERENT_FUTURE, *this->currentState());
            boardCopy = make_shared<BoardState>(BoardState::Move(d), *boardCopy);
            while (!boardCopy->isGameOver()) {
                Direction random = boardCopy->randomValidMoveFromInternalGenerator();
                boardCopy = make_shared<BoardState>(BoardState::Move(random), *boardCopy);
            }
            currentDirectionTotalScore += boardCopy->score();
        }
        if (currentDirectionTotalScore > bestScore) {
            bestDirection = d;
            bestScore = currentDirectionTotalScore;
        }
    }
    return bestDirection;
}