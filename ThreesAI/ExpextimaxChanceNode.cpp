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
#include "IMProc.h"

using namespace std;

ExpectimaxChanceNode::ExpectimaxChanceNode(BoardState const& board, Direction d, unsigned int depth) : ExpectimaxNode<ChanceNodeEdge>(board, depth), directionMovedToGetHere(d){
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
    float value = accumulate(this->children.begin(), this->children.end(), 0.0f, [this](float acc, pair<ChanceNodeEdge, shared_ptr<const ExpectimaxNodeBase>> next){
        auto childProbabilityPair = this->childrenProbabilities.find(next.first);
        float childScore = next.second->value();
        float childProbability = childProbabilityPair->second;
        return acc + childProbability * childScore;
    });
    return value;
}

void ExpectimaxChanceNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList) {
    auto possibleNextTiles = this->board.possibleNextTiles();
    vector<BoardState::BoardIndex> possibleNextLocations = this->board.validIndicesForNewTile(this->directionMovedToGetHere);
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardState::BoardIndex boardIndex : possibleNextLocations) {
            BoardState childBoard = this->board.addSpecificTile(this->directionMovedToGetHere, boardIndex, nextTile.first);
            shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(childBoard, this->depth+1);
            
            ChanceNodeEdge childIndex(nextTile.first, boardIndex);
            this->childrenProbabilities.insert({childIndex, nextTile.second*locationProbability});
            this->children.insert({childIndex, child});
            
            unfilledList.push_back(child);
        }
    }
}

void ExpectimaxChanceNode::pruneUnreachableChildren(BoardState::Hint const & nextTileHint) {
    float lostProbability = 0;
    for (auto it = this->children.cbegin(); it != this->children.cend();) {
        if (find(nextTileHint.begin(), nextTileHint.end(), it->first.newTileValue) == nextTileHint.end()) {
            lostProbability += this->childrenProbabilities[it->first];
            this->childrenProbabilities.erase(it->first);
            this->children.erase(it++);
        } else {
            ++it;
        }
    }
    for (auto&& childProbability : this->childrenProbabilities) {
        childProbability.second /= (1-lostProbability);
    }
}

void ExpectimaxChanceNode::outputDotEdges(float p) const {
    for (auto&& child : this->children) {
        cout << "\t" << long(this) << " -> " << long(child.second.get()) << " [label=\"" << child.first << "\"]" << endl;
    }
    cout << "\t" << long(this) << " [label=\"";
    cout << "Value=" << this->value() << endl;
    cout << this->board << "\"";
    if (this->board.isGameOver()) {
        cout << ",style=filled,color=red";
    }
    cout << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(this->childrenProbabilities.find(child.first)->second);
    }
}