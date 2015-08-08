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

ExpectimaxChanceNode::ExpectimaxChanceNode(SimulatedThreesBoard const& board, Direction d, unsigned int depth) : ExpectimaxNode<ChanceNodeEdge>(board, depth), directionMovedToGetHere(d){
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
    vector<BoardIndex> possibleNextLocations = this->board.validIndicesForNewTile(this->directionMovedToGetHere);
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex boardIndex : possibleNextLocations) {
            SimulatedThreesBoard childBoard = this->board;
            childBoard.set(boardIndex, nextTile.first);
            shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(childBoard, this->depth+1);
            
            ChanceNodeEdge childIndex(nextTile.first, boardIndex);
            this->childrenProbabilities.insert({childIndex, nextTile.second*locationProbability});
            this->children.insert({childIndex, child});
            
            unfilledList.push_back(child);
        }
    }
}

void ExpectimaxChanceNode::pruneUnreachableChildren(deque<unsigned int> const & nextTileHint) {
    float lostProbability = 0;
    for (auto it = this->children.begin(); it != this->children.end();) {
        auto old = it;
        it++;
        if (find(nextTileHint.begin(), nextTileHint.end(), old->first.newTileValue) == nextTileHint.end()) {
            this->children.erase(old);
            lostProbability += this->childrenProbabilities[old->first];
            this->childrenProbabilities.erase(old->first);
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
    //TODO: get the hint
    pair<shared_ptr<const ThreesBoardBase>, deque<unsigned int>> info(shared_ptr<const ThreesBoardBase>(&this->board),{});
    cout << info << "\"";
    if (this->board.isGameOver()) {
        cout << ",style=filled,color=red";
    }
    cout << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(this->childrenProbabilities.find(child.first)->second);
    }
}