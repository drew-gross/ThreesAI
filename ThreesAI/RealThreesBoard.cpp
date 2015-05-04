//
//  RealThreesBoard.cpp
//  ThreesAI
//
//  Created by Drew Gross on 4/17/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//2

#include "Debug.h"
#include "Logging.h"

#include <unistd.h>

#include "arduino-serial-lib.h"

#include "RealThreesBoard.h"
#include "SimulatedThreesBoard.h"

using namespace std;
using namespace cv;

const Point2f getpoint() {
    Point2f p;
    setMouseCallback("capture", [](int event, int x, int y, int flags, void* userdata){
        Point2f *p = (Point2f*)userdata;
        p->x = x;
        p->y = y;
    }, &p);
    waitKey();
    return p;
}

RealThreesBoard::RealThreesBoard(string portName) : watcher(0) {
    
    this->boardImage = imread("/Users/drewgross/Projects/ThreesAI/SampleData/GameStartSample.png");
    imshow("capture", this->boardImage);
    const Point2f fromPoints[4] = {{341, 303},{313, 563},{623, 565},{603, 302}};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    Mat warped;
    Mat greyWarped;
    
    warpPerspective(this->boardImage, warped, transform, Size(800,800));
    cvtColor(warped, greyWarped, CV_BGR2GRAY);
    
    imshow("capture", greyWarped);
    
    SIFT sifter = SIFT();

    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect roi = Rect(200*i, 200*j, 200, 200);
            imshow("sub_elem", greyWarped(roi));
            waitKey();
        }
    }
    //TODO this needs t go back before getting the first image
    this->fd = serialport_init("/dev/tty.usbmodem1411", 9600);
    sleep(2);
    serialport_write(this->fd, "b");
    serialport_flush(this->fd);
    
    this->watcher >> this->boardImage;
}

RealThreesBoard::~RealThreesBoard() {
    cout << serialport_close(this->fd);
}

pair<unsigned int, ThreesBoardBase::BoardIndex> RealThreesBoard::move(Direction d) {
    switch (d) {
        case LEFT:
            serialport_write(this->fd, "l");
            break;
        case RIGHT:
            serialport_write(this->fd, "r");
            break;
        case UP:
            serialport_write(this->fd, "u");
            break;
        case DOWN:
            serialport_write(this->fd, "d");
            break;
    }
    
    serialport_flush(fd);
    
    //debug();
    return {0,{0,0}};
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    debug();
    return SimulatedThreesBoard();
}

deque<unsigned int> RealThreesBoard::nextTileHint() const {
    debug();
    return {};
}