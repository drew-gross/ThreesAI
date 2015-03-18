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

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board, Direction d) : ExpectimaxNode<ChanceNodeEdge>(board), directionMovedToGetHere(d){
}

shared_ptr<const ExpectimaxNodeBase> ExpectimaxChanceNode::child(ChanceNodeEdge const& t) const {
    auto result = this->children.find(t);
    debug(result == this->children.end());
    return result->second;
}

unsigned int ExpectimaxChanceNode::value() const {
    float value = accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, pair<ChanceNodeEdge, shared_ptr<const ExpectimaxNodeBase>> next){
        ChanceNodeEdge edge = next.first;
        shared_ptr<const ExpectimaxNodeBase> node = next.second;
        auto childProbability = this->childrenProbabilities.find(edge);
        debug(childProbability == this->childrenProbabilities.end());
        return acc + childProbability->second * node->value();
    });
    return floor(value);
}

void ExpectimaxChanceNode::fillInChildren(list<shared_ptr<ExpectimaxNodeBase>> & unfilledList) {
    auto possibleNextTiles = this->board.tileStack.possibleNextTiles(this->board.maxTile());
    auto possibleNextLocations = this->board.validIndicesForNewTile(this->directionMovedToGetHere);
    vector<tuple<float, ThreesBoard, unsigned int>> possibleNextBoardStates = this->board.possibleNextBoardStates();
    
    float tileProbability = 1.0f/possibleNextLocations.size();
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (auto&& boardIndex : possibleNextLocations) {
            for (auto&& state : possibleNextBoardStates) {
                float stateProbability = tileProbability*locationProbability*get<0>(state);
                ThreesBoard nextBoard = get<1>(state);
                unsigned int upcomingTile = get<2>(state);
                
                ThreesBoard childBoard = nextBoard;
                childBoard.set(boardIndex, nextTile);
                shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(childBoard);
                
                ChanceNodeEdge childIndex(nextTile, boardIndex, upcomingTile);
                this->childrenProbabilities.insert({childIndex, stateProbability});
                this->children.insert({childIndex, child});
        
                unfilledList.push_back(child);
            }
        }
    }
}