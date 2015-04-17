//
//  ThreesBoardBase.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/16/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ThreesBoardBase.h"

#include <iostream>
#include <iomanip>

using namespace std;

ThreesBoardBase::ThreesBoardBase() : numTurns(0) {
    
}

template < class T >
ostream& operator << (ostream& os, const deque<T>& v)
{
    os << "[";
    for (typename deque<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

ostream& operator<<(ostream &os, ThreesBoardBase const& board){
    os << "Upcoming: " << board.nextTileHint() << endl;
    if (board.isGameOver()) {
        os << "Final";
    } else {
        os << "Current";
    }
    os << " Score: " <<  board.score() << endl;
    os << "Number of turns: " << board.numTurns << endl;
    os << "---------------------" << endl;
    os << "|" << setw(4) << board.at({0,0}) << "|" << setw(4) << board.at({1,0}) << "|" << setw(4) << board.at({2,0}) << "|" << setw(4) << board.at({3,0}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,1}) << "|" << setw(4) << board.at({1,1}) << "|" << setw(4) << board.at({2,1}) << "|" << setw(4) << board.at({3,1}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,2}) << "|" << setw(4) << board.at({1,2}) << "|" << setw(4) << board.at({2,2}) << "|" << setw(4) << board.at({3,2}) << "|" << endl;
    os << "|" << setw(4) << board.at({0,3}) << "|" << setw(4) << board.at({1,3}) << "|" << setw(4) << board.at({2,3}) << "|" << setw(4) << board.at({3,3}) << "|" << endl;
    os << "---------------------";
    return os;
}