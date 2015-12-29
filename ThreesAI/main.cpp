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
#include "UCTSearchAI.hpp"
#include "ManyPlayMonteCarloAI.h"
#include "HumanPlayer.h"
#include "RandomAI.h"

#include "CameraSource.h"
#include "QuickTimeSource.h"
#include "RealBoardOutput.h"
#include "SimulatedBoardOutput.h"
#include "Chromosome.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost;
using namespace cv;
using namespace IMLog;
using namespace IMProc;

unsigned int testImage(path p) {
    HintImages hintImages({
        {Hint(Tile::TILE_48,Tile::TILE_96,Tile::TILE_192), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-48-96-192.png", 0))},
        {Hint(Tile::TILE_24,Tile::TILE_48,Tile::TILE_96), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-24-48-96.png", 0))},
        {Hint(Tile::TILE_12,Tile::TILE_24,Tile::TILE_48), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-12-24-48.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12,Tile::TILE_24), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12-24.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12.png", 0))},
        {Hint(Tile::TILE_6), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6.png", 0))},
    });
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
    pair<BoardStateCPtr, array<MatchResult, 16>> result = IMProc::boardAndMatchFromAnyImage(camImage, HiddenBoardState(0,4,4,4), hintImages);
    if (result.first->getHint() != nextTileHint) {
        MYLOG(nextTileHint);
        MYLOG(result.first->getHint());
        failures++;
        debug();
        IMProc::boardFromAnyImage(camImage, HiddenBoardState(0,4,4,4), hintImages);
    }
    for (BoardIndex i : allIndices) {
        MatchResult extracted = result.second.at(i.toRegularIndex());
        Tile expectedValue = expectedBoard.at(i);
        if (expectedValue != extracted.tile.value) {
            MatchResult expected(IMProc::canonicalTiles().at(expectedValue), tiles[i.toRegularIndex()]);
            vector<Mat> expectedV = {expected.knnDrawing(), expected.ratioPassDrawing(), expected.noDupeDrawing()};
            vector<Mat> extractedV = {extracted.knnDrawing(), extracted.ratioPassDrawing(), extracted.noDupeDrawing()};
            MYSHOW(concatH({concatV(expectedV), concatV(extractedV)}));
            debug();
            IMProc::tileValueFromScreenShot(tiles[i.toRegularIndex()], canonicalTiles());
            auto result = IMProc::boardAndMatchFromAnyImage(camImage, HiddenBoardState(0,4,4,4), hintImages);
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
    ManyPlayMonteCarloAI ai(board->currentState(HiddenBoardState(0,4,4,4)), std::move(board), 2);
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

typedef vector<Chromosome> Population;

vector<pair<BoardState::Score, Chromosome>> getScores(Population p) {
    vector<pair<BoardState::Score, Chromosome>> scores;
    for (Chromosome c : p) {
        auto board = SimulatedBoardOutput::randomBoard(default_random_engine(0));
        BoardStateCPtr initialState = board->currentState(HiddenBoardState(0,1,1,1));
        ExpectimaxAI ai(board->currentState(initialState->hiddenState), std::move(board), c.to_f(), 1);
        ai.playGame(false, false);
        scores.push_back({ai.currentState()->score(), c});
    }
    return scores;
}

int main(int argc, const char * argv[]) {
    //testBoardMovement();\
    testMonteCarloAI();\
    testMoveAndFindIndexes();\
    testImageProc(); debug();
    
    try {
        array<double, CHROMOSOME_SIZE> weights = {2000, 0.5, 1000, -100, 1};
        Chromosome c(weights);
        Heuristic h = c.to_f();
        
        std::shared_ptr<HintImages const> hintImages(new HintImages({
            {Hint(Tile::TILE_48,Tile::TILE_96,Tile::TILE_192), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-48-96-192.jpg", 0))},
            {Hint(Tile::TILE_24,Tile::TILE_48,Tile::TILE_96), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-24-48-96.png", 0))},
            {Hint(Tile::TILE_12,Tile::TILE_24,Tile::TILE_48), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-12-24-48.png", 0))},
            {Hint(Tile::TILE_6,Tile::TILE_12,Tile::TILE_24), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12-24.png", 0))},
            {Hint(Tile::TILE_6,Tile::TILE_12), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12.png", 0))},
            {Hint(Tile::TILE_6), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6.png", 0))},
        }));
        auto watcher = std::shared_ptr<GameStateSource>(new QuickTimeSource(hintImages));
        BoardStateCPtr initialState;
        unique_ptr<BoardOutput> p = unique_ptr<BoardOutput>(new RealBoardOutput("/dev/tty.usbmodem1411", watcher, *initialState, hintImages));
        initialState = watcher->getInitialState();
        ExpectimaxAI ai(p->currentState(initialState->hiddenState), std::move(p), h, 3);

        time_t start = time(nullptr);
        ai.playGame(true);
        time_t end = time(nullptr);
        logGame(ai.currentState()->score(), end - start);
        
        //Prod logs
        initParse("U9Q2piuJY51XQUjQ6MMFnTM3zWLopcTGQEUgiYd8","szQsHJfqz3jZY0DKe1Vpf7jxRPMHABZG6VB9ZJLx");
    } catch (std::exception e) {
        //Debug logs
        initParse("nESS0QMzJcs14BzDBMToQKkeog7mtFkdjGvWHoVT","GCPXJJNG3DXnlsKWsjP3MVlJe52FOVmPDIkVseK0");
    }
    
    //HumanPlayer ai(p->currentState(HiddenBoardState(0,0,0,0)), std::move(p)); bool print = false;
    
    //OnePlayMonteCarloAI ai(p->currentState(), std::move(p)); bool print = false;
    
    //ManyPlayMonteCarloAI ai(p->currentState(), std::move(p), numPlays); bool print = true;
    
    //UCTSearchAI ai(p->currentState(), std::move(p), numPlays); bool print = false;
    
    signed int pop_size = 8;
    
    Population currentGeneration;
    
    for (double i = -pop_size/2; i < pop_size/2; i++) {
        double weight = i*10;
        array<double, CHROMOSOME_SIZE> weights = {weight, weight, weight, weight, weight, weight};
        Chromosome c(weights);
        currentGeneration.push_back(c);
    }

    default_random_engine rng(0);
    
    int generationNumber = 0;
    
    while (generationNumber < 3) {
        generationNumber++;
        vector<pair<BoardState::Score, Chromosome>> scores = getScores(currentGeneration);
        
        sort(scores.begin(), scores.end(), [](pair<BoardState::Score, Chromosome> l, pair<BoardState::Score, Chromosome> r){
            return l.first > r.first;
        });
        
        cout << "Generation #" << generationNumber << endl;
        for (auto scoreChrom : scores) {
            cout << scoreChrom.second << endl << "Fitness: " << scoreChrom.first << endl;
        }
        
        Population next_generation;
        
        next_generation.push_back(scores[0].second);
        next_generation.push_back(scores[0].second.mutate(rng));
        next_generation.push_back(scores[0].second.cross_with(scores[1].second));
        next_generation.push_back(scores[0].second.cross_with(scores[2].second));
        next_generation.push_back(scores[1].second.cross_with(scores[2].second));
        next_generation.push_back(scores[2].second.cross_with(scores[3].second));
        next_generation.push_back(scores[3].second.cross_with(scores[4].second));
        next_generation.push_back(scores[0].second.cross_with(scores[3].second));
        
        currentGeneration = next_generation;
    }

    return 0;
}


