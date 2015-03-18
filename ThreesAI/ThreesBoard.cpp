//
//  ThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoard.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <exception>

#include "Logging.h"

using namespace std;

class InvalidTileAdditionException : public runtime_error {
public:
    InvalidTileAdditionException() : runtime_error("Attempting to add a tile where none can be added"){};
};

ThreesBoard::ThreesBoard() {
    array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    shuffle(initialTiles.begin(), initialTiles.end(), TileStack::randomGenerator);
    this->board = array<array<unsigned int, 4>, 4>();
    for (unsigned i = 0; i < initialTiles.size(); i++) {
        this->board[i/4][i%4] = initialTiles[i];
    }
}

void ThreesBoard::set(BoardIndex p, unsigned int t){
    this->board[p.second][p.first] = t;
}

unsigned int ThreesBoard::at(BoardIndex p) const {
    return this->board[p.second][p.first];
}

bool ThreesBoard::canMerge(BoardIndex target, BoardIndex other) const {
    if (this->at(other) == 0) {
        return false;
    }
    if (this->at(target) == this->at(other) && this->at(target) != 1 && this->at(target) != 2) {
        return true;
    }
    if ((this->at(target) == 1 and this->at(other) == 2) or (this->at(target) == 2 and this->at(other) == 1)) {
        return true;
    }
    if (this->at(target) == 0 and this->at(other) != 0) {
        return true;
    }
    return false;
}

bool ThreesBoard::tryMerge(BoardIndex target, BoardIndex other) {
    if (this->canMerge(target, other)) {
        this->set(target, this->at(target) + this->at(other));
        this->set(other, 0);
        return true;
    } else {
        return false;
    }
}

bool ThreesBoard::canMove(Direction d) const {
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 0}, {i, 1})) {
                    return true;
                }
                if (this->canMerge({i, 1}, {i, 2})) {
                    return true;
                }
                if (this->canMerge({i, 2}, {i, 3})) {
                    return true;
                }
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({i, 3}, {i, 2})) {
                    return true;
                }
                if (this->canMerge({i, 2}, {i, 1})) {
                    return true;
                }
                if (this->canMerge({i, 1}, {i, 0})) {
                    return true;
                }
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({0, i}, {1, i})) {
                    return true;
                }
                if (this->canMerge({1, i}, {2, i})) {
                    return true;
                }
                if (this->canMerge({2, i}, {3, i})) {
                    return true;
                }
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge({3, i}, {2, i})) {
                    return true;
                }
                if (this->canMerge({2, i}, {1, i})) {
                    return true;
                }
                if (this->canMerge({1, i}, {0, i})) {
                    return true;
                }
            }
            break;
            
        default:
            break;
    }
    return false;
}

bool ThreesBoard::moveWithoutAdd(Direction d) {
    bool successfulMerge = false;
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({i, 0}, {i, 1});
                successfulMerge |= this->tryMerge({i, 1}, {i, 2});
                successfulMerge |= this->tryMerge({i, 2}, {i, 3});
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({i, 3}, {i, 2});
                successfulMerge |= this->tryMerge({i, 2}, {i, 1});
                successfulMerge |= this->tryMerge({i, 1}, {i, 0});
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({0, i}, {1, i});
                successfulMerge |= this->tryMerge({1, i}, {2, i});
                successfulMerge |= this->tryMerge({2, i}, {3, i});
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge({3, i}, {2, i});
                successfulMerge |= this->tryMerge({2, i}, {1, i});
                successfulMerge |= this->tryMerge({1, i}, {0, i});
            }
            break;
        default:
            break;
    }
    return successfulMerge;
}

pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::move(Direction d) {
    if (this->moveWithoutAdd(d)) {
        return this->addTile(d);
    }
    throw InvalidMoveException();
}

unsigned int ThreesBoard::maxTile() const {
    unsigned int maxTile = 0;
    for (array<unsigned int, 4> row : this->board) {
        maxTile = max(maxTile, *max_element(row.begin(), row.end()));
    }
    return maxTile;
}

vector<tuple<float, ThreesBoard>> ThreesBoard::possibleNextBoardStates() const {
    vector<tuple<float, ThreesBoard>> result;
    float num_ones = this->tileStack.ones;
    float num_twos = this->tileStack.twos;
    float num_threes = this->tileStack.threes;
    float num_elems = this->tileStack.size();
    
    if (num_ones > 0) {
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = TileStack(num_ones - 1, num_twos, num_threes, 1);
        result.push_back({num_ones/num_elems, nextBoard});
    }
    
    if (num_twos > 0) {
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = TileStack(num_ones, num_twos - 1, num_threes, 2);
        result.push_back({num_twos/num_elems, nextBoard});
    }
    
    if (num_threes > 0) {
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = TileStack(num_ones, num_twos, num_threes - 1, 3);
        result.push_back({num_threes/num_elems, nextBoard});
    }
    
    return result;
}

