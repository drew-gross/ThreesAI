//
//  HumanPlayer.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "HumanPlayer.h"

#include <iostream>

#include "InvalidMoveException.h"

using namespace std;

HumanPlayer::HumanPlayer(std::shared_ptr<AboutToMoveBoard const> board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

void HumanPlayer::prepareDirection() {}

Direction getMove() {
    cout << "Enter a move: "; cout.flush();
    for (;;) {
        switch (getchar()) {
            case 'w':
                return Direction::UP;
            case 'a':
                return Direction::LEFT;
            case 's':
                return Direction::DOWN;
            case 'd':
                return Direction::RIGHT;
            case '\n':
                break;
            default:
                cout << "Use wasd to enter a move: ";
                break;
        }
    }
}

Direction HumanPlayer::getDirection() const {
    cout << *this->currentState() << endl;
    for (;;) {
        Direction d = getMove();
        if (this->currentState()->canMove(d)) {
            return d;
        } else {
            cout << "Illegal move!" << endl;
        }
    }
}