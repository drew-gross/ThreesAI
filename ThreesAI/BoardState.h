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

#include <opencv2/opencv.hpp>

#include <boost/core/noncopyable.hpp>

class BoardState : boost::noncopyable {
public:
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    
    enum class CopyType {
        RAW,
        WITH_DIFFERENT_FUTURE,
    };
    
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
    
    static std::array<BoardIndex, 16> indexes();
    
    typedef std::array<Tile, 16> Board;
    
    BoardState(AddSpecificTile t, BoardState const& other);
    BoardState(MoveWithoutAdd d, BoardState const& other);
    BoardState(Move m, BoardState const& other);
    BoardState(AddTile t, BoardState const& other);
    BoardState(FromString s);
    
    /*BoardState(Board b,
               Tile upcomingTile,
               std::default_random_engine gen,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    */BoardState(Board b,
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
    
    BoardState(CopyType, BoardState const& b);
    
    unsigned int numTurns;
    
    Tile at(BoardIndex const& p) const;
    
    bool isGameOver() const;
    
    std::vector<Direction> validMoves() const;
    Direction randomValidMoveFromInternalGenerator() const;
    bool canMove(Direction d) const;
    
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    
    unsigned int score() const;
    bool hasSameTilesAs(BoardState const& otherBoard, std::vector<BoardIndex> excludedIndices) const;
    
    friend std::ostream& operator<<(std::ostream &os, BoardState const& info);
    Hint getHint() const;
    
    cv::Mat sourceImage;
private:
    //mutator methods, should only be called inside constructors
    void set(BoardIndex i, Tile t);
    BoardIndex indexForNextTile(Direction d);
    void removeFromStack(Tile t);
    void copy(BoardState const &other);
    void move(Direction d);
    void addTile(Direction d);

    //non-mutators that aren't used extarnally
    Tile genUpcomingTile() const;
    unsigned int stackSize() const;
    Tile maxBonusTile() const;
    Tile maxTile() const;
    float nonBonusTileProbability(Tile tile, bool canHaveBonus) const;
    
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
    Tile upcomingTile;
    boost::optional<Hint> hint;
};

std::ostream& operator<<(std::ostream &os, const BoardState::BoardIndex e);
std::ostream& operator<<(std::ostream &os, BoardState const& info);

#endif /* defined(__ThreesAI__BoardState__) */
