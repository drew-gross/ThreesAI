//
//  ThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoard.h"
#include <iostream>

ThreesBoard::ThreesBoard() {
    this->board = std::array<std::array<unsigned int, 4>, 4>();
}

std::ostream& operator<<(std::ostream &os, ThreesBoard board){
    os << "---";
    return os;
}