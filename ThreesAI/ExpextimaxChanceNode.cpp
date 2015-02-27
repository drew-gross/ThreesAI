//
//  ExpextimaxChanceNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpextimaxChanceNode.h"

#include "ExpectimaxMoveNode.h"

using namespace std;

ExpectimaxChanceNode::ExpectimaxChanceNode(ThreesBoard const& board) : ExpectimaxNode(board){
    
}

bool ExpectimaxChanceNode::childrenAreFilledIn() {
    return this->children.empty();
}

pair<float, std::shared_ptr<ExpectimaxNode>> ExpectimaxChanceNode::child(tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int> t) {
    return this->children[t];
}

unsigned int ExpectimaxChanceNode::value() {
    float value = accumulate(this->children.begin(), this->children.end(), 0, [this](float acc, pair<tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int>, pair<float,shared_ptr<ExpectimaxNode>>> next){
        return acc + next.second.first*next.second.second->value();
    });
    return floor(value);
}

void ExpectimaxChanceNode::fillInChildren(list<shared_ptr<ExpectimaxNode>> unfilledList, Direction d) {
    auto possibleNextTiles = this->board.tileStack.possibleUpcomingTiles(this->board.maxTile());
    auto possibleNextLocations = this->board.validIndicesForNewTile(d);
    vector<tuple<float, ThreesBoard, unsigned int>> possibleNextBoardStates = this->board.possibleNextBoardStates();
    
    float tileProbability = 1.0f/possibleNextLocations.size();
    float locationProbability = 1.0f/possibleNextLocations.size();
    
    for (auto&& possibleTile : possibleNextTiles) {
        for (auto&& boardIndex : possibleNextLocations) {
            for (auto&& state : possibleNextBoardStates) {
                float stateProbability = tileProbability*locationProbability*get<0>(state);
                shared_ptr<ExpectimaxNode> childState = make_shared<ExpectimaxMoveNode>(get<1>(state));
                unsigned int upcomingTile = get<2>(state);
                tuple<unsigned int, ThreesBoard::BoardIndex, unsigned int> childIndex = {possibleTile, boardIndex, upcomingTile};
                this->children.insert({childIndex, {stateProbability, childState}});
                this->child(childIndex).second->board.set(boardIndex, possibleTile);
                unfilledList.push_back(this->child(childIndex).second);
            }
        }
    }
}