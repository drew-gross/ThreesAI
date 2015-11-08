//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ZeroDepthMaxScoreAI.h"

#include <stdio.h>
#include <iostream>

#include "Debug.h"
#include "Logging.h"

using namespace std;

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output) : ThreesAIBase(move(board), move(output)) {}

Direction ZeroDepthMaxScoreAI::playTurn() {
    vector<pair<Direction, unsigned int>> scoresForMoves;
    
    for (auto&& d : directions) {
        scoresForMoves.push_back({d, BoardState(BoardState::MoveWithoutAdd(d), *this->currentState()).score()});
    }
    debug(scoresForMoves.empty());
    return max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
}