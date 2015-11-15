//
//  Direction.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "Direction.h"

using namespace std;

unsigned char toIndex(Direction d) {
    switch (d) {
        case Direction::UP: return 0;
        case Direction::DOWN: return 1;
        case Direction::LEFT: return 2;
        case Direction::RIGHT: return 3;
    }
}

ostream& operator<<(ostream &os, Direction d){
    switch (d) {
        case Direction::UP:
            os << "UP";
            break;
        case Direction::DOWN:
            os << "DOWN";
            break;
        case Direction::RIGHT:
            os << "RIGHT";
            break;
        case Direction::LEFT:
            os << "LEFT";
            break;
    }
    return os;
}
