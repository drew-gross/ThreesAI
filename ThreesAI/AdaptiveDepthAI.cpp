//
//  AdaptiveDepthAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 1/16/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#include "AdaptiveDepthAI.hpp"

#include <stdio.h>
#include <iostream>
#include <stdint.h>

#include "Debug.h"
#include "Logging.h"

using namespace std;

AdaptiveDepthAI::AdaptiveDepthAI(BoardStateCPtr board, unique_ptr<BoardOutput> output, Heuristic heuristic, unsigned int numNodesForFurtherSearch) : ThreesAIBase(move(board), move(output)), heuristic(heuristic), numNodesForFurtherSearch(numNodesForFurtherSearch) {}

void AdaptiveDepthAI::receiveState(Direction d, BoardState const & newState) {};
void AdaptiveDepthAI::prepareDirection() {};

pair<unsigned int, Direction> nodesAndDirectionAtDepth(BoardState const& b, Heuristic h, unsigned int depth) {
    
    vector<pair<Direction, float>> scoresForMoves;
    unsigned int openNodeCount = 0;
    
    for (auto&& d : allDirections) {
        if (b.isMoveValid(d)) {
            BoardState movedBoard(BoardState::MoveWithoutAdd(d), b);
            auto searchResult = movedBoard.heuristicSearchIfMovedInDirection(d, depth, h);
            scoresForMoves.push_back({d, searchResult.first});
            openNodeCount += searchResult.second;
        }
    }
    debug(scoresForMoves.empty());
    Direction d = max_element(scoresForMoves.begin(), scoresForMoves.end(), [](pair<Direction, unsigned int> left, pair<Direction, unsigned int> right){
        return left.second < right.second;
    })->first;
    return {openNodeCount, d};
}

Direction AdaptiveDepthAI::getDirection() const {
    unsigned int depth = 0;
    while (true) {
        depth++;
        auto result = nodesAndDirectionAtDepth(*this->currentState(), this->heuristic, depth);
        unsigned int openNodes = result.first;
        cout << "Nodes viewed: " << openNodes << endl;
        if (openNodes >= this->numNodesForFurtherSearch || openNodes == 0) {
            return result.second;
        }
    }
}