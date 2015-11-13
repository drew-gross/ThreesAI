//
//  UCTSearchAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 11/9/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "UCTSearchAI.hpp"

#include "Logging.h"
#include "Debug.h"

using namespace std;

UCTSearchAI::UCTSearchAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

void UCTSearchAI::receiveState(Direction d, BoardState const & newState) {};
void UCTSearchAI::prepareDirection() {};

Direction UCTSearchAI::getDirection() const {
    unsigned long bestScore = 0;
    Direction bestDirection = Direction::LEFT;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            unsigned int playsRemaining = this->numPlays;
            unsigned long currentDirectionTotalScore = 0;
            while (playsRemaining--) {
                BoardState boardCopy(BoardState::DifferentFuture(this->numPlays - playsRemaining), *this->currentState());
                boardCopy.takeTurnInPlace(d);
                while (!boardCopy.isGameOver()) {
                    boardCopy.takeTurnInPlace(boardCopy.randomValidMoveFromInternalGenerator());
                }
                currentDirectionTotalScore += boardCopy.score();
            }
            if (currentDirectionTotalScore > bestScore) {
                bestDirection = d;
                bestScore = currentDirectionTotalScore;
            }
        }
    }
    return bestDirection;
}