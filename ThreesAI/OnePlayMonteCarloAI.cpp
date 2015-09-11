//
//  ZeroDepthMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "OnePlayMonteCarloAI.h"

#include "SimulatedThreesBoard.h"

using namespace std;

OnePlayMonteCarloAI::OnePlayMonteCarloAI(shared_ptr<ThreesBoardBase> board) : ThreesAIBase(board) {}

Direction OnePlayMonteCarloAI::playTurn() {
    int bestScore = 0;
    Direction bestDirection = LEFT;
    for (Direction d : board->validMoves()) {
        SimulatedThreesBoard copyToExplore = board->simulatedCopy();
        copyToExplore.move(d);
        while (!copyToExplore.isGameOver()) {
            copyToExplore.move(copyToExplore.randomValidMove());
        }
        if (copyToExplore.score() > bestScore) {
            bestDirection = d;
        }
    }
    this->board->move(bestDirection);
    return bestDirection;
}