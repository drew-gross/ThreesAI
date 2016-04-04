//
//  RealBoardOutput.h
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__RealBoardOutput__
#define __ThreesAI__RealBoardOutput__

#include "BoardOutput.h"

#include "GameStateSource.h"

class RealBoardOutput : public BoardOutput {
public:
    RealBoardOutput(std::string port, std::shared_ptr<GameStateSource> source, AboutToMoveBoard const& initialState, std::shared_ptr<HintImages const> hintImages);
    void move(Direction d, AboutToMoveBoard const& originalBoard);
    BoardStateCPtr currentState(HiddenBoardState otherInfo) const;
    AddedTileInfo computeChangeFrom(AboutToAddTileBoard const& previousState) const;
    BoardStateCPtr sneakyState() const;
    
    void pressWithServo();
    void moveStepper(Direction d);
private:
    int fd;
    std::shared_ptr<GameStateSource> source;
    std::shared_ptr<HintImages const> hintImages;
};

#endif /* defined(__ThreesAI__RealBoardOutput__) */
