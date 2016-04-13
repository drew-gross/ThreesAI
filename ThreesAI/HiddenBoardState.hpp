//
//  HiddenBoardState.hpp
//  ThreesAI
//
//  Created by Drew Gross on 4/12/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef HiddenBoardState_hpp
#define HiddenBoardState_hpp

#include "Debug.h"
#include "Tile.hpp"

class HiddenBoardState {
public:
    HiddenBoardState(unsigned int numTurns, unsigned int onesInStack, unsigned int twosInStack, unsigned int threesInStack) :
    numTurns(numTurns) {
        debug(onesInStack > 4);
        debug(twosInStack > 4);
        debug(threesInStack > 4);
        this->onesInStack = onesInStack;
        this->twosInStack = twosInStack;
        this->threesInStack = threesInStack;
        if (this->onesInStack == 0 && this->twosInStack == 0 && this->threesInStack == 0) {
            this->onesInStack = 4;
            this->twosInStack = 4;
            this->threesInStack = 4;
        }
    }
    
    HiddenBoardState nextTurnStateWithAddedTile(Tile t) const;
    unsigned int stackSize() const {
        return this->onesInStack + this->twosInStack + this->threesInStack;
    }
    unsigned int numTurns;
    unsigned int onesInStack:3;
    unsigned int twosInStack:3;
    unsigned int threesInStack:3;
    
    bool operator==(HiddenBoardState const& other) const;
};


#endif /* HiddenBoardState_hpp */
