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
    BoardState expectedBoard(BoardState::FromString(p.stem().string()));
    
    vector<string> splitName;
    split(splitName, p.stem().string(), is_any_of("-"));
    deque<string> nextTileHintStrings;
    split(nextTileHintStrings, splitName[1], is_any_of(","));
    debug(nextTileHintStrings.size() > 3);
    
    Tile tile1 = tileFromString(nextTileHintStrings[0]);
    Tile tile2 = nextTileHintStrings.size() > 1 ? tileFromString(nextTileHintStrings[1]) : Tile::EMPTY;
    Tile tile3 = nextTileHintStrings.size() > 2 ? tileFromString(nextTileHintStrings[2]) : Tile::EMPTY;
    Hint nextTileHint(tile1, tile2, tile3);
    
    Mat camImage = imread(p.string());
    array<Mat, 16> tiles;
    if (camImage.rows == 2272 && camImage.cols == 1280) {
        tiles = tilesFromScreenImage(camImage);
    } else {
        tiles = tilesFromScreenImage(IMProc::screenImage(camImage));
    }
    pair<std::shared_ptr<BoardState const>, array<MatchResult, 16>> result = IMProc::boardAndMatchFromAnyImage(camImage);
    if (result.first->getHint() != nextTileHint) {
        MYLOG(nextTileHint);
        MYLOG(result.first->getHint());
        failures++; 
        debug();
        IMProc::boardFromAnyImage(camImage);
    }
    for (unsigned char i = 0; i < 16; i++) {
        MatchResult extracted = result.second.at(i);
        Tile expectedValue = expectedBoard.at({i%4,i/4});
        if (expectedValue != extracted.tile.value) {
            MatchResult expected(IMProc::canonicalTiles().at(expectedValue), tiles[i]);
            vector<Mat> expectedV = {expected.knnDrawing(), expected.ratioPassDrawing(), expected.noDupeDrawing()};
            vector<Mat> extractedV = {extracted.knnDrawing(), extracted.ratioPassDrawing(), extracted.noDupeDrawing()};
            MYSHOW(concatH({concatV(expectedV), concatV(extractedV)}));
            debug();
            IMProc::tileValueFromScreenShot(tiles[i], canonicalTiles());
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
    BoardState b1(BoardState::FromString("0,0,1,1,\
                                            0,0,1,1,\
                                            0,0,0,0,\
                                            0,0,0,0-1"));
    BoardState b2(BoardState::MoveWithoutAdd(Direction::LEFT), b1);
    debug(!b2.hasSameTilesAs(BoardState::FromString("0,1,1,0,\
                                                     0,1,1,0,\
                                                     0,0,0,0,\
                                                     0,0,0,0-1"), {}));
    BoardState b3(BoardState::MoveWithoutAdd(Direction::DOWN), b2);
    debug(!b3.hasSameTilesAs(BoardState::FromString("0,0,0,0,\
                                                     0,1,1,0,\
                                                     0,1,1,0,\
                                                     0,0,0,0-1"), {}));
    BoardState b4(BoardState::MoveWithoutAdd(Direction::RIGHT), b3);
    debug(!b4.hasSameTilesAs(BoardState::FromString("0,0,0,0,\
                                                     0,0,1,1,\
                                                     0,0,1,1,\
                                                     0,0,0,0-1"), {}));
    BoardState b5(BoardState::MoveWithoutAdd(Direction::UP), b4);
    debug(!b5.hasSameTilesAs(BoardState::FromString("0,0,1,1,\
                                                     0,0,1,1,\
                                                     0,0,0,0,\
                                                     0,0,0,0-1"), {}));
    
    
    BoardState x(BoardState::FromString("6,0,0,0,\
                                           0,0,1,0,\
                                           0,0,6,0,\
                                           0,6,0,0-1"));
    
    BoardState y(BoardState::FromString("3,0,0,0,\
                                           0,0,1,0,\
                                           0,0,3,0,\
                                           0,0,2,0-1"));
    debug(!x.hasSameTilesAs(y, {{0,0}, {1,3}, {2,2}, {2,3}}));
}

void testMonteCarloAI() {
    std::unique_ptr<SimulatedBoardOutput> board = std::unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(make_shared<BoardState const>(BoardState::FromString("2,6,3,1,\
                                                                                                        3,24,384,6,\
                                                                                                        6,24,96,192,\
                                                                                                        3,6,1,3-2"))));
    ManyPlayMonteCarloAI ai(board->currentState(), std::move(board), 2);
    debug(ai.getDirection() == Direction::DOWN);
    ai.getDirection();
}

void testMoveAndFindIndexes() {
    BoardState preMove(BoardState::FromString("0,0,1,0,\
                                                 0,0,3,0,\
                                                 3,0,24,0,\
                                                 6,3,2,1-2"));
    BoardState postMove(BoardState::MoveWithoutAdd(Direction::LEFT), preMove);
    BoardState expected(BoardState::FromString("0,1,0,0,\
                                                 0,3,0,0,\
                                                 3,24,0,0,\
                                                 6,3,3,0-3"));
    debug(!postMove.hasSameTilesAs(expected, {}));
}

int main(int argc, const char * argv[]) {
    //testBoardMovement();
    //testMonteCarloAI();
    //testMoveAndFindIndexes();
    //testImageProc(); debug();
    
    
    for (int i = 0; i < 1; i++) {
        unique_ptr<BoardOutput> p = SimulatedBoardOutput::randomBoard();
        auto watcher = std::shared_ptr<GameStateSource>(new QuickTimeSource());\
        //auto initialState = watcher->getGameState();\
        unique_ptr<BoardOutput> p = unique_ptr<BoardOutput>(new RealBoardOutput("/dev/tty.usbmodem1411", watcher, initialState));
        HumanPlayer ai(p->currentState(), std::move(p)); bool print = false;
        //ManyPlayMonteCarloAI ai(p->currentState(), std::move(p), 100); bool print = true;
        ai.playGame(print); //Passed bool used to print move.
        MYLOG(ai.currentState());
    }
    return 0;
}
