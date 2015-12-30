//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ZeroDepthAI.h"

#include <stdio.h>
#include <iostream>

#include "Debug.h"
#include "Logging.h"

using namespace std;

ZeroDepthAI::ZeroDepthAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic) : ThreesAIBase(move(board), move(output)), heuristic(heuristic) {}

void ZeroDepthAI::receiveState(Direction d, BoardState const & newState) {};
void ZeroDepthAI::prepareDirection() {};

Direction ZeroDepthAI::getDirection() const {
    vector<pair<Direction, float>> scoresForMoves;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            scoresForMoves.push_back({d, this->heuristic(BoardState(BoardState::MoveWithoutAdd(d), *this->currentState()))});
        }
    }
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    return d;
}