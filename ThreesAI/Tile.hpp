//
//  Tile.hpp
//  ThreesAI
//
//  Created by Drew Gross on 9/20/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <string>

#include <boost/optional/optional.hpp>

class HiddenBoardState;

enum class T {
    EMPTY,
    _1,
    _2,
    _3,
    _6,
    _12,
    _24,
    _48,
    _96,
    _192,
    _384,
    _768,
    _1536,
    _3072,
    _6144
};

class Tile {
    T value;
    Tile(){}
public:
    Tile(T v) : value(v) {}
    bool operator <=(Tile const& other) const { return this->value <= other.value; };
    bool operator >=(Tile const& other) const { return this->value >= other.value; };
    bool operator >(Tile const& other) const { return this->value > other.value; };
    bool operator <(Tile const& other) const { return this->value < other.value; };
    bool operator ==(Tile const& other) const { return this->value == other.value; };
    bool operator !=(Tile const& other) const { return this->value != other.value; };
    Tile pred() const;
    Tile succ() const;
    bool canMergeOrMove(Tile target) const;
    bool canMerge(Tile target) const;
    boost::optional<Tile> mergeResult(Tile t2) const;
    unsigned int tileScore() const;
    friend std::ostream& operator<<(std::ostream &os, Tile t);
    //TODO: Make this a member
    friend float nonBonusTileProbability(HiddenBoardState const& hiddenState, Tile tile, bool canHaveBonus);
    friend HiddenBoardState;
};

Tile tileFromString(std::string s);

std::ostream& operator<<(std::ostream &os, Tile t);

#endif /* Tile_hpp */
