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

#include <opencv2/opencv.hpp>

#include <boost/core/noncopyable.hpp>

#include "Debug.h"

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

class BoardState : boost::noncopyable {
public:
    typedef unsigned long Score;
    
    class AddSpecificTile {
    public:
        AddSpecificTile(Direction d, BoardIndex const& i, const Tile t) : d(d), i(i), t(t) {};
        Direction const d;
        BoardIndex const i;
        Tile const t;
    };
    
    class AddTile {
    public:
        AddTile(Direction d) : d(d) {};
        Direction const d;
    };
    
    class MoveWithoutAdd {
    public:
        MoveWithoutAdd(Direction d) : d(d) {};
        Direction const d;
    };
    
    class MoveWithAdd {
    public:
        MoveWithAdd(Direction d) : d(d) {};
        Direction const d;
    };
    
    class FromString {
    public:
        FromString(std::string s) : s(s) {};
        std::string const s;
    };
    
    class SetHint {
    public:
        SetHint(Hint h) : h(h) {};
        Hint h;
    };
    
    class SetHiddenState {
    public:
        SetHiddenState(HiddenBoardState h) : h(h) {};
        HiddenBoardState h;
    };
    
    class DifferentFuture {
    public:
        DifferentFuture(unsigned int howDifferent) : howDifferent(howDifferent) {};
        unsigned int howDifferent;
    };
    
    typedef std::array<Tile, 16> Board;
    
    BoardState(AddSpecificTile t, BoardState const& other, bool hasNoHint);
    BoardState(MoveWithoutAdd d, BoardState const& other);
    BoardState(MoveWithAdd m, BoardState const& other);
    BoardState(AddTile t, BoardState const& other);
    BoardState(FromString s);
    BoardState(DifferentFuture, BoardState const& other);
    BoardState(SetHint h, BoardState const& other);
    BoardState(SetHiddenState h, BoardState const& other);
    
    BoardState(Board b,
               std::default_random_engine hintGen,
               cv::Mat sourceImage);
    
    BoardState(Board b,
               HiddenBoardState h,
               std::default_random_engine hintGen,
               cv::Mat sourceImage);
    
    BoardState(Board b,
               HiddenBoardState h,
               std::default_random_engine gen,
               Hint hint,
               cv::Mat sourceImage);
    
    HiddenBoardState hiddenState;
    
    Tile at(BoardIndex const& p) const;
    long countOfTile(Tile t) const;
    
    bool isGameOver() const;
    bool isMoveValid(Direction d) const;
    Direction randomValidMoveFromInternalGenerator() const;
    bool canMove(Direction d) const;
    
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    EnabledIndices validIndicesForNewTile(Direction movedDirection) const;
    
    Score score() const;
    bool hasSameTilesAs(BoardState const& otherBoard, EnabledIndices excludedIndices) const;
    
    friend std::ostream& operator<<(std::ostream &os, BoardState const& info);
    Hint getHint() const;
    
    cv::Mat sourceImage;
    
    Score runRandomSimulation(unsigned int simNumber) const;
    HiddenBoardState nextHiddenState(boost::optional<Tile> mostRecentlyAddedTile) const;
    
    bool hasNoHint;
public:
    void takeTurnInPlace(Direction d); //exposed to make monte carlo ai faster
private:
    //mutator methods, should only be called inside constructors
    void set(BoardIndex i, Tile t);
    BoardIndex indexForNextTile(Direction d);
    void copy(BoardState const &other);
    void addTile(Direction d);
    void move(Direction d);

    //non-mutators that aren't used extarnally
    EnabledDirections validMoves() const;
    Tile genUpcomingTile() const;
    Tile maxBonusTile() const;
    Tile maxTile() const;
    float nonBonusTileProbability(Tile tile, bool canHaveBonus) const;
    Tile getUpcomingTile() const;
    
    mutable Tile maxTileCache = Tile::TILE_3;
    mutable unsigned int scoreCache;
    mutable bool scoreCacheIsValid = false;
    mutable bool validMovesCacheIsValid = false;
    mutable EnabledDirections validMovesCache;
    std::default_random_engine generator;
    
    Board board;
    boost::optional<Tile> upcomingTile;
    boost::optional<Hint> hint;
};

typedef std::shared_ptr<BoardState const> BoardStateCPtr;

std::ostream& operator<<(std::ostream &os, const BoardIndex e);
std::ostream& operator<<(std::ostream &os, BoardState const& info);

#endif /* defined(__ThreesAI__BoardState__) */
