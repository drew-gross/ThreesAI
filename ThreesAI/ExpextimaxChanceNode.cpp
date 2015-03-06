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

using namespace std;

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board, Direction d) : ExpectimaxNode<ChanceNodeEdge>(board), directionMovedToGetHere(d){
}

shared_ptr<ExpectimaxNodeBase> ExpectimaxChanceNode::child(ChanceNodeEdge t) {
    return this->children[t];
}

unsigned int ExpectimaxChanceNode::value() {
    float value = accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, pair<ChanceNodeEdge, shared_ptr<ExpectimaxNodeBase>> next){
        ChanceNodeEdge edge = next.first;
        shared_ptr<ExpectimaxNodeBase> node = next.second;
        return acc + this->childrenProbabilities[edge] * node->value();
    });
    return floor(value);
}

void ExpectimaxChanceNode::fillInChildren(list<shared_ptr<ExpectimaxNodeBase>> & unfilledList) {
    auto possibleNextTiles = this->board.tileStack.possibleUpcomingTiles(this->board.maxTile());
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
                
                shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(nextBoard);
                child->board.set(boardIndex, nextTile);
                ChanceNodeEdge childIndex(nextTile, boardIndex, upcomingTile);
                
                debug(child == nullptr);
                
                this->childrenProbabilities[childIndex] = stateProbability;
                this->children[childIndex] = child;
                
                unfilledList.push_back(this->child(childIndex));
            }
        }
    }
}