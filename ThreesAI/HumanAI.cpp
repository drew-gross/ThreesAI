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

Direction HumanAI::playTurn() {
    cout << board->score() << endl;
    cout << &board << endl;
    try {
        switch (getchar()) {
            case 'w':
                board->move(UP);
                cout << &board;
                return UP;
                break;
                
            case 'a':
                board->move(LEFT);
                cout << &board;
                return LEFT;
                break;
                
            case 's':
                board->move(DOWN);
                cout << &board;
                return DOWN;
                break;
                
            case 'd':
                board->move(RIGHT);
                cout << &board;
                return RIGHT;
                break;
                
            default:
                throw InvalidMoveException();
                break;
        }
    } catch (InvalidMoveException e) {
        cout << "Not a valid move" << endl;
        return this->playTurn();
    }
}