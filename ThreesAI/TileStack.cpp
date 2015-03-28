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
    this->prepUpcomingTile(3);
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
    this->prepUpcomingTile(maxTile);
    return theTile;
}

void TileStack::prepUpcomingTile(unsigned int maxTile) {
    uniform_real_distribution<> r(0,1);
    float tileFinder = r(TileStack::randomGenerator);
    auto possibleNexts = this->possibleNextTiles(maxTile);
    for (auto&& possibleNextTile : possibleNexts) {
        float possibleNextsProbability = possibleNextTile.second;
        unsigned int possibleNext = possibleNextTile.first;
        if (tileFinder < possibleNextsProbability) {
            switch (possibleNext) {
                case 1:
                    this->ones--;
                    break;
                case 2:
                    this->twos--;
                    break;
                case 3:
                    this->threes--;
                    break;
            }
            this->upcomingTile = possibleNext;
            this->rebuildIfNecessary();
            return;
        } else {
            tileFinder -= possibleNextsProbability;
        }
    }
    debug();
}

float TileStack::nonBonusTileProbability(unsigned int tile, bool canHaveBonus) const {
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
    float nonBonusProbability = float(count)/this->size();
    if (canHaveBonus) {
        nonBonusProbability *= float(20)/21;
    }
    return nonBonusProbability;
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
        result.push_back({1, this->nonBonusTileProbability(1, maxBoardTile >= 48)});
    }
    if (twos >= 0) {
        result.push_back({2, this->nonBonusTileProbability(2, maxBoardTile >= 48)});
    }
    if (threes >= 0) {
        result.push_back({3, this->nonBonusTileProbability(3, maxBoardTile >= 48)});
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