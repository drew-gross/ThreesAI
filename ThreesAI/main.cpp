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
#include "Evaluators.hpp"

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

void getToGame(std::shared_ptr<HintImages const> hintImages) {
    default_random_engine fake;
    RealBoardOutput initializer("/dev/tty.usbmodem1411", std::shared_ptr<GameStateSource>(new QuickTimeSource(hintImages)), *SimulatedBoardOutput::randomBoard(fake)->sneakyState(), hintImages);
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
    //testImageProc();\
    debug();
    
    std::shared_ptr<HintImages const> hintImages(new HintImages({
        {Hint(Tile::TILE_48,Tile::TILE_96,Tile::TILE_192), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-48-96-192.jpg", 0))},
        {Hint(Tile::TILE_24,Tile::TILE_48,Tile::TILE_96), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-24-48-96.png", 0))},
        {Hint(Tile::TILE_12,Tile::TILE_24,Tile::TILE_48), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-12-24-48.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12,Tile::TILE_24), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12-24.png", 0))},
        {Hint(Tile::TILE_6,Tile::TILE_12), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6-12.png", 0))},
        {Hint(Tile::TILE_6), screenImageToBonusHintImage(imread("/Users/drewgross/Projects/ThreesAI/SampleData/Hint-6.png", 0))},
    }));
    vector<FuncAndWeight> currentWeights = {
        {countEmptyTile, 2},
        {score, 0.000005},
        {countAdjacentPair, 2},
        {countSplitPair, -1},
        {simScore, 0.00001},
        {countAdjacentOffByOne, 1},
        {countTrappedTiles, -5},
        {highestIsInCorner, 10},
        {highestIsOnEdge, 5},
    };
    vector<BoardEvaluator> currentFuncs;
    currentFuncs.resize(currentWeights.size());
    transform(currentWeights.begin(), currentWeights.end(), currentFuncs.begin(), [](FuncAndWeight const& b){return b.first;});
    Chromosome c(currentWeights); //Must not get destroyed
    Heuristic h = c.to_f();
    
    initAndPlayIfPossible(hintImages, Chromosome(currentWeights));
    
    bool playOneGame = false;
    playOneGame = true;
    bool trulyRandom = false;
    trulyRandom = true;
    bool play10Games = false;
    play10Games = true;
    
    random_device trueRandom;
    default_random_engine seededEngine(trulyRandom ? trueRandom() : 0);
    
    if (play10Games) {
        time_t start = time(nullptr);
        unsigned long totalScore = 0;
        for (int i = 0; i < 10; i++) {
            unique_ptr<BoardOutput> trulyRandomBoard = SimulatedBoardOutput::randomBoard(seededEngine);
            AdaptiveDepthAI ai(trulyRandomBoard->sneakyState(), std::move(trulyRandomBoard), h, 500);
            ai.playGame(false, false);
            totalScore += ai.currentState()->score();
            cout << "Score " << i << ": " << ai.currentState()->score() << endl;
        }
        time_t end = time(nullptr);
        cout << "Final score: " << totalScore/10 << endl;
        cout << "Time taken: " << end - start << "s" << endl;
        exit(0);
    }
    
    
    unique_ptr<BoardOutput> trulyRandomBoard = SimulatedBoardOutput::randomBoard(seededEngine);
    
    if (playOneGame) {
        AdaptiveDepthAI ai(trulyRandomBoard->sneakyState(), std::move(trulyRandomBoard), h, 1000);
        time_t start = time(nullptr);
        ai.playGame(true, false);
        time_t end = time(nullptr);
        cout << "Final score: " << ai.currentState()->score() << endl;
        cout << "Time taken: " << end - start << "s" << endl;
        exit(0);
    }
    
    signed int pop_size = 8;
    std::vector<Chromosome> p;
    
    default_random_engine initial_population_generator;
    normal_distribution<float> population_dist(0,5);
    for (int i = 0; i < pop_size; i++) {
        vector<FuncAndWeight> weights;
        for (auto&& func : currentFuncs) {
            weights.push_back({func, population_dist(initial_population_generator)});
        }
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
            next_generation.push_back(candidate);
            next_generation.emplace_back(Chromosome::Mutate(), candidate, rng);
        }
        
        next_generation.emplace_back(currentGeneration.get(0));
        
        currentGeneration = Population(next_generation);
    }

    return 0;
}


