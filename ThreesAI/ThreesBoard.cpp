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

class InvalidTileAdditionException : public std::runtime_error {
public:
    InvalidTileAdditionException() : runtime_error("Attempting to add a tile where none can be added"){};
};

ThreesBoard::ThreesBoard() {
    std::array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    std::shuffle(initialTiles.begin(), initialTiles.end(), ThreesBoard::randomGenerator);
    this->board = std::array<std::array<unsigned int, 4>, 4>();
    for (unsigned i = 0; i < initialTiles.size(); i++) {
        this->board[i/4][i%4] = initialTiles[i];
    }
    this->getNextTile(); // Put the (true) first tile into this->upcomingTile, and ignore the first (garbage) returned tile
}

std::default_random_engine ThreesBoard::randomGenerator = std::default_random_engine();

unsigned int* ThreesBoard::at(unsigned int x, unsigned int y) {
    return &this->board[y][x];
}

unsigned int* ThreesBoard::at(BoardIndex p){
    return &this->board[p.second][p.first];
}

bool ThreesBoard::canMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY) {
    if (*this->at(otherX, otherY) == 0) {
        return false;
    }
    if (*this->at(targetX,targetY) == *this->at(otherX,otherY) && *this->at(targetX, targetY) != 1 && *this->at(otherX, otherY) != 2) {
        return true;
    }
    if ((*this->at(targetX,targetY) == 1 and *this->at(otherX,otherY) == 2) or (*this->at(targetX,targetY) == 2 and *this->at(otherX,otherY) == 1)) {
        return true;
    }
    if (*this->at(targetX, targetY) == 0 and *this->at(otherX, otherY) != 0) {
        return true;
    }
    return false;
}

bool ThreesBoard::tryMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY) {
    if (this->canMerge(targetX, targetY, otherX, otherY)) {
        *this->at(targetX, targetY) += *this->at(otherX, otherY);
        *this->at(otherX, otherY) = 0;
        return true;
    } else {
        return false;
    }
}

bool ThreesBoard::canMove(Direction d) {
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge(i, 0, i, 1)) {
                    return true;
                }
                if (this->canMerge(i, 1, i, 2)) {
                    return true;
                }
                if (this->canMerge(i, 2, i, 3)) {
                    return true;
                }
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge(i, 3, i, 2)) {
                    return true;
                }
                if (this->canMerge(i, 2, i, 1)) {
                    return true;
                }
                if (this->canMerge(i, 1, i, 0)) {
                    return true;
                }
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge(0,i,1,i)) {
                    return true;
                }
                if (this->canMerge(1,i,2,i)) {
                    return true;
                }
                if (this->canMerge(2,i,3,i)) {
                    return true;
                }
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                if (this->canMerge(3,i,2,i)) {
                    return true;
                }
                if (this->canMerge(2,i,1,i)) {
                    return true;
                }
                if (this->canMerge(1,i,0,i)) {
                    return true;
                }
            }
            break;
            
        default:
            break;
    }
    return false;
}

std::pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::move(Direction d) {
    bool successfulMerge = false;
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(i,0,i,1);
                successfulMerge |= this->tryMerge(i,1,i,2);
                successfulMerge |= this->tryMerge(i,2,i,3);
            }
            if (successfulMerge) {
                return this->addTile(DOWN);
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(i,3,i,2);
                successfulMerge |= this->tryMerge(i,2,i,1);
                successfulMerge |= this->tryMerge(i,1,i,0);
            }
            if (successfulMerge) {
                return this->addTile(UP);
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(0,i,1,i);
                successfulMerge |= this->tryMerge(1,i,2,i);
                successfulMerge |= this->tryMerge(2,i,3,i);
            }
            if (successfulMerge) {
                return this->addTile(RIGHT);
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(3,i,2,i);
                successfulMerge |= this->tryMerge(2,i,1,i);
                successfulMerge |= this->tryMerge(1,i,0,i);
            }
            if (successfulMerge) {
                return this->addTile(LEFT);
            }
            break;
        default:
            break;
    }
    throw InvalidMoveException();
}

void ThreesBoard::rebuildTileStackIfNecessary() {
    if (this->tileStack.empty()) {
        std::shuffle(baseStack.begin(), baseStack.end(), ThreesBoard::randomGenerator);
        for (unsigned int tile : this->baseStack) {
            this->tileStack.push_back(tile);
        }
    }
}

std::array<unsigned int, 12> ThreesBoard::baseStack = {1,1,1,1,2,2,2,2,3,3,3,3};

