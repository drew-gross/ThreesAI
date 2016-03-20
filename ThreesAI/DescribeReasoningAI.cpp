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

DescribeReasoningAI::DescribeReasoningAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, shared_ptr<Heuristic> heuristic) : ThreesAIBase(move(board), move(output)), heuristic(heuristic) {}

void DescribeReasoningAI::receiveState(Direction d, BoardState const & newState) {};
void DescribeReasoningAI::prepareDirection() {};

Direction DescribeReasoningAI::getDirection() const {
    vector<pair<Direction, EvalutationWithDescription>> scoresForMoves;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            auto evaluationResult = this->heuristic->evaluateWithDescription(BoardState(BoardState::MoveWithoutAdd(d), *this->currentState()));
            scoresForMoves.push_back({d, evaluationResult});
        }
    }
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, EvalutationWithDescription> left, pair<Direction, EvalutationWithDescription> right){
        return left.second.score < right.second.score;
    })->first;
    
    for (auto&& result : scoresForMoves) {
        cout << result.first << ": " << result.second.desciption << endl;
    }
    return d;
}