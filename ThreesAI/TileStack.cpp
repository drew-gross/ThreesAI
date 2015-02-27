//
//  TileStack.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "TileStack.h"

#include <vector>

using namespace std;

TileStack::TileStack() : ones(4), twos(4), threes(4) {
    this->getNextTile(3); //preload upcomingtile
}

TileStack::TileStack(unsigned char ones, unsigned char twos, unsigned char threes, unsigned int upcomingTile) : ones(ones), twos(twos), threes(threes), upcomingTile(upcomingTile) {
    
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
        shuffle(possibleTiles.begin(), possibleTiles.end(), this->randomGenerator);
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

deque<unsigned int> TileStack::possibleUpcomingTiles(unsigned int maxTile) {
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
            inRangeTiles.push_back(this->upcomingTile*2);
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

unsigned int TileStack::maxBonusTile(unsigned int maxBoardTile) {
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

unsigned int TileStack::size() {
    return this->ones+this->twos+this->threes;
}

bool TileStack::empty() {
    return this->size() == 0;
}