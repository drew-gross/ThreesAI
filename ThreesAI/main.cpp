//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>

#include <memory>
#include <array>

#include "Logging.h"
#include "Debug.h"

#include "SimulatedThreesBoard.h"
#include "RandomAI.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "HumanPlayer.h"
#include "RealThreesBoard.h"

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace cv;

void playOneGame() {
    unique_ptr<SimulatedThreesBoard> b(new SimulatedThreesBoard(SimulatedThreesBoard::randomBoard()));
    ZeroDepthMaxScoreAI ai(move(b));
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

const string project_path = "/Users/drewgross/Projects/ThreesAI/";

void testImage(path p) {
    Mat image = imread(p.string());
    SimulatedThreesBoard expectedBoard = SimulatedThreesBoard::fromString(p.stem().string());
    array<Mat, 16> is = IMProc::tileImages(image);
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            int extratedValue = IMProc::tileValue(is[i*4+j], IMProc::loadCanonicalTiles());
            if (expectedBoard.at({i,j}) != extratedValue) {
                waitKey();
                debug();
            }
        }
    }
    
    SimulatedThreesBoard extractedBoard = SimulatedThreesBoard(IMProc::boardState(IMProc::colorImageToBoard(imread(p.string())), IMProc::canonicalTiles));
    
    if (!expectedBoard.hasSameTilesAs(extractedBoard, {})) {
        MYSHOW(image);
        MYLOG(p);
        waitKey();
        debug();
    }
}

void runTests() {
    for (auto&& image : directory_iterator(project_path + "TestCaseImages/")) {
        if (image.path().extension() == ".png") {
            testImage(image.path());
        }
    }
}

int main(int argc, const char * argv[]) {
    runTests();
    
    deque<unsigned int> turnsSurvived;
    for (int seed=1; seed <= 3; seed++) {
        TileStack::randomGenerator.seed(seed);
        unique_ptr<ThreesBoardBase> b(new RealThreesBoard("/dev/tty.usbmodem1411"));
        ZeroDepthMaxScoreAI ai(move(b));
        ai.playGame();
        turnsSurvived.push_back(ai.board->numTurns);
        MYLOG(seed);
    }
    MYLOG(turnsSurvived);
    return 0;
}
