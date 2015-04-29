//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>

#include <memory>

#include "Logging.h"

#include "SimulatedThreesBoard.h"
#include "RandomAI.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "HumanAI.h"
#include "RealThreesBoard.h"

#include <opencv2/highgui/highgui.hpp>

using namespace std;

template <typename T>
ostream& operator<<(ostream &os, const std::deque<T> d){
    os << "[";
    for (auto&& t : d) {
        os << " " << t;
    }
    os << "]";
    return os;
}

void playOneGame() {
    unique_ptr<SimulatedThreesBoard> b(new SimulatedThreesBoard);
    ExpectimaxAI ai(move(b));
    clock_t startTime = clock();
    while (!ai.board->isGameOver()) {
        cout << &ai.board << endl;
        Direction move = ai.playTurn();
        cout << move << endl << endl;
    }
    cout << &ai.board << endl;
    clock_t endTime = clock();
    double elapsed_time = (endTime-startTime)/(double)CLOCKS_PER_SEC;
    MYLOG(elapsed_time);
}

int main(int argc, const char * argv[]) {
    cv::namedWindow("stest");
    
    deque<unsigned int> turnsSurvived;
    for (int seed=1; seed <= 3; seed++) {
        TileStack::randomGenerator.seed(seed);
        unique_ptr<ThreesBoardBase> b(new RealThreesBoard("/dev/tty.usbmodem1411"));
        HumanAI ai(move(b));
        ai.playGame();
        turnsSurvived.push_back(ai.board->numTurns);
        MYLOG(seed);
    }
    MYLOG(turnsSurvived);
    return 0;
}
