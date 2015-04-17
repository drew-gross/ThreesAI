//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ZeroDepthMaxScoreAI.h"
#include "ThreesBoard.h"

using namespace std;

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI(unique_ptr<ThreesBoard> board) : ThreesAIBase(move(board)) {
    
}

Direction ZeroDepthMaxScoreAI::playTurn() {
    vector<pair<Direction, unsigned int>> scoresForMoves;
    
    try {
        ThreesBoard leftBoard(this->board->simulatedCopy());
        leftBoard.move(LEFT);
        scoresForMoves.push_back({LEFT, leftBoard.score()});
    } catch (InvalidMoveException) {
        //Carry on trying the others
    }
    
    try {
        ThreesBoard rightBoard(this->board->simulatedCopy());
        rightBoard.move(RIGHT);
        scoresForMoves.push_back({RIGHT, rightBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    try {
        ThreesBoard upBoard(this->board->simulatedCopy());
        upBoard.move(UP);
        scoresForMoves.push_back({UP, upBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    try {
        ThreesBoard downBoard(this->board->simulatedCopy());
        downBoard.move(DOWN);
        scoresForMoves.push_back({DOWN, downBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    Direction d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    
    this->board->move(d);
    return d;
}