std::deque<unsigned int> ThreesBoard::possibleUpcomingTiles() {
    std::deque<unsigned int> inRangeTiles;
    if (this->upcomingTile <= 3) {
        inRangeTiles.push_back(this->upcomingTile);
    } else {
        //bonus tile
        
        //add possible values to the list
        if (this->upcomingTile / 4 >= 6) {
            inRangeTiles.push_back(this->upcomingTile/4);
        }
        if (this->upcomingTile / 2 >= 6) {
            inRangeTiles.push_back(this->upcomingTile/2);
        }
        inRangeTiles.push_back(this->upcomingTile);
        if (this->upcomingTile * 2 <= maxBonusTile()) {
            inRangeTiles.push_back(this->upcomingTile*2);
        }
        if (this->upcomingTile * 4 <= maxBonusTile()) {
            inRangeTiles.push_back(this->upcomingTile*2);
        }
        
        //trim the list down to size
        if (inRangeTiles.size() <= 3) {
            return inRangeTiles;
        }
        if (inRangeTiles.size() == 4) {
            if (std::uniform_int_distribution<>(0,1)(this->randomGenerator) == 1) {
                inRangeTiles.pop_back();
            } else {
                inRangeTiles.pop_front();
            }
        } else {
            int rand = std::uniform_int_distribution<>(0,2)(this->randomGenerator);
            if (rand == 0) {
                inRangeTiles.pop_back();
                inRangeTiles.pop_back();
            } else if (rand == 1) {
                inRangeTiles.pop_back();
                inRangeTiles.pop_front();
            } else {
                inRangeTiles.pop_front();
                inRangeTiles.pop_front();
            }
        }
    }
    return inRangeTiles;
}

unsigned int ThreesBoard::getNextTile() {
    unsigned int theTile = this->upcomingTile;
    std::uniform_int_distribution<> bonusChance(1,21);
    if (this->canGiveBonusTile() && bonusChance(this->randomGenerator) == 21) {
        this->upcomingTile = this->getBonusTile();
    } else {
        this->rebuildTileStackIfNecessary();
        this->upcomingTile = *this->tileStack.rbegin();
        this->tileStack.pop_back();
    }
    return theTile;
}

unsigned int ThreesBoard::getMaxTile() {
    unsigned int maxTile = 0;
    for (std::array<unsigned int, 4> row : this->board) {
        maxTile = std::max(maxTile, *std::max_element(row.begin(), row.end()));
    }
    return maxTile;
}

bool ThreesBoard::canGiveBonusTile(){
    return this->getMaxTile() >= 48;
}

unsigned int ThreesBoard::maxBonusTile() {
    return this->getMaxTile()/8;
}

unsigned int ThreesBoard::getBonusTile() {
    unsigned int maxBonus = this->maxBonusTile();
    std::vector<unsigned int> possibleBonuses;
    while (maxBonus > 3) {
        possibleBonuses.push_back(maxBonus);
        maxBonus /= 2;
    }
    std::shuffle(possibleBonuses.begin(), possibleBonuses.end(), ThreesBoard::randomGenerator);
    return possibleBonuses[0];
}

std::vector<std::tuple<float, ThreesBoard, unsigned int>> ThreesBoard::possibleNextBoardStates() {
    //TODO: convert this to use number of each type of tile remaining
    std::vector<std::tuple<float, ThreesBoard, unsigned int>> result;
    float num_ones = std::count(this->tileStack.begin(), this->tileStack.end(), 1);
    float num_twos = std::count(this->tileStack.begin(), this->tileStack.end(), 2);
    float num_threes = std::count(this->tileStack.begin(), this->tileStack.end(), 3);
    float num_elems = this->tileStack.size();
    
    if (num_ones > 0) {
        std::deque<unsigned int> nextStack;
        for (int i = 0; i < num_ones - 1; ++i) {
            nextStack.push_back(1);
        }
        for (int i = 0; i < num_twos; ++i) {
            nextStack.push_back(2);
        }
        for (int i = 0; i < num_threes; ++i) {
            nextStack.push_back(3);
        }
        
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = nextStack;
        nextBoard.upcomingTile = 1;
        result.push_back({num_ones/num_elems, nextBoard, 1});
    }
    
    if (num_twos > 0) {
        std::deque<unsigned int> nextStack;
        for (int i = 0; i < num_ones; ++i) {
            nextStack.push_back(1);
        }
        for (int i = 0; i < num_twos - 1; ++i) {
            nextStack.push_back(2);
        }
        for (int i = 0; i < num_threes; ++i) {
            nextStack.push_back(3);
        }
        
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = nextStack;
        nextBoard.upcomingTile = 2;
        result.push_back({num_twos/num_elems, nextBoard, 2});
    }
    
    if (num_threes > 0) {
        std::deque<unsigned int> nextStack;
        for (int i = 0; i < num_ones; ++i) {
            nextStack.push_back(1);
        }
        for (int i = 0; i < num_twos; ++i) {
            nextStack.push_back(2);
        }
        for (int i = 0; i < num_threes - 1; ++i) {
            nextStack.push_back(3);
        }
        
        ThreesBoard nextBoard = ThreesBoard(*this);
        nextBoard.tileStack = nextStack;
        nextBoard.upcomingTile = 3;
        result.push_back({num_twos/num_elems, nextBoard, 3});
    }
    
    return result;
}

