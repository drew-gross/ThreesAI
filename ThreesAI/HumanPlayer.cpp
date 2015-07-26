//
//  HumanPlayer.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "HumanPlayer.h"

#include <iostream>

using namespace std;

HumanPlayer::HumanPlayer(std::unique_ptr<ThreesBoardBase>&& board) : ThreesAIBase(move(board)) {}

Direction HumanPlayer::playTurn() {
    try {
        switch (getchar()) {
            case 'w':
                board->move(UP);
                return UP;
                break;
                
            case 'a':
                board->move(LEFT);
                return LEFT;
                break;
                
            case 's':
                board->move(DOWN);
                return DOWN;
                break;
                
            case 'd':
                board->move(RIGHT);
                return RIGHT;
                break;
                
            case '\n':
                return this->playTurn();
                
            default:
                throw InvalidMoveException();
                break;
        }
    } catch (InvalidMoveException e) {
        cout << "Not a valid move" << endl;
        return this->playTurn();
    }
}