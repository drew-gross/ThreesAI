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

UCTSearchAI::UCTSearchAI(shared_ptr<AboutToMoveBoard const> board, unique_ptr<BoardOutput> output, unsigned int numPlays) : ThreesAIBase(board, move(output)), numPlays(numPlays) {}

void UCTSearchAI::receiveState(Direction d, AboutToMoveBoard const & newState) {};
void UCTSearchAI::prepareDirection() {};

Direction UCTSearchAI::getDirection() const {
    map<Direction, float> means;
    map<Direction, float> plays;
    unsigned int numPlays = 0;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            plays[d] = 1;
            AboutToMoveBoard boardCopy(AboutToMoveBoard::DifferentFuture(0), *this->currentState());
            AboutToMoveBoard movedCopy = boardCopy.moveWithAdd(d);
            means[d] = movedCopy.runRandomSimulation(numPlays);
            numPlays++;
        }
    }
    while (numPlays < this->numPlays) {
        numPlays++;
        Direction currentBest = max_element(means.begin(), means.end(), [numPlays, &plays](pair<Direction, float> l, pair<Direction, float> r){
            float leftBound = l.second/plays[l.first] + sqrt(2*log(numPlays)/plays[l.first]);
            float rightBound = r.second/plays[r.first] + sqrt(2*log(numPlays)/plays[r.first]);
            return leftBound < rightBound;
        })->first;
        AboutToMoveBoard boardCopy(AboutToMoveBoard::DifferentFuture(numPlays), *this->currentState());
        AboutToMoveBoard movedCopy = boardCopy.moveWithAdd(currentBest);
        BoardScore nextScore = movedCopy.runRandomSimulation(numPlays);
        plays[currentBest]++;
        means[currentBest] += (nextScore - means[currentBest])/plays[currentBest];
    }
    Direction best = max_element(means.begin(), means.end(), [](pair<Direction, float> l, pair<Direction, float> r) {
        return l.second < r.second;
    })->first;
    return best;
}