std::vector<ThreesBoard::BoardIndex> ThreesBoard::validIndicesForNewTile(Direction d) {
    std::array<std::pair<unsigned, unsigned>, 4> indicies;
    switch (d) {
        case LEFT:
            indicies = {std::pair<unsigned, unsigned>(0,0),std::pair<unsigned, unsigned>(0,1),std::pair<unsigned, unsigned>(0,2),std::pair<unsigned, unsigned>(0,3)};
            break;
        case RIGHT:
            indicies = {std::pair<unsigned, unsigned>(3,0),std::pair<unsigned, unsigned>(3,1),std::pair<unsigned, unsigned>(3,2),std::pair<unsigned, unsigned>(3,3)};
            break;
        case UP:
            indicies = {std::pair<unsigned, unsigned>(0,0),std::pair<unsigned, unsigned>(1,0),std::pair<unsigned, unsigned>(2,0),std::pair<unsigned, unsigned>(3,0)};
            break;
        case DOWN:
            indicies = {std::pair<unsigned, unsigned>(0,3),std::pair<unsigned, unsigned>(1,3),std::pair<unsigned, unsigned>(2,3),std::pair<unsigned, unsigned>(3,3)};
            break;
        default:
            break;
    }
    auto endIterator = std::remove_if(indicies.begin(), indicies.end(), [this](ThreesBoard::BoardIndex tile) {
        return *this->at(tile) != 0;
    });
    std::vector<ThreesBoard::BoardIndex> result(indicies.begin(), endIterator);
    return result;
}

std::pair<unsigned int, ThreesBoard::BoardIndex> ThreesBoard::addTile(Direction d) {
    auto indices = this->validIndicesForNewTile(d);
    std::shuffle(indices.begin(), indices.end(), this->randomGenerator);
    unsigned int nextTileValue = this->getNextTile();
    return {nextTileValue, *indices.begin()};
}

unsigned int ThreesBoard::score() {
    return std::accumulate(this->board.begin(), this->board.end(), 0, [](unsigned int acc1, std::array<unsigned int, 4> row){
        return std::accumulate(row.begin(), row.end(), acc1, [](unsigned int acc2, unsigned int tile){
            return acc2 + ThreesBoard::tileScore(tile);
        });
    });
}

unsigned int ThreesBoard::tileScore(unsigned int tileValue) {
    return std::unordered_map<unsigned int, unsigned int>({
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

std::vector<Direction> ThreesBoard::validMoves(){
    std::vector<Direction> result;
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

bool ThreesBoard::isGameOver() {
    return this->validMoves().empty();
}

template < class T >
std::ostream& operator << (std::ostream& os, const std::deque<T>& v)
{
    os << "[";
    for (typename std::deque<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream &os, ThreesBoard board){
    os << board.possibleUpcomingTiles() << std::endl;
    os << "---------------------  Current Score: " <<  board.score() << std::endl;
    os << "|" << std::setw(4) << *board.at(0,0) << "|" << std::setw(4) << *board.at(1,0) << "|" << std::setw(4) << *board.at(2,0) << "|" << std::setw(4) << *board.at(3,0) << "|" << std::endl;
    os << "|" << std::setw(4) << *board.at(0,1) << "|" << std::setw(4) << *board.at(1,1) << "|" << std::setw(4) << *board.at(2,1) << "|" << std::setw(4) << *board.at(3,1) << "|" << std::endl;
    os << "|" << std::setw(4) << *board.at(0,2) << "|" << std::setw(4) << *board.at(1,2) << "|" << std::setw(4) << *board.at(2,2) << "|" << std::setw(4) << *board.at(3,2) << "|" << std::endl;
    os << "|" << std::setw(4) << *board.at(0,3) << "|" << std::setw(4) << *board.at(1,3) << "|" << std::setw(4) << *board.at(2,3) << "|" << std::setw(4) << *board.at(3,3) << "|" << std::endl;
    os << "---------------------" << std::endl;
    return os;
}