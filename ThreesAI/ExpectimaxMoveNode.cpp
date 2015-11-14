//
//  ExpectimaxMoveNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxMoveNode.h"

#include <memory>
#include <iomanip>

#include "ExpextimaxChanceNode.h"

#include "Debug.h"
#include "Logging.h"
#include "IMProc.h"

using namespace std;

ExpectimaxMoveNode::ExpectimaxMoveNode(std::shared_ptr<BoardState const> board, unsigned int depth): ExpectimaxNode<Direction>(board, depth) {}

pair<Direction, shared_ptr<const ExpectimaxNodeBase>> ExpectimaxMoveNode::maxChild(std::function<float(BoardState const&)> heuristic) const {
    return *max_element(this->children.begin(), this->children.end(), [&heuristic](pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> left, pair<Direction, std::shared_ptr<const ExpectimaxNodeBase>> right){
        float leftValue = left.second->value(heuristic);
        float rightValue = right.second->value(heuristic);
        return leftValue < rightValue;
    });
}

void ExpectimaxMoveNode::fillInChildren(list<weak_ptr<ExpectimaxNodeBase>> & unfilledList){
    debug(this->childrenAreFilledIn());
    for (Direction d : allDirections) {
        if (this->board->isMoveValid(d)) {
            std::shared_ptr<BoardState const> childBoard = make_shared<BoardState const>(BoardState::MoveWithoutAdd(d), *this->board);
            shared_ptr<ExpectimaxChanceNode> child = make_shared<ExpectimaxChanceNode>(childBoard, d, this->depth+1);
            this->children.insert({d, child});
            unfilledList.push_back(weak_ptr<ExpectimaxChanceNode>(child));
        }
    }
}

void ExpectimaxMoveNode::pruneUnreachableChildren(Hint const& nextTileHint, std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) {
    for (auto&& child : this->children) {
        child.second->pruneUnreachableChildren(nextTileHint, unfilledList);
    }
}

float ExpectimaxMoveNode::value(std::function<float(BoardState const&)> heuristic) const {
    if (this->board->isGameOver()) {
        return 0;
    }
    if (this->childrenAreFilledIn()) {
        return this->maxChild(heuristic).second->value(heuristic);
    }
    return heuristic(*this->board);
}

std::shared_ptr<const ExpectimaxNodeBase> ExpectimaxMoveNode::child(Direction const& d) const {
    auto result = this->children.find(d);
    debug(result == this->children.end());
    return result->second;
}

void ExpectimaxMoveNode::outputDotEdges(float p, std::function<float(BoardState const&)> heuristic) const {
    for (auto&& child : this->children) {
        cout << "\t" << long(this) << " -> " << long(child.second.get()) << " [label=\"" << child.first << "\"]" << std::endl;
    }
    cout << "\t" << long(this) << " [label=\"";
    cout << "Value=" << setprecision(7) << this->value(heuristic) << endl;
    cout << "P=" << p << endl;
    cout << this->board << "\"";
    if (this->board->isGameOver()) {
        cout << ",style=filled,color=red";
    }
    cout << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(NAN, heuristic);
    }
}