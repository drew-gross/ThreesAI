//
//  ManyPlayMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ManyPlayMonteCarloAI.h"

#include "Logging.h"
#include "Debug.h"

using namespace std;

ManyPlayMonteCarloAI::ManyPlayMonteCarloAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

void ManyPlayMonteCarloAI::receiveState(Direction d, BoardState const & newState) {};
void ManyPlayMonteCarloAI::prepareDirection() {};

Direction ManyPlayMonteCarloAI::getDirection() const {
    BoardState::Score bestScore = 0;
    auto validMoves = this->currentState()->validMoves();
    Direction bestDirection = Direction::LEFT;
    for (Direction d : allDirections) {
        if (validMoves.isEnabled(d)) {
            unsigned int playsRemaining = this->numPlays;
            unsigned long currentDirectionTotalScore = 0;
            while (playsRemaining--) {
                BoardState boardCopy(BoardState::DifferentFuture(this->numPlays - playsRemaining), *this->currentState());
                BoardState movedCopy(BoardState::Move(d), boardCopy);
                boardCopy.takeTurnInPlace(d);
                currentDirectionTotalScore += boardCopy.runRandomSimulation(this->numPlays - playsRemaining);
            }
            if (currentDirectionTotalScore > bestScore) {
                bestDirection = d;
                bestScore = currentDirectionTotalScore;
            }
        }
    }
    return bestDirection;
}