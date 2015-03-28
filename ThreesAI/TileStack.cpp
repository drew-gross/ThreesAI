//
//  TileStack.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "TileStack.h"

#include <vector>

#include "Debug.h"

using namespace std;

TileStack::TileStack() : ones(4), twos(4), threes(4) {
    this->getNextTile(3); //preload upcomingtile
}

TileStack::TileStack(unsigned char ones, unsigned char twos, unsigned char threes, unsigned int upcomingTile) : ones(ones), twos(twos), threes(threes), upcomingTile(upcomingTile) {
    if (ones + twos + threes == 0) {
        this->ones = 4;
        this->twos = 4;
        this->threes = 4;
    }
}

default_random_engine TileStack::randomGenerator = default_random_engine();

unsigned int TileStack::getNextTile(unsigned int maxTile) {
    unsigned int theTile = this->upcomingTile;
    uniform_int_distribution<> bonusChance(1,21);
    if (maxTile >= 48 && bonusChance(this->randomGenerator) == 21) {
        this->upcomingTile = this->getBonusTile(maxTile);
    } else {
        this->rebuildIfNecessary();
        vector<unsigned int> possibleTiles;
        if (ones > 0) {
            possibleTiles.push_back(1);
        }
        if (twos > 0) {
            possibleTiles.push_back(2);
        }
        if (threes > 0) {
            possibleTiles.push_back(3);
        }
        shuffle(possibleTiles.begin(), possibleTiles.end(), this->randomGenerator); //TODO: this is wrong! a stack with 1 1 and 4 3s is more likely to have a 3 next
        this->upcomingTile = possibleTiles[0];
        if (this->upcomingTile == 1) {
            ones--;
        }
        if (this->upcomingTile == 2) {
            twos--;
        }
        if (this->upcomingTile == 3) {
            threes--;
        }
    }
    return theTile;
}

float TileStack::nonBonusTileProbability(unsigned int tile) const {
    unsigned int count = 0;
    switch (tile) {
        case 1:
            count = this->ones;
            break;
        case 2:
            count = this->twos;
            break;
        case 3:
            count = this->threes;
            break;
    }
    return (float(count)/this->size())*(float(20)/21);
}

unsigned int int_log2(unsigned int x) {
    unsigned int result = 0;
    while (x > 0) {
        result++;
        x >>= 1;
    }
    return result;
}

deque<pair<unsigned int, float>> TileStack::possibleNextTiles(unsigned int maxBoardTile) const {
    deque<pair<unsigned int, float>> result;
    //should be able to only add 1,2,3 if they are in the stack
    if (ones >= 0) {
        result.push_back({1, this->nonBonusTileProbability(1)});
    }
    if (twos >= 0) {
        result.push_back({2, this->nonBonusTileProbability(2)});
    }
    if (threes >= 0) {
        result.push_back({3, this->nonBonusTileProbability(3)});
    }
    maxBoardTile /= 8;
    unsigned int numPossibleBonusTiles = int_log2(maxBoardTile) - 2;
    while (maxBoardTile >= 6) {
        debug(float(1)/numPossibleBonusTiles/21 > 1);
        result.push_back({maxBoardTile, float(1)/numPossibleBonusTiles/21});
        maxBoardTile /= 2;
    }
    return result;
}

deque<unsigned int> TileStack::nextTileHint(unsigned int maxTile) const {
    deque<unsigned int> inRangeTiles;
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
        if (this->upcomingTile * 2 <= this->maxBonusTile(maxTile)) {
            inRangeTiles.push_back(this->upcomingTile*2);
        }
        if (this->upcomingTile * 4 <= this->maxBonusTile(maxTile)) {
            inRangeTiles.push_back(this->upcomingTile*4);
        }
        
        //trim the list down to size
        if (inRangeTiles.size() <= 3) {
            return inRangeTiles;
        }
        if (inRangeTiles.size() == 4) {
            if (uniform_int_distribution<>(0,1)(TileStack::randomGenerator) == 1) {
                inRangeTiles.pop_back();
            } else {
                inRangeTiles.pop_front();
            }
        } else {
            int rand = uniform_int_distribution<>(0,2)(TileStack::randomGenerator);
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

void TileStack::rebuildIfNecessary() {
    if (this->empty()) {
        this->ones = 4;
        this->twos = 4;
        this->threes = 4;
    }
}

unsigned int TileStack::maxBonusTile(unsigned int maxBoardTile) const {
    return maxBoardTile/8;
}

unsigned int TileStack::getBonusTile(unsigned int maxBoardTile) {
    unsigned int maxBonus = this->maxBonusTile(maxBoardTile);
    vector<unsigned int> possibleBonuses;
    while (maxBonus > 3) {
        possibleBonuses.push_back(maxBonus);
        maxBonus /= 2;
    }
    shuffle(possibleBonuses.begin(), possibleBonuses.end(), this->randomGenerator);
    return possibleBonuses[0];
}

unsigned int TileStack::size() const {
    return this->ones+this->twos+this->threes;
}

bool TileStack::empty() {
    return this->size() == 0;
}