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

ExpectimaxMoveNode::ExpectimaxMoveNode(std::shared_ptr<BoardState const> board, unsigned int depth): ExpectimaxNode<Direction>(board, depth) {
    debug(!this->board->hasNoHint && this->board->getHint().contains(Tile::TILE_2) && this->board->hiddenState.twosInStack == 0);
}

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
            bool test = false;
            if (test) {
                MYLOG((!this->board->hasNoHint && this->board->getHint().contains(Tile::TILE_2) && this->board->hiddenState.twosInStack == 0));
                MYLOG(*childBoard);
                make_shared<BoardState const>(BoardState::MoveWithoutAdd(d), *this->board);
            }
            this->children.insert({d, child});
            unfilledList.push_back(weak_ptr<ExpectimaxChanceNode>(child));
        }
    }
}

void ExpectimaxMoveNode::pruneUnreachableChildren() {
    for (auto&& child : this->children) {
        child.second->pruneUnreachableChildren();
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

void ExpectimaxMoveNode::outputDotEdges(std::ostream& os, float p) const {
    for (auto&& child : this->children) {
        os << "\t" << long(this) << " -> " << long(child.second.get()) << " [label=\"" << child.first << "\"]" << std::endl;
    }
    os << "\t" << long(this) << " [label=\"";
    os << "P=" << p << endl;
    os << *this->board << "\"";
    if (this->board->isGameOver()) {
        os << ",style=filled,color=red";
    }
    os << "]" << endl;
    for (auto&& child : this->children) {
        child.second->outputDotEdges(os, NAN);
    }
}