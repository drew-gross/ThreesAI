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
#include "ForcedHint.hpp"

#include <opencv2/opencv.hpp>
class BoardState {
public:
    typedef std::pair<unsigned int, unsigned int> BoardIndex;
    static std::array<BoardIndex, 16> indexes();
    
    typedef std::array<Tile, 16> Board;
    
    BoardState(Board b,
               Tile upcomingTile,
               std::default_random_engine gen,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    BoardState(Board b,
               std::default_random_engine hintGen,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    BoardState(Board b,
               std::default_random_engine gen,
               std::shared_ptr<Hint const> hint,
               unsigned int numTurns,
               cv::Mat sourceImage,
               unsigned int onesInStack,
               unsigned int twosInStack,
               unsigned int threesInStack);
    
    static BoardState fromString(const std::string s);
    
    unsigned int numTurns;
    
    Tile at(BoardIndex const& p) const;
    //Advances the RNG like how is normally done when adding a tile, but pretends the tile was actually added in the specified place.
    BoardState addSpecificTile(Direction d, BoardState::BoardIndex const& i, const Tile t) const;
    
    bool isGameOver() const;
    
    std::vector<Direction> validMoves() const;
    Direction randomValidMoveFromInternalGenerator() const;
    bool canMove(Direction d) const;
    const BoardState move(Direction d) const;
    const BoardState moveWithoutAdd(Direction d) const;
    const BoardState addTile(Direction d) const;
    
    const BoardState copyWithDifferentFuture() const;
    
    std::deque<std::pair<Tile, float>> possibleNextTiles() const;
    std::vector<BoardIndex> validIndicesForNewTile(Direction movedDirection) const;
    
    unsigned int score() const;
    
    Tile maxTile() const;
    bool hasSameTilesAs(BoardState const& otherBoard, std::vector<BoardIndex> excludedIndices) const;
    
    friend std::ostream& operator<<(std::ostream &os, BoardState const& info);
    friend class Hint;
    Tile maxBonusTile() const;
    float nonBonusTileProbability(Tile tile, bool canHaveBonus) const;
    unsigned int stackSize() const;
    std::shared_ptr<Hint const> getHint() const;
    
    cv::Mat sourceImage;
private:
    mutable bool isGameOverCache;
    mutable bool isGameOverCacheIsValid;
    mutable unsigned int scoreCache;
    mutable bool scoreCacheIsValid;
    mutable bool validMovesCacheIsValid;
    mutable std::vector<Direction> validMovesCache;
    std::default_random_engine generator;
    
    BoardState mutableCopy() const;
    
    Tile nextBoardUpcomingTile() const;
    
    unsigned int onesInStack;
    unsigned int twosInStack;
    unsigned int threesInStack;
    Board board;
    Tile upcomingTile;
    std::shared_ptr<Hint const> hint;
};

std::ostream& operator<<(std::ostream &os, const BoardState::BoardIndex e);
std::ostream& operator<<(std::ostream &os, BoardState const& info);

#endif /* defined(__ThreesAI__BoardState__) */
