//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ZeroDepthMaxScoreAI.h"
#include "ThreesBoard.h"

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI() : ThreesAIBase() {
    
}

void ZeroDepthMaxScoreAI::playTurn() {
    std::vector<std::pair<Direction, unsigned int>> scoresForMoves;
    
    ThreesBoard leftBoard(this->board);
    if (leftBoard.tryMove(LEFT)) {
        scoresForMoves.push_back({LEFT, leftBoard.score()});
    }
    
    ThreesBoard rightBoard(this->board);
    if (rightBoard.tryMove(RIGHT)) {
        scoresForMoves.push_back({RIGHT, rightBoard.score()});
    }
    
    ThreesBoard upBoard(this->board);
    if (upBoard.tryMove(UP)) {
        scoresForMoves.push_back({UP, upBoard.score()});
    }
    
    ThreesBoard downBoard(this->board);
    if (downBoard.tryMove(DOWN)) {
        scoresForMoves.push_back({DOWN, downBoard.score()});
    }
    
    Direction d = std::max_element(scoresForMoves.begin(), scoresForMoves.end(), [](std::pair<Direction, unsigned int> left, std::pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    
    this->board.tryMove(d);
}