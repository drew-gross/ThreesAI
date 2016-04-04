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

FixedDepthAI::FixedDepthAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, shared_ptr<Heuristic> heuristic, uint8_t depth) : ThreesAIBase(move(board), move(output)), heuristic(heuristic), depth(depth) {}

void FixedDepthAI::receiveState(Direction d, AboutToMoveBoard const & newState) {};
void FixedDepthAI::prepareDirection() {};

Direction FixedDepthAI::getDirection() const {
    vector<pair<Direction, float>> scoresForMoves;
    
    //unsigned int totalNodesViewed = 0;
    
    for (auto&& d : allDirections) {
        if (this->currentState()->isMoveValid(d)) {
            AboutToAddTileBoard movedBoard(MoveWithoutAdd(d), *this->currentState());
            debug();
            //TODO: If I use fixedDepthAI again, fix this by searching chilren
            //auto searchResult = movedBoard.heuristicSearchIfMovedInDirection(d, this->depth, this->heuristic);
            //totalNodesViewed += searchResult.openNodes;
            scoresForMoves.push_back({d, 0});//searchResult.value});
        }
    }
    
    debug(scoresForMoves.empty());
    auto d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    return d;
}