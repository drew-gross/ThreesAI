//
//  Direction.h
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__Direction__
#define __ThreesAI__Direction__

#include <iostream>

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

std::ostream& operator<<(std::ostream &os, Direction d);

#endif /* defined(__ThreesAI__Direction__) */
