//
//  SimulatedBoardOutput.h
//  ThreesAI
//
//  Created by Drew Gross on 9/15/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__SimulatedBoardOutput__
#define __ThreesAI__SimulatedBoardOutput__

#include "BoardOutput.h"

#include "BoardState.h"

#include <memory>

class SimulatedBoardOutput : public BoardOutput {
public:
    std::shared_ptr<AboutToMoveBoard const> state; //Made public to make debugging easier
    SimulatedBoardOutput(Board otherBoard, std::default_random_engine hintGen, unsigned int onesInStack, unsigned int twosInStack, unsigned int threesInStack);
    SimulatedBoardOutput(BoardStateCPtr b);
    
    static std::unique_ptr<SimulatedBoardOutput> randomBoard(std::default_random_engine& shuffler);
    void move(Direction d, AboutToMoveBoard const& originalBoard);
    BoardStateCPtr currentState(HiddenBoardState otherInfo) const;
    AddedTileInfo computeChangeFrom(AboutToAddTileBoard const& previousBoart) const;
    
    BoardStateCPtr sneakyState() const;
};

#endif /* defined(__ThreesAI__SimulatedBoardOutput__) */
