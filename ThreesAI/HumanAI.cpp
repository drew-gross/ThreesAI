//
//  HumanAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "HumanAI.h"

#include <iostream>

using namespace std;

void HumanAI::playTurn() {
    cout << board.score() << endl;
    cout << board << endl;
    try {
        switch (getchar()) {
            case 'w':
                board.move(UP);
                cout << board;
                break;
                
            case 'a':
                board.move(LEFT);
                cout << board;
                break;
                
            case 's':
                board.move(DOWN);
                std::cout << board;
                break;
                
            case 'd':
                board.move(RIGHT);
                std::cout << board;
                break;
                
            default:
                break;
        }
    } catch (InvalidMoveException e) {
        cout << "Not a valid move" << endl;
        this->playTurn();
    }
}