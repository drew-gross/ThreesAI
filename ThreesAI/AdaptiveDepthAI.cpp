//
//  AdaptiveDepthAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 1/16/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "AdaptiveDepthAI.hpp"

#include <stdio.h>
#include <iostream>
#include <stdint.h>

#include "Debug.h"
#include "Logging.h"

using namespace std;

AdaptiveDepthAI::AdaptiveDepthAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic, uint8_t depth) : ThreesAIBase(move(board), move(output)), heuristic(heuristic), depth(depth) {}

void AdaptiveDepthAI::receiveState(Direction d, BoardState const & newState) {};
void AdaptiveDepthAI::prepareDirection() {};

Direction AdaptiveDepthAI::getDirection() const {
    vector<pair<Direction, float>> scoresForMoves;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            BoardState movedBoard(BoardState::MoveWithoutAdd(d), *this->currentState());
            //scoresForMoves.push_back({d, getExpectedScoreIfMovedInDirection(movedBoard, d, this->depth, this->heuristic)});
        }
    }
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    return d;
}