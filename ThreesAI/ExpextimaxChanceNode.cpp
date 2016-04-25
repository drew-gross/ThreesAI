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

ExpectimaxChanceNode::ExpectimaxChanceNode(std::shared_ptr<AboutToAddTileBoard const> board, Direction d, unsigned int depth) :
ExpectimaxNode<AddedTileInfo>(depth),
directionMovedToGetHere(d),
board(board)
{}

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

float ExpectimaxChanceNode::value(std::function<float(AboutToMoveBoard const&)> heuristic) const {
    if (!this->childrenAreFilledIn()) {
        return this->board->score();
    }
    float value = accumulate(this->children.begin(), this->children.end(), 0.0f, [this, &heuristic](float acc, pair<AddedTileInfo, shared_ptr<const ExpectimaxNodeBase>> next){
        auto childProbabilityPair = this->childrenProbabilities.find(next.first);
        float childScore = dynamic_pointer_cast<const ExpectimaxMoveNode>(next.second)->value(heuristic);
        float childProbability = childProbabilityPair->second;
        return acc + childProbability * childScore;
    });
    return value;
}

void ExpectimaxChanceNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList) {
    deque<pair<Tile, float>> possibleNextTiles;
    //TODO: only use the tiles that the hint says might come if I ever start using expectimax again
    possibleNextTiles = this->board->possibleNextTiles();
    //TODO: Fix this if I ever start using Expectimax again
    debug();
    EnabledIndices possibleNextLocations({}); // = this->board->validIndicesForNewTile(this->directionMovedToGetHere);
    
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& nextTile : possibleNextTiles) {
        for (BoardIndex i : allIndices) {
            if (possibleNextLocations.isEnabled(i)) {
                std::shared_ptr<AboutToMoveBoard const> childBoard = make_shared<AboutToMoveBoard const>(this->board->addSpecificTile(AddedTileInfo(nextTile.first, i)));
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
    float lostProbability = 0;
    //TODO: reimplement this if I ever use Expectimax again
    debug();
    //Hint currentHint = this->board->getHint();
    for (auto it = this->children.cbegin(); it != this->children.cend();) {
        if (false){//(!currentHint.contains(it->first.newTileValue)) {
            //lostProbability += this->childrenProbabilities[it->first];
            //this->childrenProbabilities.erase(it->first);
            //this->children.erase(it++);
        } else {
            ++it;
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
    os << this->board << "\"";
    os << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(os, this->childrenProbabilities.find(child.first)->second);
    }
}