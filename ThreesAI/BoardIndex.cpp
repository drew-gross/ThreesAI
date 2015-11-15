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
