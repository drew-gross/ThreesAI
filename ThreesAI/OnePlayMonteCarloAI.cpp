//
//  ZeroDepthMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "OnePlayMonteCarloAI.h"

#include "Logging.h"

using namespace std;

OnePlayMonteCarloAI::OnePlayMonteCarloAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

void OnePlayMonteCarloAI::receiveState(Direction d, BoardState const & newState){}
void OnePlayMonteCarloAI::prepareDirection(){}

Direction OnePlayMonteCarloAI::getDirection() const {
    unsigned long bestScore = 0;
    Direction bestDirection = Direction::LEFT;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            BoardState moved(BoardState::Move(d), *this->currentState());
            BoardState::Score score = moved.runRandomSimulation(1);
            if (score > bestScore) {
                bestDirection = d;
                bestScore = score;
            }
        }
    }
    return bestDirection;
}