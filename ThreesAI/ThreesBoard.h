//
//  ThreesBoard.h
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ThreesBoard__
#define __ThreesAI__ThreesBoard__

#include <stdio.h>
#include <array>

class ThreesBoard {
public:
    ThreesBoard();
    std::ostream& operator<<(std::ostream &os);
private:
    std::array<std::array<unsigned int, 4>, 4> board;
};

#endif /* defined(__ThreesAI__ThreesBoard__) */
