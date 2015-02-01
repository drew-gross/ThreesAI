//
//  ThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoard.h"
#include <iostream>
#include <random>

ThreesBoard::ThreesBoard() {
    std::array<unsigned int, 16> initialTiles = {3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0};
    std::random_device randomDevice;
    //auto randomGenerator = std::default_random_engine(randomDevice);
    //std::shuffle(initialTiles.begin(), initialTiles.end(), randomGenerator);
    this->board = std::array<std::array<unsigned int, 4>, 4>();
    for (unsigned i = 0; i < initialTiles.size(); i++) {
        this->board[i/4][i%4] = initialTiles[i];
    }
}

unsigned int* ThreesBoard::at(unsigned int x, unsigned int y) {
    return &this->board[y][x];
}

unsigned int* ThreesBoard::at(std::pair<unsigned, unsigned> p){
    return &this->board[p.second][p.first];
}

bool ThreesBoard::tryMerge(unsigned targetX, unsigned targetY, unsigned otherX, unsigned otherY) {
    if (*this->at(targetX,targetY) == *this->at(otherX,otherY) && *this->at(targetX, targetY) != 1 && *this->at(otherX, otherY) != 2) {
        *this->at(targetX,targetY) *= 2;
        *this->at(otherX,otherY) = 0;
        return true;
    } else if ((*this->at(targetX,targetY) == 1 and *this->at(otherX,otherY) == 2) or (*this->at(targetX,targetY) == 2 and *this->at(otherX,otherY) == 1)) {
        *this->at(targetX,targetY) = 3;
        *this->at(otherX,otherY) = 0;
        return true;
    } else if (*this->at(targetX, targetY) == 0 and *this->at(otherX, otherY) != 0) {
        *this->at(targetX, targetY) = *this->at(otherX, otherY);
        *this->at(otherX, otherY) = 0;
        return true;
    }
    return false;
}

void ThreesBoard::processInputDirection(Direction d) {
    bool successfulMerge = false;
    switch (d) {
        case UP:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(i,0,i,1);
                successfulMerge |= this->tryMerge(i,1,i,2);
                successfulMerge |= this->tryMerge(i,2,i,3);
            }
            if (successfulMerge) {
                this->addTile(DOWN);
            }
            break;
        case DOWN:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(i,3,i,2);
                successfulMerge |= this->tryMerge(i,2,i,1);
                successfulMerge |= this->tryMerge(i,1,i,0);
            }
            if (successfulMerge) {
                this->addTile(UP);
            }
            break;
        case LEFT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(0,i,1,i);
                successfulMerge |= this->tryMerge(1,i,2,i);
                successfulMerge |= this->tryMerge(2,i,3,i);
            }
            if (successfulMerge) {
                this->addTile(RIGHT);
            }
            break;
        case RIGHT:
            for (unsigned i = 0; i < 4; i++) {
                successfulMerge |= this->tryMerge(3,i,2,i);
                successfulMerge |= this->tryMerge(2,i,1,i);
                successfulMerge |= this->tryMerge(1,i,0,i);
            }
            if (successfulMerge) {
                this->addTile(LEFT);
            }
            break;
        default:
            break;
    }
}

unsigned int ThreesBoard::getNextTile() {
    return 3;
}

void ThreesBoard::addTile(Direction d) {
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
    std::random_device randomDevice;
    //auto randomGenerator = std::default_random_engine(randomDevice);
    //std::shuffle(indicies.begin(), indicies.end(), randomGenerator);
    for (auto it = indicies.begin(); it != indicies.end(); it++) {
        if (*this->at(*it) == 0) {
            *this->at(*it) = this->getNextTile();
            return;
        }
    }
}

std::ostream& operator<<(std::ostream &os, ThreesBoard board){
    os << "---" << std::endl;
    os << "|" << *board.at(0,0) << "|" << *board.at(1,0) << "|" << *board.at(2,0) << "|" << *board.at(3,0) << "|" << std::endl;
    os << "|" << *board.at(0,1) << "|" << *board.at(1,1) << "|" << *board.at(2,1) << "|" << *board.at(3,1) << "|" << std::endl;
    os << "|" << *board.at(0,2) << "|" << *board.at(1,2) << "|" << *board.at(2,2) << "|" << *board.at(3,2) << "|" << std::endl;
    os << "|" << *board.at(0,3) << "|" << *board.at(1,3) << "|" << *board.at(2,3) << "|" << *board.at(3,3) << "|" << std::endl;
    os << "---" << std::endl;
    return os;
}