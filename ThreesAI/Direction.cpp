//
//  Direction.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "Direction.h"

using namespace std;

ostream& operator<<(ostream &os, Direction d){
    switch (d) {
        case UP:
            os << "UP";
            break;
        case DOWN:
            os << "DOWN";
            break;
        case RIGHT:
            os << "RIGHT";
            break;
        case LEFT:
            os << "LEFT";
            break;
    }
    return os;
}
