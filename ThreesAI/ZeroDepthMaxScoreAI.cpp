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

#include "SimulatedThreesBoard.h"

#include "Debug.h"
#include "Logging.h"

using namespace std;

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI(shared_ptr<ThreesBoardBase> board) : ThreesAIBase(board) {
    
}

Direction ZeroDepthMaxScoreAI::playTurn() {
    vector<pair<Direction, unsigned int>> scoresForMoves;
    SimulatedThreesBoard leftBoard(this->board->simulatedCopy());
    SimulatedThreesBoard rightBoard(this->board->simulatedCopy());
    SimulatedThreesBoard upBoard(this->board->simulatedCopy());
    SimulatedThreesBoard downBoard(this->board->simulatedCopy());
    
    if (leftBoard.moveWithoutAdd(LEFT)) {
        scoresForMoves.push_back({LEFT, leftBoard.score()});
    }
    if (rightBoard.moveWithoutAdd(RIGHT)) {
        scoresForMoves.push_back({RIGHT, rightBoard.score()});
    }
    if (upBoard.moveWithoutAdd(UP)) {
        scoresForMoves.push_back({UP, upBoard.score()});
    }
    if (downBoard.moveWithoutAdd(DOWN)) {
        scoresForMoves.push_back({DOWN, downBoard.score()});
    }
    debug(scoresForMoves.empty());
    Direction d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    
    this->board->move(d);
    return d;
}