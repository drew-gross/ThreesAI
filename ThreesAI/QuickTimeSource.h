//
//  QuickTimeSource.h
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__QuickTimeSource__
#define __ThreesAI__QuickTimeSource__

#include "GameStateSource.h"

class QuickTimeSource : public GameStateSource {
public:
    QuickTimeSource();
    
    BoardInfo getGameState();
};

#endif /* defined(__ThreesAI__QuickTimeSource__) */
