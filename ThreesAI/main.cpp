//
//  main.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2015-01-23.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include <iostream>

#include <memory>

#include "arduino-serial-lib.h"

#include "Logging.h"

#include "ThreesBoard.h"
#include "RandomAI.h"
#include "ZeroDepthMaxScoreAI.h"
#include "ExpectimaxAI.h"
#include "HumanAI.h"

using namespace std;

template <typename T>
ostream& operator<<(ostream &os, const std::deque<T> d){
    os << "[";
    for (auto&& t : d) {
        os << " " << t;
    }
    os << "]";
    return os;
}

void playOneGame() {
    unique_ptr<ThreesBoard> b(new ThreesBoard);
    ExpectimaxAI ai(move(b));
    clock_t startTime = clock();
    while (!ai.board->isGameOver()) {
        cout << &ai.board << endl;
        Direction move = ai.playTurn();
        cout << move << endl << endl;
    }
    cout << &ai.board << endl;
    clock_t endTime = clock();
    double elapsed_time = (endTime-startTime)/(double)CLOCKS_PER_SEC;
    MYLOG(elapsed_time);
}

void send_byte(){
    int fd = -1;
    fd = serialport_init("/dev/tty.usbmodem1411", 9600);
    if (fd != -1) {
        serialport_flush(fd);
        serialport_write(fd, "b\n");
    }
}

int main(int argc, const char * argv[]) {
    //send_byte();
    deque<unsigned int> turnsSurvived;
    for (int i=1; i <= 3; i++) {
        TileStack::randomGenerator.seed(i);
        unique_ptr<ThreesBoard> b(new ThreesBoard);
        ExpectimaxAI ai(move(b));
        ai.playGame();
        turnsSurvived.push_back(ai.board->numTurns);
        cout << "Seed: " << i << endl;
        cout << *ai.board.get() << endl;
    }
    MYLOG(turnsSurvived);
    return 0;
}