//
//  TileStack.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__TileStack__
#define __ThreesAI__TileStack__

#include <random>
#include <deque>

class TileStack {
public:
    TileStack();
    TileStack(unsigned char ones, unsigned char twos, unsigned char threes, unsigned int upcomingTile);
    
    unsigned char ones;
    unsigned char twos;
    unsigned char threes;
    
    bool empty();
    unsigned int size();
    unsigned int getNextTile(unsigned int maxBoardTile);
    std::deque<unsigned int> possibleUpcomingTiles(unsigned int maxBoardTile);
    
    static std::default_random_engine randomGenerator; //TODO: this should probably be stored somewhere else?
    
private:
    unsigned int upcomingTile;
    unsigned int maxBonusTile(unsigned int maxBoardTile);
    unsigned int getBonusTile(unsigned int maxBoardTile);
    void rebuildIfNecessary();
    
};

#endif /* defined(__ThreesAI__TileStack__) */