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
#include "IMLog.h"

#include "SimulatedThreesBoard.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "RealThreesBoard.h"
#include "CameraSource.h"
#include "QuickTimeSource.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost;
using namespace cv;
using namespace IMLog;

void playOneGame() {
    ZeroDepthMaxScoreAI ai(move(SimulatedThreesBoard::randomBoard()));
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

unsigned int testImage(path p) {
    unsigned int failures = 0;
    SimulatedThreesBoard expectedBoard = SimulatedThreesBoard::fromString(p.stem().string());
    
    vector<string> splitName;
    split(splitName, p.stem().string(), is_any_of("-"));
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    deque<unsigned int> nextTileHint;
    nextTileHint.resize(nextTileHintStrings.size());
    transform(nextTileHintStrings.begin(), nextTileHintStrings.end(), nextTileHint.begin(), [](string s){
        return stoi(s);
    });
    
    Mat camImage = imread(p.string());
    Mat screenImage = IMProc::screenImage(camImage);
    array<Mat, 16> tiles = IMProc::tileImages(IMProc::boardImageFromScreen(screenImage));
    BoardInfo state = IMProc::boardState(screenImage, camImage, IMProc::canonicalTiles());
    if (state.nextTileHint != nextTileHint && nextTileHint.size() == 1 && nextTileHint[0] != 6) {
        MYSHOW(screenImage);
        MYLOG(nextTileHint);
        MYLOG(state.nextTileHint);
        failures++; 
        debug();
        IMProc::boardState(screenImage, camImage, IMProc::canonicalTiles());
    }
    for (unsigned char i = 0; i < 16; i++) {
        MatchResult extracted = IMProc::tileValue(tiles[i], IMProc::canonicalTiles());
        int expectedValue = expectedBoard.at({i%4,i/4});
        if (state.tiles[i] != extracted.tile.value) {
            MatchResult expected(IMProc::canonicalTiles().at(expectedValue), tiles[i]);
            vector<Mat> expectedV = {expected.knnDrawing(), expected.ratioPassDrawing(), expected.noDupeDrawing()};
            vector<Mat> extractedV = {extracted.knnDrawing(), extracted.ratioPassDrawing(), extracted.noDupeDrawing()};
            MYSHOW(concatH({concatV(expectedV), concatV(extractedV)}));
            debug();
            IMProc::tileValue(tiles[i], IMProc::canonicalTiles());
            failures++;
        }
    }
    return failures;
}

void testImageProc() {
    vector<path> paths;
    for (auto&& path : directory_iterator(Log::project_path + "TestCaseImages/")) {
        if (path.path().extension() == ".png") {
            paths.push_back(path.path());
        }
    }
    sort(paths.begin(), paths.end(), [](path l, path r){
        return last_write_time(l) > last_write_time(r);
    });
    
    unsigned int num_tests = 0;
    for (auto&& path : paths) {
        cout << num_tests << "/" << paths.size() << " " << path.leaf() << endl;
        unsigned int failures = testImage(path);
        if (failures > 0) {
            
            imwrite(path.string(), imread(path.string())); //Make most recently failed tests run first.
            MYLOG(failures);
        } else {
        }
        num_tests++;

    }
}

void testBoardMovement() {
    SimulatedThreesBoard b = SimulatedThreesBoard::fromString("0,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,0,0,\
                                                               0,0,0,0-1");
    b.moveWithoutAdd(LEFT);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,1,0,0,\
                                                              0,0,0,0,\
                                                              0,0,0,0-1"), {}));
    b.moveWithoutAdd(DOWN);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,0,0,\
                                                              0,1,0,0,\
                                                              0,0,0,0-1"), {}));
    b.moveWithoutAdd(RIGHT);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,0,0,\
                                                              0,0,1,0,\
                                                              0,0,0,0-1"), {}));
    b.moveWithoutAdd(UP);
    debug(!b.hasSameTilesAs(SimulatedThreesBoard::fromString("0,0,0,0,\
                                                              0,0,1,0,\
                                                              0,0,0,0,\
                                                              0,0,0,0-1"), {}));
    
    
    SimulatedThreesBoard x = SimulatedThreesBoard::fromString("6,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,6,0,\
                                                               0,6,0,0-1");
    
    SimulatedThreesBoard y = SimulatedThreesBoard::fromString("3,0,0,0,\
                                                               0,0,1,0,\
                                                               0,0,3,0,\
                                                               0,0,2,0-1");
    debug(!x.hasSameTilesAs(y, {{0,0}, {1,3}, {2,2}, {2,3}}));
}

int main(int argc, const char * argv[]) {
    testBoardMovement();
    //testImageProc(); debug();
    
    for (;;) {
        unique_ptr<GameStateSource> watcher = unique_ptr<GameStateSource>(new QuickTimeSource());
        auto initialState = watcher->getGameState();
        
        auto p = make_shared<RealThreesBoard>("/dev/tty.usbmodem1411", move(watcher), initialState);
        ExpectimaxAI ai(p);
        ai.playGame();
        MYLOG("game over");
    }
    return 0;
}
