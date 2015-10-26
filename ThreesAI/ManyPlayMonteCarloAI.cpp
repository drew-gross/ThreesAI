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

ManyPlayMonteCarloAI::ManyPlayMonteCarloAI(BoardState board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

void ManyPlayMonteCarloAI::receiveState(Direction d, BoardState const & newState) {};
void ManyPlayMonteCarloAI::prepareDirection() {};

Direction ManyPlayMonteCarloAI::getDirection() const {
    unsigned long bestScore = 0;
    Direction bestDirection = Direction::LEFT;
    for (Direction d : this->currentState().validMoves()) {
        unsigned int playsRemaining = this->numPlays;
        unsigned long currentDirectionTotalScore = 0;
        while (playsRemaining--) {
            BoardState boardCopy = this->currentState().move(d).copyWithDifferentFuture();
            while (!boardCopy.isGameOver()) {
                Direction random = boardCopy.randomValidMoveFromInternalGenerator();
                boardCopy = boardCopy.move(random);
            }
            currentDirectionTotalScore += boardCopy.score();
        }
        if (currentDirectionTotalScore > bestScore) {
            bestDirection = d;
            bestScore = currentDirectionTotalScore;
        }
    }
    return bestDirection;
}