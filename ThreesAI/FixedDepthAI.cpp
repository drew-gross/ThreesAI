//
//  FixedDepthAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 1/10/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "FixedDepthAI.hpp"

#include <stdio.h>
#include <iostream>
#include <stdint.h>

#include "Debug.h"
#include "Logging.h"

using namespace std;

FixedDepthAI::FixedDepthAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic) : ThreesAIBase(move(board), move(output)), heuristic(heuristic) {}

void FixedDepthAI::receiveState(Direction d, BoardState const & newState) {};
void FixedDepthAI::prepareDirection() {};


float FixedDepthAI::getExpectedScoreIfMovedInDirection(BoardState const& justMovedBoard, Direction d, uint8_t depth) const {
    auto&& allAdditions = justMovedBoard.possibleAdditions(d);
    if (depth == 0) {
        float score = 0;
        for (auto&& info : allAdditions) {
            BoardState newBoard(BoardState::AddSpecificTile(d, info.i, info.t), justMovedBoard, true);
            score += this->heuristic(newBoard)/allAdditions.size();
        }
        return score;
    } else {
        debug();
        return 0;
    }
}


Direction FixedDepthAI::getDirection() const {
    vector<pair<Direction, float>> scoresForMoves;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            BoardState movedBoard(BoardState::MoveWithoutAdd(d), *this->currentState());
            scoresForMoves.push_back({d, this->getExpectedScoreIfMovedInDirection(movedBoard, d, 0)});
        }
    }
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    return d;
}