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
    map<Direction, float> means;
    map<Direction, float> plays;
    unsigned int numPlays = 0;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            plays[d] = 1;
            BoardState boardCopy(BoardState::DifferentFuture(0), *this->currentState());
            BoardState movedCopy(BoardState::Move(d), boardCopy);
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
        BoardState boardCopy(BoardState::DifferentFuture(numPlays), *this->currentState());
        BoardState movedCopy(BoardState::Move(currentBest), boardCopy);
        BoardState::Score nextScore = movedCopy.runRandomSimulation(numPlays);
        plays[currentBest]++;
        means[currentBest] += (nextScore - means[currentBest])/plays[currentBest];
    }/*
    for (auto&& d : means) {
        cout << d.first << ": score: " << d.second << " plays: " << plays[d.first] << endl;
    }*/
    Direction best = max_element(means.begin(), means.end(), [](pair<Direction, float> l, pair<Direction, float> r) {
        return l.second < r.second;
    })->first;
    //cout << best << endl;
    return best;
}