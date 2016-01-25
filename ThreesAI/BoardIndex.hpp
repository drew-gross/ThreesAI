//
//  BoardIndex.hpp
//  ThreesAI
//
//  Created by Drew Gross on 11/8/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef BoardIndex_hpp
#define BoardIndex_hpp

#include <utility>
#include <array>

#include <boost/optional/optional.hpp>

class BoardIndex {
public:
    unsigned char first:2;
    unsigned char second:2;
    BoardIndex(unsigned char first, unsigned char second) : first(first), second(second) {};
    unsigned char toRegularIndex() const;
    
    boost::optional<BoardIndex> left() const;
    boost::optional<BoardIndex> right() const;
    boost::optional<BoardIndex> up() const;
    boost::optional<BoardIndex> down() const;
};

static std::array<BoardIndex, 16> const allIndices = {
    BoardIndex(0,0),
    BoardIndex(0,1),
    BoardIndex(0,2),
    BoardIndex(0,3),
    BoardIndex(1,0),
    BoardIndex(1,1),
    BoardIndex(1,2),
    BoardIndex(1,3),
    BoardIndex(2,0),
    BoardIndex(2,1),
    BoardIndex(2,2),
    BoardIndex(2,3),
    BoardIndex(3,0),
    BoardIndex(3,1),
    BoardIndex(3,2),
    BoardIndex(3,3),
};

#endif /* BoardIndex_hpp */
