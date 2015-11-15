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
    map<Direction, float> scores;
    for (Direction d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            unsigned int playsRemaining = this->numPlays;
            unsigned long currentDirectionTotalScore = 0;
            while (playsRemaining--) {
                BoardState boardCopy(BoardState::DifferentFuture(this->numPlays - playsRemaining), *this->currentState());
                BoardState movedCopy(BoardState::MoveWithAdd(d), boardCopy);
                BoardState::Score newScore = movedCopy.runRandomSimulation(this->numPlays - playsRemaining);
                currentDirectionTotalScore += newScore;
            }
            scores[d] = float(currentDirectionTotalScore) / this->numPlays;
        } else {
            scores[d] = this->currentState()->score();
        }
    }
    for (auto&& d : scores) {
        cout << d.first << ": " << d.second << endl;
    }
    Direction bestDirection = max_element(scores.begin(), scores.end(), [](pair<Direction, float> l, pair<Direction, float> r){
        return l.second < r.second;
    })->first;
    float bestScore = scores[bestDirection];
    scores.erase(bestDirection);
    Direction secondBestDirection = max_element(scores.begin(), scores.end(), [](pair<Direction, float> l, pair<Direction, float> r){
        return l.second < r.second;
    })->first;
    float secondBestScore = scores[secondBestDirection];
    cout << bestDirection << " beats " << secondBestDirection << " by " << bestScore - secondBestScore << endl;
    return bestDirection;
}