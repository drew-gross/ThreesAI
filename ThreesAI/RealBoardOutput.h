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
    RealBoardOutput(std::string port, std::shared_ptr<GameStateSource> source, BoardState const& initialState);
    ~RealBoardOutput();
    void move(Direction d, BoardState const& originalBoard);
    std::shared_ptr<BoardState const> currentState(HiddenBoardState otherInfo) const;
private:
    int fd;
    std::shared_ptr<GameStateSource> source;
};

#endif /* defined(__ThreesAI__RealBoardOutput__) */