vector<ThreesBoard::BoardIndex> ThreesBoard::validIndicesForNewTile(Direction movedDirection) const {
    array<BoardIndex, 4> indicies;
    switch (movedDirection) {
        case LEFT:
            indicies = {BoardIndex(3,0),BoardIndex(3,1),BoardIndex(3,2),BoardIndex(3,3)};
            break;
        case RIGHT:
            indicies = {BoardIndex(0,0),BoardIndex(0,1),BoardIndex(0,2),BoardIndex(0,3)};
            break;
        case UP:
            indicies = {BoardIndex(0,3),BoardIndex(1,3),BoardIndex(2,3),BoardIndex(3,3)};
            break;
        case DOWN:
            indicies = {BoardIndex(0,0),BoardIndex(1,0),BoardIndex(2,0),BoardIndex(3,0)};
            break;
        default:
            break;
    }
    auto endIterator = remove_if(indicies.begin(), indicies.end(), [this](ThreesBoard::BoardIndex tile) {
        return this->at(tile) != 0;
    });
    vector<ThreesBoard::BoardIndex> result(indicies.begin(), endIterator);
    return result;
}

pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::addTile(Direction d) {
    auto indices = this->validIndicesForNewTile(d);
    shuffle(indices.begin(), indices.end(), TileStack::randomGenerator);
    unsigned int nextTileValue = this->tileStack.getNextTile(this->maxTile());
    this->set(*indices.begin(), nextTileValue);
    return {nextTileValue, *indices.begin()};
}

unsigned int ThreesBoard::score() const {
    return accumulate(this->board.begin(), this->board.end(), 0, [](unsigned int acc1, array<unsigned int, 4> row){
        return accumulate(row.begin(), row.end(), acc1, [](unsigned int acc2, unsigned int tile){
            return acc2 + ThreesBoard::tileScore(tile);
        });
    });
}

unsigned int ThreesBoard::tileScore(unsigned int tileValue) {
    return unordered_map<unsigned int, unsigned int>({
        {0,0},
        {1,0},
        {2,0},
        {3,3},
        {6,9},
        {12,27},
        {24,81},
        {48,243},
        {96,729},
        {192,2187},
        {384,6561},
        {768,19683},
        {1536,59049},
        {3072,177147},
        {6144,531441}
    })[tileValue];
}

vector<Direction> ThreesBoard::validMoves() const {
    vector<Direction> result;
    if (this->canMove(DOWN)) {
        result.push_back(DOWN);
    }
    if (this->canMove(UP)) {
        result.push_back(UP);
    }
    if (this->canMove(LEFT)) {
        result.push_back(LEFT);
    }
    if (this->canMove(RIGHT)) {
        result.push_back(RIGHT);
    }
    return result;
}

bool ThreesBoard::isGameOver() const {
    return this->validMoves().empty();
}

deque<unsigned int> ThreesBoard::nextTileHint() const {
    return this->tileStack.nextTileHint(this->maxTile());
}

template < class T >
ostream& operator << (ostream& os, const deque<T>& v)
{
    os << "[";
    for (typename deque<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

ostream& operator<<(ostream &os, Direction d){
    switch (d) {
        case UP:
            os << "UP";
            break;
        case DOWN:
            os << "DOWN";
            break;
        case RIGHT:
            os << "RIGHT";
            break;
        case LEFT:
            os << "LEFT";
            break;
    }
    return os;
}

ostream& operator<<(ostream &os, ThreesBoard const& board){
    os << board.tileStack.nextTileHint(board.maxTile()) << endl;
    os << "---------------------  Current Score: " <<  board.score() << endl;
    os << "|" << setw(4) << board.at({0,0}) << "|" << setw(4) << board.at({1,0}) << "|" << setw(4) << board.at({2,0}) << "|" << setw(4) << board.at({3,0}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,1}) << "|" << setw(4) << board.at({1,1}) << "|" << setw(4) << board.at({2,1}) << "|" << setw(4) << board.at({3,1}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,2}) << "|" << setw(4) << board.at({1,2}) << "|" << setw(4) << board.at({2,2}) << "|" << setw(4) << board.at({3,2}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,3}) << "|" << setw(4) << board.at({1,3}) << "|" << setw(4) << board.at({2,3}) << "|" << setw(4) << board.at({3,3}) << "|" << endl;
    os << "---------------------" << endl;
    return os;
}