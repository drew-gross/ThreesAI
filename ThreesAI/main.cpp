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
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "HumanAI.h"

using namespace std;

int main(int argc, const char * argv[]) {
    random_device rd;
    TileStack::randomGenerator.seed(rd());
    HumanAI ai;
    while (!ai.board.isGameOver()) {
        ai.playTurn();
        std::cout << ai.board << std::endl;
    }
    return 0;
}