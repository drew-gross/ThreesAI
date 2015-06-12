//
//  ZeroDepthMaxScoreAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ZeroDepthMaxScoreAI.h"
#include "SimulatedThreesBoard.h"

#include "Debug.h"

using namespace std;

ZeroDepthMaxScoreAI::ZeroDepthMaxScoreAI(unique_ptr<ThreesBoardBase>&& board) : ThreesAIBase(move(board)) {
    
}

Direction ZeroDepthMaxScoreAI::playTurn() {
    vector<pair<Direction, unsigned int>> scoresForMoves;
    
    try {
        SimulatedThreesBoard leftBoard(this->board->simulatedCopy());
        leftBoard.move(LEFT);
        scoresForMoves.push_back({LEFT, leftBoard.score()});
    } catch (InvalidMoveException) {
        //Carry on trying the others
    }
    
    try {
        SimulatedThreesBoard rightBoard(this->board->simulatedCopy());
        rightBoard.move(RIGHT);
        scoresForMoves.push_back({RIGHT, rightBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    try {
        SimulatedThreesBoard upBoard(this->board->simulatedCopy());
        upBoard.move(UP);
        scoresForMoves.push_back({UP, upBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    try {
        SimulatedThreesBoard downBoard(this->board->simulatedCopy());
        downBoard.move(DOWN);
        scoresForMoves.push_back({DOWN, downBoard.score()});
    } catch (InvalidMoveException &e) {
        //Carry on with the others
    }
    
    debug(scoresForMoves.empty());
    Direction d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    
    this->board->move(d);
    return d;
}