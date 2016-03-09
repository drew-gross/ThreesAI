//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "DescribeReasoningAI.h"

#include <stdio.h>
#include <iostream>

#include "Debug.h"
#include "Logging.h"

using namespace std;

DescribeReasoningAI::DescribeReasoningAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic) : ThreesAIBase(move(board), move(output)), heuristic(heuristic) {}

void DescribeReasoningAI::receiveState(Direction d, BoardState const & newState) {};
void DescribeReasoningAI::prepareDirection() {};

Direction DescribeReasoningAI::getDirection() const {
    vector<pair<Direction, pair<float, string>>> scoresForMoves;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            auto evaluationResult = this->heuristic.evaluate(BoardState(BoardState::MoveWithoutAdd(d), *this->currentState()));
            scoresForMoves.push_back({d, evaluationResult});
        }
    }
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, pair<float, string>> left, pair<Direction, pair<float, string>> right){
        return left.second.first < right.second.first;
    })->first;
    
    for (auto&& result : scoresForMoves) {
        cout << result.first << ": " << result.second.second << endl;
    }
    return d;
}