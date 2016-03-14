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

class DirectionAndScore {
public:
    DirectionAndScore(float score, Direction d) : d(d), score(score) {}
    float score;
    Direction d;
};

class NodeCountAndScores {
public:
    unsigned int nodeCount;
    vector<DirectionAndScore> scores;
};

NodeCountAndScores openNodesAndScoresAtDepth(BoardState const& b, Heuristic h, unsigned int depth) {
    vector<DirectionAndScore> scoresForMoves;
    unsigned int openNodeCount = 0;
    
    for (auto&& d : allDirections) {
        if (b.isMoveValid(d)) {
            BoardState movedBoard(BoardState::MoveWithoutAdd(d), b);
            auto searchResult = movedBoard.heuristicSearchIfMovedInDirection(d, depth, h);
            scoresForMoves.push_back(DirectionAndScore(searchResult.value, d));
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
        sort(result.scores.begin(), result.scores.end(), [](DirectionAndScore l, DirectionAndScore r) {
            return l.score > r.score;
        });
        unsigned int openNodes = result.nodeCount;
        if (openNodes >= this->numNodesForFurtherSearch || openNodes == 0) {
            return result.scores[0].d;
        }
    }
}