//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>
#include "ThreesBoard.h"
#include "RandomAI.h"

int main(int argc, const char * argv[]) {
    std::random_device rd;
    ThreesBoard::randomGenerator.seed(rd());
    RandomAI ai;
    while (!ai.board.isGameOver()) {
        ai.playTurn();
        std::cout << ai.board << std::endl;
    }
    return 0;
}