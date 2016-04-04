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

class HiddenBoardState {
public:
    HiddenBoardState(unsigned int numTurns, unsigned int onesInStack, unsigned int twosInStack, unsigned int threesInStack) :
    numTurns(numTurns) {
        debug(onesInStack > 4);
        debug(twosInStack > 4);
        debug(threesInStack > 4);
        this->onesInStack = onesInStack;
        this->twosInStack = twosInStack;
        this->threesInStack = threesInStack;
        if (this->onesInStack == 0 && this->twosInStack == 0 && this->threesInStack == 0) {
            this->onesInStack = 4;
            this->twosInStack = 4;
            this->threesInStack = 4;
        }
    }
    unsigned int stackSize() const {
        return this->onesInStack + this->twosInStack + this->threesInStack;
    }
    unsigned int numTurns;
    unsigned int onesInStack:3;
    unsigned int twosInStack:3;
    unsigned int threesInStack:3;
    
    bool operator==(HiddenBoardState const& other) const;
};


class AdditionInfo {
public:
    Tile t;
    BoardIndex i;
    float probability;
};

class AboutToMoveBoard;

class MoveWithoutAdd {
public:
    MoveWithoutAdd(Direction d) : d(d) {};
    Direction const d;
};

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
    
public:
    cv::Mat preMoveSourceImage;
    
    //TODO: make these non-public
    Board board;
    HiddenBoardState hiddenState;
    EnabledIndices validIndicesForNewTile;
    std::default_random_engine generator;
    bool hasNoHint;
    boost::optional<Tile> upcomingTile;
    boost::optional<Hint> h;
    //End TODO
    
    AboutToAddTileBoard(MoveWithoutAdd d, AboutToMoveBoard const& other);
    AboutToAddTileBoard(SetHint h, AboutToAddTileBoard const& other);
    
    //TODO: remove this. Taking the score of a board that is waiting for a tile to be added doesn't make sense
    BoardScore score() const {return this->board.score();};
    
    Tile at(BoardIndex const& p) const {return this->board.at(p);};
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    Hint getHint() const;
    HiddenBoardState nextHiddenState(boost::optional<Tile> mostRecentlyAddedTile) const;
    BoardIndex indexForNextTile();
    Tile genUpcomingTile() const;
    
    std::deque<AdditionInfo> possibleAdditions() const;
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
    
    class FromString {
    public:
        FromString(std::string s) : s(s) {};
        std::string const s;
    };
    
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
    
    class MoveWithAdd {
    public:
        MoveWithAdd(Direction d) : d(d) {};
        Direction const d;
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
    
    AboutToMoveBoard(FromString s);
    AboutToMoveBoard(AddSpecificTile s, AboutToAddTileBoard const & b, bool hasNoHint);
    AboutToMoveBoard(DifferentFuture, AboutToMoveBoard const& other);
    AboutToMoveBoard(MoveWithAdd m, AboutToMoveBoard const& other);
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
    
    Tile at(BoardIndex const& p) const {return this->board.at(p);};
    Hint getHint() const;
    BoardScore score() const {return this->board.score();};
    SearchResult heuristicSearchIfMovedInDirection(Direction d, uint8_t depth, std::shared_ptr<Heuristic> h) const;
    
    bool isMoveValid(Direction d) const {return this->board.isMoveValid(d);};
    bool isGameOver() const;
    
    Direction randomValidMoveFromInternalGenerator() const;
    BoardScore runRandomSimulation(unsigned int simNumber) const;
    HiddenBoardState nextHiddenState(boost::optional<Tile> mostRecentlyAddedTile) const;
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
    
    AboutToAddTileBoard moveWithoutAdd(Direction d) const;
    
    friend std::ostream& operator<<(std::ostream &os, AboutToMoveBoard const& info);
    
    void takeTurnInPlace(Direction d); //exposed to make monte carlo ai faster
};

typedef std::shared_ptr<AboutToMoveBoard const> BoardStateCPtr;

std::ostream& operator<<(std::ostream &os, const BoardIndex e);
std::ostream& operator<<(std::ostream &os, AboutToMoveBoard const& info);

#endif /* defined(__ThreesAI__BoardState__) */
