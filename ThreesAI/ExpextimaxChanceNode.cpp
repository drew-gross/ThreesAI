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

ExpectimaxChanceNode::ExpectimaxChanceNode(std::shared_ptr<BoardState const> board, Direction d, unsigned int depth) : ExpectimaxNode<AddedTileInfo>(board, depth), directionMovedToGetHere(d){}

shared_ptr<const ExpectimaxNodeBase> ExpectimaxChanceNode::child(AddedTileInfo const& t) const {
    auto result = this->children.find(t);
    if (result == this->children.end()) {
        this->outputDot();
        debug();
        list<weak_ptr<ExpectimaxNodeBase>> l;
        ((ExpectimaxChanceNode*)this)->fillInChildren(l);
    }
    return result->second;
}

float ExpectimaxChanceNode::value(std::function<float(BoardState const&)> heuristic) const {
    if (!this->childrenAreFilledIn()) {
        return this->board->score();
    }
    float value = accumulate(this->children.begin(), this->children.end(), 0.0f, [this, &heuristic](float acc, pair<AddedTileInfo, shared_ptr<const ExpectimaxNodeBase>> next){
        auto childProbabilityPair = this->childrenProbabilities.find(next.first);
        float childScore = next.second->value(heuristic);
        float childProbability = childProbabilityPair->second;
        return acc + childProbability * childScore;
    });
    return value;
}

void ExpectimaxChanceNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList) {
    deque<pair<Tile, float>> possibleNextTiles;
    if (this->board->hasNoHint) {
        possibleNextTiles = this->board->possibleNextTiles();
    } else {
        possibleNextTiles = this->board->getHint().possibleTiles();
    }
    EnabledIndices possibleNextLocations = this->board->validIndicesForNewTile(this->directionMovedToGetHere);
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex i : allIndices) {
            if (possibleNextLocations.isEnabled(i)) {
                std::shared_ptr<BoardState const> childBoard = make_shared<BoardState const>(BoardState::AddSpecificTile(this->directionMovedToGetHere, i, nextTile.first), *this->board, true);
                shared_ptr<ExpectimaxMoveNode> child = make_shared<ExpectimaxMoveNode>(childBoard, this->depth+1);
                AddedTileInfo childIndex(nextTile.first, i);
                this->childrenProbabilities.insert({childIndex, nextTile.second*locationProbability});
                this->children.insert({childIndex, child});
                
                unfilledList.push_back(child);
            }
        }
    }
    debug(this->children.empty());
}

void ExpectimaxChanceNode::pruneUnreachableChildren() {
    debug(!this->childrenAreFilledIn());
    debug(this->board->hasNoHint);
    float lostProbability = 0;
    Hint currentHint = this->board->getHint();
    for (auto it = this->children.cbegin(); it != this->children.cend();) {
        if (!currentHint.contains(it->first.newTileValue)) {
            lostProbability += this->childrenProbabilities[it->first];
            this->childrenProbabilities.erase(it->first);
            this->children.erase(it++);
        } else {
            ++it;
        }
    }
    debug(this->children.empty()); //Probably means stack tracker was wrong due to attaching AI to the middle of a game.
    if (this->children.empty()) {
        MYLOG(*this->board)
        std::list<std::weak_ptr<ExpectimaxNodeBase>> fakeUnfilledList;
        if (!this->childrenAreFilledIn()) {
            this->fillInChildren(fakeUnfilledList);
        }
        for (auto it = this->children.cbegin(); it != this->children.cend();) {
            if (!this->board->getHint().contains(it->first.newTileValue)) {
                lostProbability += this->childrenProbabilities[it->first];
                this->childrenProbabilities.erase(it->first);
                this->children.erase(it++);
            } else {
                ++it;
            }
        }
    }
    for (auto&& childProbability : this->childrenProbabilities) {
        childProbability.second /= (1-lostProbability);
    }
}

void ExpectimaxChanceNode::outputDotEdges(std::ostream& os, float p) const {
    for (auto&& child : this->children) {
        os << "\t" << long(this) << " -> " << long(child.second.get()) << " [label=\"" << child.first << "\"]" << endl;
    }
    os << "\t" << long(this) << " [label=\"";
    os << *this->board << "\"";
    if (this->board->isGameOver()) {
        os << ",style=filled,color=red";
    }
    os << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(os, this->childrenProbabilities.find(child.first)->second);
    }
}