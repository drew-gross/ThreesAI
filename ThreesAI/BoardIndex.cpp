//
//  BoardIndex.cpp
//  ThreesAI
//
//  Created by Drew Gross on 11/8/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "BoardIndex.hpp"

unsigned char BoardIndex::toRegularIndex() const {
    return first + second * 4;
}

boost::optional<BoardIndex> BoardIndex::left() const {
    if (this->first == 0) {
        return boost::none;
    }
    return BoardIndex(this->first - 1, this->second);
};
boost::optional<BoardIndex> BoardIndex::right() const {
    if (this->first == 3) {
        return boost::none;
    }
    return BoardIndex(this->first + 1, this->second);
};
boost::optional<BoardIndex> BoardIndex::up() const {
    if (this->second == 0) {
        return boost::none;
    }
    return BoardIndex(this->first, this->second - 1);
};
boost::optional<BoardIndex> BoardIndex::down() const {
    if (this->second == 3) {
        return boost::none;
    }
    return BoardIndex(this->first, this->second + 1);
};