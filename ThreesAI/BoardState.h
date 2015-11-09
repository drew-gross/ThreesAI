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

#include <opencv2/opencv.hpp>

#include <boost/core/noncopyable.hpp>

class BoardState : boost::noncopyable {
public:
    
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
    
    class Move {
    public:
        Move(Direction d) : d(d) {};
        Direction const d;
    };
    
    class FromString {
    public:
        FromString(std::string s) : s(s) {};
        std::string const s;
    };
    
    class DifferentFuture {
    public:
        DifferentFuture(unsigned int howDifferent) : howDifferent(howDifferent) {};
        unsigned int howDifferent;
    };
    
    typedef std::array<Tile, 16> Board;
    
    BoardState(AddSpecificTile t, BoardState const& other);
    BoardState(MoveWithoutAdd d, BoardState const& other);
    BoardState(Move m, BoardState const& other);
    BoardState(AddTile t, BoardState const& other);
    BoardState(FromString s);
    BoardState(DifferentFuture, BoardState const& other);
    
    BoardState(Board b,
               std::default_random_engine hintGen,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    BoardState(Board b,
               std::default_random_engine gen,
               Hint hint,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    
    unsigned int numTurns;
    
    Tile at(BoardIndex const& p) const;
    
    bool isGameOver() const;
    
    std::vector<Direction> validMoves() const;
    Direction randomValidMoveFromInternalGenerator() const;
    bool canMove(Direction d) const;
    
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    EnabledIndices validIndicesForNewTile(Direction movedDirection) const;
    
    unsigned int score() const;
    bool hasSameTilesAs(BoardState const& otherBoard, EnabledIndices excludedIndices) const;
    
    friend std::ostream& operator<<(std::ostream &os, BoardState const& info);
    Hint getHint() const;
    
    cv::Mat sourceImage;
public:
    void takeTurnInPlace(Direction d); //exposed to make monte carlo ai faster
private:
    //mutator methods, should only be called inside constructors
    void set(BoardIndex i, Tile t);
    BoardIndex indexForNextTile(Direction d);
    void removeFromStack(Tile t);
    void copy(BoardState const &other);
    void addTile(Direction d);
    void move(Direction d);//making public so MonteCarlo doesn't have to copy state around a lot

    //non-mutators that aren't used extarnally
    Tile genUpcomingTile() const;
    unsigned int stackSize() const;
    Tile maxBonusTile() const;
    Tile maxTile() const;
    float nonBonusTileProbability(Tile tile, bool canHaveBonus) const;
    Tile getUpcomingTile();
    
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid = false;
    mutable unsigned int scoreCache;
    mutable bool scoreCacheIsValid = false;
    mutable bool validMovesCacheIsValid = false;
    mutable std::vector<Direction> validMovesCache;
    std::default_random_engine generator;
    
    unsigned int onesInStack;
    unsigned int twosInStack;
    unsigned int threesInStack;
    Board board;
    boost::optional<Tile> upcomingTile;
    boost::optional<Hint> hint;
};

std::ostream& operator<<(std::ostream &os, const BoardIndex e);
std::ostream& operator<<(std::ostream &os, BoardState const& info);

#endif /* defined(__ThreesAI__BoardState__) */
