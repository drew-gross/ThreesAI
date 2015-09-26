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

OnePlayMonteCarloAI::OnePlayMonteCarloAI(BoardState board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

void OnePlayMonteCarloAI::receiveState(Direction d, BoardState const & newState){}
void OnePlayMonteCarloAI::prepareDirection(){}

Direction OnePlayMonteCarloAI::getDirection() const {
    int bestScore = 0;
    Direction bestDirection = LEFT;
    vector<Direction> validMoves = this->currentState().validMoves();
    for (Direction d : validMoves) {
        BoardState copyToExplore = this->currentState().move(d).copyWithDifferentFuture();
        while (!copyToExplore.isGameOver()) {
            copyToExplore = copyToExplore.move(copyToExplore.randomValidMoveFromInternalGenerator());
        }
        if (copyToExplore.score() > bestScore) {
            bestDirection = d;
            bestScore = copyToExplore.score();
        }
    }
    return bestDirection;
}