//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>

#include "Logging.h"

#include "ThreesBoard.h"
#include "RandomAI.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "HumanAI.h"

using namespace std;

int main(int argc, const char * argv[]) {
    random_device rd;
    TileStack::randomGenerator.seed(0);
    ExpectimaxAI ai;
    clock_t startTime = clock();
    while (!ai.board.isGameOver()) {
        cout << ai.board << endl;
        Direction move = ai.playTurn();
        cout << move << endl;
    }
    clock_t endTime = clock();
    double elapsed_time = (endTime-startTime)/(double)CLOCKS_PER_SEC;
    MYLOG(elapsed_time);
    return 0;
}