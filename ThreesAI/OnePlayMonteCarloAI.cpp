//
//  ZeroDepthMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "OnePlayMonteCarloAI.h"

using namespace std;

OnePlayMonteCarloAI::OnePlayMonteCarloAI(BoardState board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

Direction OnePlayMonteCarloAI::getDirection() const {
    int bestScore = 0;
    Direction bestDirection = LEFT;
    for (Direction d : this->currentState().validMoves()) {
        BoardState copyToExplore = this->currentState().move(d);
        copyToExplore.move(d);
        while (!copyToExplore.isGameOver()) {
            default_random_engine g;
            copyToExplore.move(copyToExplore.randomValidMove(g));
        }
        if (copyToExplore.score() > bestScore) {
            bestDirection = d;
            bestScore = copyToExplore.score();
        }
    }
    return bestDirection;
}