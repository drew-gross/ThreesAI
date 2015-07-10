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
#include "IMProc.h"

#include "SimulatedThreesBoard.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
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
    Mat boardImage = IMProc::colorImageToBoard(image);
    SimulatedThreesBoard expectedBoard = SimulatedThreesBoard::fromString(p.stem().string());
    array<Mat, 16> is = IMProc::tileImages(boardImage);
    
    int failures = 0;
    for (unsigned char i = 0; i < 16; i++) {
        MatchResult match = IMProc::tileValue(is[i], IMProc::canonicalTiles());
        int extractedValue = match.tile.value;
        int expectedValue = expectedBoard.at({i%4,i/4});
        if (expectedValue != extractedValue) {
            if (expectedValue != 0) {
                MatchResult expectedMatch(IMProc::canonicalTiles().at(expectedValue), is[i]);
                MYSHOW(expectedMatch.knnDrawing);
                MYSHOW(expectedMatch.ratioPassDrawing);
                MYSHOW(expectedMatch.noDupeDrawing);
                MYSHOW(match.knnDrawing);
                MYSHOW(match.ratioPassDrawing);
                MYSHOW(match.noDupeDrawing);
                debug();
            } else {
                MYSHOW(match.knnDrawing);
                MYSHOW(match.ratioPassDrawing);
                MYSHOW(match.noDupeDrawing);
                debug();
            }
            IMProc::tileValue(is[i], IMProc::canonicalTiles());
            failures++;
        }
    }
    MYLOG(failures);
}

void testImageProc() {
    for (auto&& image : directory_iterator(project_path + "TestCaseImages/")) {
        if (image.path().extension() == ".png") {
            testImage(image.path());
        }
    }
}

void testBoardMovement() {
    SimulatedThreesBoard b = SimulatedThreesBoard::fromString("0,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,0,0,\
                                                               0,0,0,0");
    b.moveWithoutAdd(LEFT);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,1,0,0,\
                                                              0,0,0,0,\
                                                              0,0,0,0"), {}));
    b.moveWithoutAdd(DOWN);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,0,0,\
                                                              0,1,0,0,\
                                                              0,0,0,0"), {}));
    b.moveWithoutAdd(RIGHT);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,0,0,\
                                                              0,0,1,0,\
                                                              0,0,0,0"), {}));
    b.moveWithoutAdd(UP);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,1,0,\
                                                              0,0,0,0,\
                                                              0,0,0,0"), {}));
    
    
    SimulatedThreesBoard x = SimulatedThreesBoard::fromString("6,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,6,0,\
                                                               0,6,0,0");
    
    SimulatedThreesBoard y = SimulatedThreesBoard::fromString("3,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,3,0,\
                                                               0,0,2,0");
    debug(!x.hasSameTilesAs(y, {{0,0}, {1,3}, {2,2}, {2,3}}));
}

int main(int argc, const char * argv[]) {
    testBoardMovement();
    //testImageProc();
    
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
