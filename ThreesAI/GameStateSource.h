//
//  GameStateSource.h
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__GameStateSource__
#define __ThreesAI__GameStateSource__

#include "IMProc.h"

#include <memory>

class GameStateSource {
public:
    virtual std::shared_ptr<AboutToMoveBoard const> getGameState(HiddenBoardState otherInfo) = 0;
    std::shared_ptr<AboutToMoveBoard const> getInitialState();
};

#endif /* defined(__ThreesAI__GameStateSource__) */
