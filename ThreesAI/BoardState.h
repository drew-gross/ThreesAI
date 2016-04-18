//
//  BoardState.h
//  ThreesAI
//
//  Created by Drew Gross on 9/11/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__BoardState__
#define __ThreesAI__BoardState__

#include <vector>
#include <random>
#include <array>
#include <deque>

#include "Direction.h"
#include "Hint.h"
#include "EnabledIndices.hpp"
#include "EnabledDirections.hpp"
#include "Board.hpp"
#include "AddedTileInfo.h"
#include "UpcomingTileGenerator.hpp"

#include <opencv2/opencv.hpp>

#include <boost/core/noncopyable.hpp>

#include "Debug.h"
#include <functional>

class Heuristic;

class SearchResult {
public:
    float value;
    unsigned int openNodes;
};

class AddedTileInfoWithProbability {
public:
    AddedTileInfo i;
    float probability;
};

class AboutToMoveBoard;

class AddTile {
public:
    AddTile(Direction d) : d(d) {};
    Direction const d;
};

class SetHint {
public:
    SetHint(Hint h) : h(h) {};
    Hint h;
};

class AboutToAddTileBoard {
    UpcomingTileGenerator upcomingTileGenerator;
    
public:
    cv::Mat preMoveSourceImage;
    
    //TODO: make these non-public
    Board board;
    HiddenBoardState hiddenState;
    EnabledIndices validIndicesForNewTile;
    std::default_random_engine generator;
    //End TODO
    
    AboutToAddTileBoard(SetHint h, AboutToAddTileBoard const& other);
    AboutToAddTileBoard(Board b, EnabledIndices i, HiddenBoardState h, std::default_random_engine g, Hint hint);
    
    //TODO: remove this. Taking the score of a board that is waiting for a tile to be added doesn't make sense
    BoardScore score() const {return this->board.score();};
    
    Tile at(BoardIndex const& p) const {return this->board.at(p);};
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    BoardIndex indexForNextTile();
    Tile genUpcomingTile() const;
    AboutToMoveBoard addSpecificTile(AddedTileInfo tile) const;
    
    std::deque<AddedTileInfoWithProbability> possibleAdditionsWithProbability() const;
    std::deque<AddedTileInfo> possibleAdditions() const;
};

class AboutToMoveBoard {
    boost::optional<Tile> upcomingTile;
    void addTile(Direction d); //Exists to make takeTurnInPlace possible
public:
    Board board;// TODO: make this private
    
    cv::Mat sourceImage;
    bool hasNoHint;
    HiddenBoardState hiddenState;
    boost::optional<Hint> hint;
    std::default_random_engine generator;
    
    class AddSpecificTile {
    public:
        AddSpecificTile(Direction d, BoardIndex const& i, const Tile t) : d(d), i(i), t(t) {};
        Direction const d;
        BoardIndex const i;
        Tile const t;
    };
    
    class DifferentFuture {
    public:
        DifferentFuture(unsigned int howDifferent) : howDifferent(howDifferent) {};
        unsigned int howDifferent;
    };
    
    class SetHiddenState {
    public:
        SetHiddenState(HiddenBoardState h) : h(h) {};
        HiddenBoardState h;
    };
    
    class SetHint {
    public:
        SetHint(Hint h) : h(h) {};
        Hint h;
    };
    
    AboutToMoveBoard(Board b, HiddenBoardState h);
    
    AboutToMoveBoard(std::string s);
    AboutToMoveBoard(DifferentFuture, AboutToMoveBoard const& other);
    AboutToMoveBoard(SetHiddenState h, AboutToMoveBoard const& other);
    AboutToMoveBoard(SetHint h, AboutToMoveBoard const& other);
    AboutToMoveBoard(AddTile t, AboutToAddTileBoard const& other);
    AboutToMoveBoard(Board b,
               std::default_random_engine hintGen,
               cv::Mat sourceImage);
    
    AboutToMoveBoard(Board b,
               HiddenBoardState h,
               std::default_random_engine hintGen,
               cv::Mat sourceImage);
    
    AboutToMoveBoard(Board b,
               HiddenBoardState h,
               std::default_random_engine gen,
               Hint hint,
               cv::Mat sourceImage);
    
    AboutToMoveBoard moveWithAdd(Direction d) const;
    
    Tile at(BoardIndex const& p) const {return this->board.at(p);};
    Hint getHint(bool exploratory = false) const;
    BoardScore score() const {return this->board.score();};
    SearchResult heuristicSearchIfMovedInDirection(Direction d, uint8_t depth, std::shared_ptr<Heuristic> h) const;
    
    bool isMoveValid(Direction d) const {return this->board.isMoveValid(d);};
    bool isGameOver() const;
    
    Direction randomValidMoveFromInternalGenerator() const;
    BoardScore runRandomSimulation(unsigned int simNumber) const;
    long countOfTile(Tile t) const;
    bool hasSameTilesAs(AboutToAddTileBoard const& otherBoard) const;
    bool hasSameTilesAs(AboutToMoveBoard const& otherBoard) const;
    bool canMove(Direction d) const;
    unsigned long adjacentPairCount() const;
    unsigned long adjacentOffByOneCount() const;
    unsigned long splitPairCount() const;
    unsigned long splitPairsOfTile(Tile t) const;
    unsigned long trappedTileCount() const;
    Tile getUpcomingTile() const;
    Tile genUpcomingTile() const;
    
    AboutToAddTileBoard moveWithoutAdd(Direction d, bool exploratory) const;
    
    friend std::ostream& operator<<(std::ostream &os, AboutToMoveBoard const& info);
    
    void takeTurnInPlace(Direction d); //exposed to make monte carlo ai faster
};

typedef std::shared_ptr<AboutToMoveBoard const> BoardStateCPtr;

std::ostream& operator<<(std::ostream &os, const BoardIndex e);
std::ostream& operator<<(std::ostream &os, AboutToMoveBoard const& info);

#endif /* defined(__ThreesAI__BoardState__) */
