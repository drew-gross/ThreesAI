//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>
#include "ThreesBoard.h"

int main(int argc, const char * argv[]) {
    ThreesBoard board;
    std::cout << board;
    while (true) {
        std::cout << board.score() << std::endl;
        switch (getchar()) {
            case 'w':
                board.processInputDirection(UP);
                std::cout << board;
                break;
                
            case 'a':
                board.processInputDirection(LEFT);
                std::cout << board;
                break;
                
            case 's':
                board.processInputDirection(DOWN);
                std::cout << board;
                break;
                
            case 'd':
                board.processInputDirection(RIGHT);
                std::cout << board;
                break;
                
            default:
                break;
        }
    }
    return 0;
}