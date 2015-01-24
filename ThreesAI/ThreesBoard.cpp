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
    os << "|" << board.board[0][0] << "|" << board.board[0][1] << "|" << board.board[0][2] << "|" << board.board[0][3] << "|" << std::endl;
    os << "|" << board.board[1][0] << "|" << board.board[1][1] << "|" << board.board[1][2] << "|" << board.board[1][3] << "|" << std::endl;
    os << "|" << board.board[2][0] << "|" << board.board[2][1] << "|" << board.board[2][2] << "|" << board.board[2][3] << "|" << std::endl;
    os << "|" << board.board[3][0] << "|" << board.board[3][1] << "|" << board.board[3][2] << "|" << board.board[3][3] << "|" << std::endl;
    return os;
}