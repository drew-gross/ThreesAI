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

ManyPlayMonteCarloAI::ManyPlayMonteCarloAI(shared_ptr<AboutToMoveBoard const> board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

Direction ManyPlayMonteCarloAI::getDirection() const {
    map<Direction, float> scores;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            unsigned int playsRemaining = this->numPlays;
            unsigned long currentDirectionTotalScore = 0;
            while (playsRemaining--) {
                AboutToMoveBoard boardCopy(AboutToMoveBoard::DifferentFuture(this->numPlays - playsRemaining), *this->currentState());
                AboutToMoveBoard movedCopy(AboutToMoveBoard::MoveWithAdd(d), boardCopy);
                BoardScore newScore = movedCopy.runRandomSimulation(this->numPlays - playsRemaining);
                currentDirectionTotalScore += newScore;
            }
            scores[d] = float(currentDirectionTotalScore) / this->numPlays;
        } else {
            scores[d] = this->currentState()->score();
        }
    }
    Direction bestDirection = max_element(scores.begin(), scores.end(), [](pair<Direction, float> l, pair<Direction, float> r){
        return l.second < r.second;
    })->first;
    return bestDirection;
}