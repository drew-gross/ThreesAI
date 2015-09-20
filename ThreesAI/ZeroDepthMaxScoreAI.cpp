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

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI(BoardState board, unique_ptr<BoardOutput> output) : ThreesAIBase(move(board), move(output)) {}

Direction ZeroDepthMaxScoreAI::playTurn() {
    vector<pair<Direction, unsigned int>> scoresForMoves;
    
    scoresForMoves.push_back({LEFT, this->currentState().moveWithoutAdd(LEFT).score()});
    scoresForMoves.push_back({RIGHT, this->currentState().moveWithoutAdd(RIGHT).score()});
    scoresForMoves.push_back({UP, this->currentState().moveWithoutAdd(UP).score()});
    scoresForMoves.push_back({DOWN, this->currentState().moveWithoutAdd(DOWN).score()});
    debug(scoresForMoves.empty());
    return max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
}