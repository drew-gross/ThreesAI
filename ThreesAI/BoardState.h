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

#include <opencv2/opencv.hpp>

class BoardState {
public:
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    static std::array<BoardIndex, 16> indexes();
    
    typedef std::array<unsigned int, 16> Board;
    typedef std::deque<unsigned int> Hint;
    
    BoardState(Board b, unsigned int numTurns, cv::Mat sourceImage, unsigned int onesInStack, unsigned int twosInStack, unsigned int threesInStack, std::deque<unsigned int> hint);
    static BoardState fromString(const std::string s);
    
    unsigned int numTurns;
    
    unsigned int at(BoardIndex const& p) const;
    //Advances the RNG like how is normally done when adding a tile, but pretends the tile was actually added in the specified place.
    BoardState addSpecificTile(Direction d, BoardState::BoardIndex const& i, const unsigned int t) const;
    
    bool isGameOver() const;
    
    std::vector<Direction> validMoves() const;
    Direction randomValidMove(std::default_random_engine generator) const;
    bool canMove(Direction d) const;
    const BoardState move(Direction d) const;
    const BoardState moveWithoutAdd(Direction d) const;
    const BoardState addTile(Direction d) const;
    
    const BoardState copyWithDifferentFuture() const;
    
    std::deque<std::pair<unsigned int, float>> possibleNextTiles() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    
    unsigned int score() const;
    
    unsigned int maxTile() const;
    bool hasSameTilesAs(BoardState const& otherBoard, std::vector<BoardIndex> excludedIndices) const;
    
    friend std::ostream& operator<<(std::ostream &os, BoardState const& info);
    unsigned int maxBonusTile() const;
    float nonBonusTileProbability(unsigned int tile, bool canHaveBonus) const;
    unsigned int stackSize() const;
    Hint nextTileHint() const;
    
    cv::Mat sourceImage;
private:
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid;
    mutable unsigned int scoreCache;
    mutable bool scoreCacheIsValid;
    std::default_random_engine generator;
    
    Hint forcedHint;
    
    BoardState mutableCopy() const;
    
    bool canMerge(BoardIndex const& target, BoardIndex const& here) const;
    bool tryMerge(BoardIndex const& target, BoardIndex const& other) const;
    unsigned int upcomingTile() const;
    
    unsigned int onesInStack;
    unsigned int twosInStack;
    unsigned int threesInStack;
    Board board;
};

std::ostream& operator<<(std::ostream &os, const BoardState::BoardIndex e);
std::ostream& operator<<(std::ostream &os, BoardState const& info);

#endif /* defined(__ThreesAI__BoardState__) */
