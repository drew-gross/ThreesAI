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

#include "ZeroDepthAI.h"
#include "ExpectimaxAI.h"
#include "OnePlayMonteCarloAI.h"
#include "UCTSearchAI.hpp"
#include "ManyPlayMonteCarloAI.h"
#include "HumanPlayer.h"
#include "RandomAI.h"
#include "FixedDepthAI.hpp"
#include "AdaptiveDepthAI.hpp"

#include "CameraSource.h"
#include "QuickTimeSource.h"
#include "RealBoardOutput.h"
#include "SimulatedBoardOutput.h"
#include "Chromosome.hpp"
#include "Population.hpp"

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

void getToGame(std::shared_ptr<HintImages const> hintImages) {
    RealBoardOutput initializer("/dev/tty.usbmodem1411", std::shared_ptr<GameStateSource>(new QuickTimeSource(hintImages)), *SimulatedBoardOutput::randomBoard(default_random_engine())->sneakyState(), hintImages);
    while (IMProc::isInOutOfMovesState(getMostRecentFrame())) {
        cout << "Getting through out of moves screen" << endl;
        initializer.moveStepper(Direction::DOWN);
    }
    sleep(10);
    while (IMProc::isInSwipeToSaveState(getMostRecentFrame())) {
        cout << "Getting through swipe to save screen" << endl;
        initializer.moveStepper(Direction::UP);
    }
    sleep(10);
    while (IMProc::isInRetryState(getMostRecentFrame())) {
        cout << "Getting through retry screen" << endl;
        initializer.pressWithServo();
    }
}

void initAndPlayIfPossible(std::shared_ptr<HintImages const> hintImages, Chromosome c) {
    try {
        getToGame(hintImages);
        
        //Prod logs
        initParse("U9Q2piuJY51XQUjQ6MMFnTM3zWLopcTGQEUgiYd8","szQsHJfqz3jZY0DKe1Vpf7jxRPMHABZG6VB9ZJLx");
        auto watcher = std::shared_ptr<GameStateSource>(new QuickTimeSource(hintImages));
        BoardStateCPtr initialState;
        unique_ptr<BoardOutput> p = unique_ptr<BoardOutput>(new RealBoardOutput("/dev/tty.usbmodem1411", watcher, *initialState, hintImages));
        initialState = watcher->getInitialState();
        AdaptiveDepthAI ai(p->currentState(initialState->hiddenState), std::move(p), c.to_f(), 1000000);
        
        time_t start = time(nullptr);
        ai.playGame(true);
        time_t end = time(nullptr);
        logGame(ai.currentState()->score(), end - start);
        exit(0);
        
    } catch (std::exception e) {
        //Debug logs
        initParse("nESS0QMzJcs14BzDBMToQKkeog7mtFkdjGvWHoVT","GCPXJJNG3DXnlsKWsjP3MVlJe52FOVmPDIkVseK0");
    }
}

int main(int argc, const char * argv[]) {
    //testBoardMovement();\
    testMonteCarloAI();\
    testMoveAndFindIndexes();\
    testImageProc();\
    debug();
    
    std::shared_ptr<HintImages const> hintImages(new HintImages({
        {Hint(Tile::TILE_48,Tile::TILE_96,Tile::TILE_192), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-48-96-192.jpg", 0))},
        {Hint(Tile::TILE_24,Tile::TILE_48,Tile::TILE_96), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-24-48-96.png", 0))},
        {Hint(Tile::TILE_12,Tile::TILE_24,Tile::TILE_48), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-12-24-48.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12,Tile::TILE_24), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12-24.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12.png", 0))},
        {Hint(Tile::TILE_6), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6.png", 0))},
    }));
    
    array<float, CHROMOSOME_SIZE> currentWeights = {8.65, 18.0, -9.62, -12.3, 3.63, -7.62};
//    currentWeights = {1,0,0,0,0,0}; //Empty count only;
//    currentWeights = {0,1,0,0,0,0}; //Score only;
//    currentWeights = {0,0,1,0,0,0}; //Adjacent pairs only;
//    currentWeights = {0,0,0,1,0,0}; //Split pairs only;
    currentWeights = {0,0,0,0,1,0}; //Single sim only;
//    currentWeights = {0,0,0,0,0,1}; //Adjacent off by one only;
    
    initAndPlayIfPossible(hintImages, Chromosome(currentWeights));
    
    bool playOneGame = false;
    playOneGame = true;
    bool trulyRandom = false;
    trulyRandom = true;
    
    random_device trueRandom;
    default_random_engine seededEngine(trulyRandom ? trueRandom() : 0);
    
    unique_ptr<BoardOutput> trulyRandomBoard = SimulatedBoardOutput::randomBoard(seededEngine);
    
    if (playOneGame) {
        AdaptiveDepthAI ai(trulyRandomBoard->sneakyState(), std::move(trulyRandomBoard), Chromosome(currentWeights).to_f(), 20000);
        ai.playGame(true, false);
        exit(0);
    }
    
    signed int pop_size = 8;
    std::vector<Chromosome> p;
    
    default_random_engine initial_population_generator;
    normal_distribution<float> population_dist(1,5);
    for (int i = 0; i < pop_size; i++) {
        array<float, CHROMOSOME_SIZE> weights = {
            population_dist(initial_population_generator),
            population_dist(initial_population_generator),
            population_dist(initial_population_generator),
            population_dist(initial_population_generator),
            population_dist(initial_population_generator),
            population_dist(initial_population_generator)
        };
        p.emplace_back(weights);
    }
    
    Population currentGeneration(p);
    
    default_random_engine rng(trueRandom());
    
    int generationNumber = 0;
    
    while (true) {
        generationNumber++;
        default_random_engine prng(0);
        currentGeneration.populateScoresAndSort(5, prng);
        
        cout << "Generation #" << generationNumber << endl;
        cout << currentGeneration << endl;
        
        vector<Chromosome> next_generation;
        
        for (int i = 0; i < pop_size / 2; i++) {
            
            Chromosome candidate = currentGeneration.cross(i, pop_size/2 - 1, rng);
            bool nextGenAlreadyHasCandidate = find_if(next_generation.cbegin(), next_generation.cend(), [&next_generation](Chromosome const& c){
                for (auto&& existing : next_generation) {
                    if (existing.weights == c.weights) {
                        return true;
                    }
                };
                return false;
            }) == next_generation.end();
            
            if (!nextGenAlreadyHasCandidate) {
                next_generation.push_back(candidate);
            }
            
            next_generation.emplace_back(Chromosome::Mutate(), candidate, rng);
        }
        
        next_generation.emplace_back(currentGeneration.get(0));
        
        currentGeneration = Population(next_generation);
    }

    return 0;
}


