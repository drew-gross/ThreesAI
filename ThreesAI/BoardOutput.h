//
//  BoardOutput.h
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__BoardOutput__
#define __ThreesAI__BoardOutput__

#include "Direction.h"
#include "BoardState.h"
#include "AddedTileInfo.h"

#include <functional>

typedef std::function<void(void)> Work;

class BoardOutput {
public:
    //Throws if move is invalid. Returns location and value of new tile if not.
    virtual void move(Direction d, BoardState const& originalBoard) = 0;
    virtual std::shared_ptr<BoardState const> currentState(HiddenBoardState otherInfo) const = 0;
    virtual ~BoardOutput() {};
    virtual AddedTileInfo computeChangeFrom(BoardState const& previousState) const = 0;
    Work doWork;    
    
    virtual std::shared_ptr<BoardState const> sneakyState() const = 0;

    void doWorkFor(int millis);
};

#endif /* defined(__ThreesAI__BoardOutput__) */
