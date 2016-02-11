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

pair<unsigned int, vector<pair<Direction, float>>> openNodesAndScoresAtDepth(BoardState const& b, Heuristic h, unsigned int depth) {
    vector<pair<Direction, float>> scoresForMoves;
    unsigned int openNodeCount = 0;
    
    for (auto&& d : allDirections) {
        if (b.isMoveValid(d)) {
            BoardState movedBoard(BoardState::MoveWithoutAdd(d), b);
            auto searchResult = movedBoard.heuristicSearchIfMovedInDirection(d, depth, h);
            scoresForMoves.push_back({d, searchResult.value});
            openNodeCount += searchResult.openNodes;
        }
    }
    return {openNodeCount, scoresForMoves};
}

Direction AdaptiveDepthAI::getDirection() const {
    unsigned int depth = 0;
    while (true) {
        depth++;
        auto result = openNodesAndScoresAtDepth(*this->currentState(), this->heuristic, depth);
        sort(result.second.begin(), result.second.end(), [](pair<Direction, float> l, pair<Direction, float> r) {
            return l.second > r.second;
        });
        auto sortedMoves = result.second;
        if (sortedMoves.size() >= 2) {
            if (sortedMoves[1].second < 0.0001) {
                cout << "Only valid: " << sortedMoves[0].first << endl;
                return sortedMoves[0].first;
            }
        }
        unsigned int openNodes = result.first;
        if (openNodes >= this->numNodesForFurtherSearch || openNodes == 0) {
            for (auto&& pair : sortedMoves) {
                cout << pair.first << " scored " << pair.second << endl;
            }
            return result.second[0].first;
        }
    }
}