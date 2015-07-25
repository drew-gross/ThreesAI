
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

void testImage(path p) {
    Mat image = imread(p.string());
    Mat boardImage = IMProc::colorImageToBoard(image);
    SimulatedThreesBoard expectedBoard = SimulatedThreesBoard::fromString(p.stem().string());
    array<Mat, 16> is = IMProc::tileImages(boardImage);
    
    int failures = 0;
    for (unsigned char i = 0; i < 16; i++) {
        MatchResult extracted = IMProc::tileValue(is[i], IMProc::canonicalTiles());
        int expectedValue = expectedBoard.at({i%4,i/4});
        if (expectedValue != extracted.tile.value) {
            MatchResult expected(IMProc::canonicalTiles().at(expectedValue), is[i]);
            vector<Mat> expectedV = {expected.knnDrawing(), expected.ratioPassDrawing(), expected.noDupeDrawing()};
            vector<Mat> extractedV = {extracted.knnDrawing(), extracted.ratioPassDrawing(), extracted.noDupeDrawing()};
            MYSHOW(IMProc::concatH({IMProc::concatV(expectedV), IMProc::concatV(extractedV)}));
            debug();
            IMProc::tileValue(is[i], IMProc::canonicalTiles());
            failures++;
        }
    }
    if (failures > 0) {
        imwrite(p.string(), image); //Make most recently failed tests run first.
    }
    MYLOG(failures); 
}

void testImageProc() {
    vector<vector<Mat>> canonicalKeypoints = {{}};
    for (auto&& image : IMProc::canonicalTiles()) {
        if (canonicalKeypoints.rbegin()->size() == 7) {
            canonicalKeypoints.push_back({});
        }
        Mat kpi;
        drawKeypoints(image.second.image, image.second.keypoints, kpi);
        canonicalKeypoints.rbegin()->push_back(kpi);
    }
    vector<Mat> cv;
    for (auto&& vector : canonicalKeypoints) {
        cv.push_back(IMProc::concatV(vector));
    }
    //MYSHOW(IMProc::concatH(cv)); debug();
    
    vector<path> paths;
    for (auto&& path : directory_iterator(Log::project_path + "TestCaseImages/")) {
        paths.push_back(path.path());
    }
    sort(paths.begin(), paths.end(), [](path l, path r){
        return last_write_time(l) > last_write_time(r);
    });
    
    for (auto&& path : paths) {
        if (path.extension() == ".png") {
            testImage(path);
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
    testImageProc(); debug();
    
    for (;;) {
        shared_ptr<ThreesBoardBase> b = make_shared<RealThreesBoard>("/dev/tty.usbmodem1411");
        ExpectimaxAI ai(b);
        ai.playGame();
        MYLOG("game over");
    }
    return 0;
}
