//
//  RealThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__RealThreesBoard__
#define __ThreesAI__RealThreesBoard__

#include <memory>

#include <opencv2/opencv.hpp>

#include "IMProc.h"
#include "GameStateSource.h"

class RealThreesBoard : public ThreesBoardBase {
public:
    RealThreesBoard(std::string port, std::unique_ptr<GameStateSource> source, BoardInfo initialState);
    
    //Throws if move is invalid. Returns location and value of new tile if not.
    MoveResult move(Direction d);
    SimulatedThreesBoard simulatedCopy() const;
    
    ~RealThreesBoard();
    
private:
    
    std::unique_ptr<GameStateSource> source;
    BoardInfo oldState;
    
    int fd;
    
    void connectAndStart(std::string portName);
};

#endif /* defined(__ThreesAI__RealThreesBoard__) */
