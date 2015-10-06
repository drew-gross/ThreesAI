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

#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "OnePlayMonteCarloAI.h"
#include "ManyPlayMonteCarloAI.h"
#include "HumanPlayer.h"
#include "RandomAI.h"

#include "CameraSource.h"
#include "QuickTimeSource.h"
#include "RealBoardOutput.h"
#include "SimulatedBoardOutput.h"

#include "ForcedHint.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost;
using namespace cv;
using namespace IMLog;
using namespace IMProc;

unsigned int testImage(path p) {
    unsigned int failures = 0;
    BoardState expectedBoard = BoardState::fromString(p.stem().string());
    
    vector<string> splitName;
    split(splitName, p.stem().string(), is_any_of("-"));
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    ForcedHint nextTileHint(stoi(nextTileHintStrings[0]), nextTileHintStrings.size() > 1 ? stoi(nextTileHintStrings[1]) : 0, nextTileHintStrings.size() > 2 ? stoi(nextTileHintStrings[2]) : 0);
    
    Mat camImage = imread(p.string());
    array<Mat, 16> tiles;
    if (camImage.rows == 2272 && camImage.cols == 1280) {
        tiles = tilesFromScreenImage(camImage);
    } else {
        tiles = tilesFromScreenImage(IMProc::screenImage(camImage));
    }
    auto result = IMProc::boardAndMatchFromAnyImage(camImage);
    if (result.first.getHint()->operator!=(nextTileHint) && nextTileHint.isNonBonus()) {
        MYLOG(nextTileHint);
        MYLOG(result.first.getHint());
        failures++; 
        //debug();
        IMProc::boardFromAnyImage(camImage);
    }
    for (unsigned char i = 0; i < 16; i++) {
        MatchResult extracted = result.second.at(i);
        int expectedValue = expectedBoard.at({i%4,i/4});
        if (expectedValue != extracted.tile.value) {
            MatchResult expected(IMProc::canonicalTiles().at(expectedValue), tiles[i]);
            vector<Mat> expectedV = {expected.knnDrawing(), expected.ratioPassDrawing(), expected.noDupeDrawing()};
            vector<Mat> extractedV = {extracted.knnDrawing(), extracted.ratioPassDrawing(), extracted.noDupeDrawing()};
            MYSHOW(concatH({concatV(expectedV), concatV(extractedV)}));
            debug();
            IMProc::tileValue(tiles[i], canonicalTiles());
            auto result = IMProc::boardAndMatchFromAnyImage(camImage);
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
    BoardState b1 = BoardState::fromString("0,0,1,1,\
                                            0,0,1,1,\
                                            0,0,0,0,\
                                            0,0,0,0-1");
    auto b2 = b1.moveWithoutAdd(LEFT);
    debug(!b2.hasSameTilesAs(BoardState::fromString("0,1,1,0,\
                                                     0,1,1,0,\
                                                     0,0,0,0,\
                                                     0,0,0,0-1"), {}));
    auto b3 = b2.moveWithoutAdd(DOWN);
    debug(!b3.hasSameTilesAs(BoardState::fromString("0,0,0,0,\
                                                     0,1,1,0,\
                                                     0,1,1,0,\
                                                     0,0,0,0-1"), {}));
    auto b4 = b3.moveWithoutAdd(RIGHT);
    debug(!b4.hasSameTilesAs(BoardState::fromString("0,0,0,0,\
                                                     0,0,1,1,\
                                                     0,0,1,1,\
                                                     0,0,0,0-1"), {}));
    auto b5 = b4.moveWithoutAdd(UP);
    debug(!b5.hasSameTilesAs(BoardState::fromString("0,0,1,1,\
                                                     0,0,1,1,\
                                                     0,0,0,0,\
                                                     0,0,0,0-1"), {}));
    
    
    BoardState x = BoardState::fromString("6,0,0,0,\
                                           0,0,1,0,\
                                           0,0,6,0,\
                                           0,6,0,0-1");
    
    BoardState y = BoardState::fromString("3,0,0,0,\
                                           0,0,1,0,\
                                           0,0,3,0,\
                                           0,0,2,0-1");
    debug(!x.hasSameTilesAs(y, {{0,0}, {1,3}, {2,2}, {2,3}}));
}

int main(int argc, const char * argv[]) {
    testBoardMovement();
    testImageProc(); debug();
    
    for (int i = 0; i < 1; i++) {
        //unique_ptr<BoardOutput> p = SimulatedBoardOutput::randomBoard();
        auto watcher = std::shared_ptr<GameStateSource>(new QuickTimeSource());\
        auto initialState = watcher->getGameState();\
        unique_ptr<BoardOutput> p = unique_ptr<BoardOutput>(new RealBoardOutput("/dev/tty.usbmodem1411", watcher, initialState));
        //HumanPlayer ai(p->currentState(), move(p));
        ManyPlayMonteCarloAI ai(p->currentState(), move(p), 100);
        ai.playGame(true);
        MYLOG(ai.currentState());
    }
    return 0;
}
