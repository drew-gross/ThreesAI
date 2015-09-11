//
//  ManyPlayMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ManyPlayMonteCarloAI.h"

#include "SimulatedThreesBoard.h"

using namespace std;

ManyPlayMonteCarloAI::ManyPlayMonteCarloAI(shared_ptr<ThreesBoardBase> board, unsigned int numPlays) : ThreesAIBase(board), numPlays(numPlays) {}

Direction ManyPlayMonteCarloAI::playTurn() {
    int bestScore = 0;
    Direction bestDirection = LEFT;
    for (Direction d : board->validMoves()) {
        unsigned int playsRemaining = this->numPlays;
        unsigned long currentDirectionTotalScore = 0;
        while (playsRemaining--) {
            SimulatedThreesBoard copyToExplore = board->simulatedCopy();
            copyToExplore.move(d);
            while (!copyToExplore.isGameOver()) {
                copyToExplore.move(copyToExplore.randomValidMove());
            }
            currentDirectionTotalScore += copyToExplore.score();
        }
        if (currentDirectionTotalScore > bestScore) {
            bestDirection = d;
            bestScore = currentDirectionTotalScore;
        }
    }
    this->board->move(bestDirection);
    return bestDirection;
}