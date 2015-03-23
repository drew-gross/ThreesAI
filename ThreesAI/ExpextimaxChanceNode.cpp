//
//  ExpextimaxChanceNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpextimaxChanceNode.h"

#include "ExpectimaxMoveNode.h"

#include "Debug.h"
#include "Logging.h"

using namespace std;

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board, Direction d, unsigned int depth) : ExpectimaxNode<ChanceNodeEdge>(board, depth), directionMovedToGetHere(d){
}

shared_ptr<const ExpectimaxNodeBase> ExpectimaxChanceNode::child(ChanceNodeEdge const& t) const {
    auto result = this->children.find(t);
    debug(result == this->children.end());
    return result->second;
}

float ExpectimaxChanceNode::value() const {
    if (!this->childrenAreFilledIn()) {
        return this->board.score();
    }
    float value = accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, pair<ChanceNodeEdge, shared_ptr<const ExpectimaxNodeBase>> next){
        ChanceNodeEdge edge = next.first;
        shared_ptr<const ExpectimaxNodeBase> node = next.second;
        auto childProbabilityPair = this->childrenProbabilities.find(edge);
        debug(childProbabilityPair == this->childrenProbabilities.end());
        float childScore = node->value();
        float childProbability = childProbabilityPair->second;
        return acc + childProbability * childScore;
    });
    return value;
}

void ExpectimaxChanceNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList) {
    auto possibleNextTiles = this->board.tileStack.possibleNextTiles(this->board.maxTile());
    auto possibleNextLocations = this->board.validIndicesForNewTile(this->directionMovedToGetHere);
    vector<tuple<float, ThreesBoard>> possibleNextBoardStates = this->board.possibleNextBoardStates();
    
    float tileProbability = 1.0f/possibleNextLocations.size();
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (auto&& boardIndex : possibleNextLocations) {
            for (auto&& state : possibleNextBoardStates) {
                float stateProbability = tileProbability*locationProbability*get<0>(state);
                ThreesBoard nextBoard = get<1>(state);
                
                ThreesBoard childBoard = nextBoard;
                childBoard.set(boardIndex, nextTile);
                shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(childBoard, this->depth+1);
                
                ChanceNodeEdge childIndex(nextTile, boardIndex);
                this->childrenProbabilities.insert({childIndex, stateProbability});
                this->children.insert({childIndex, child});
        
                unfilledList.push_back(child);
            }
        }
    }
}

void ExpectimaxChanceNode::pruneUnreachableChildren(deque<unsigned int> const & nextTileHint) {
    if (nextTileHint.size() == 1) {
        unsigned int nextTile = nextTileHint.front();
        for (auto it = this->children.begin(); it != this->children.end();) {
            auto old = it;
            it++;
            if (old->first.newTileValue != nextTile) {
                this->children.erase(old);
            }
        }
    } else {
        debug();
    }
}