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
    const Point2f fromPoints[4] = {{341, 303},{313, 563},{623, 565},{603, 302}};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    Mat warped;
    Mat greyWarped;
    
    warpPerspective(this->boardImage, warped, transform, Size(800,800));
    cvtColor(warped, greyWarped, CV_BGR2GRAY);
    
    SIFT sifter = SIFT();
    vector<KeyPoint> kp1;
    vector<KeyPoint> kp2;
    vector<KeyPoint> kp3;
    
    Mat oneImage;
    Mat twoImage;
    Mat treImage;

    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect roi = Rect(200*i+50, 200*j+50, 100, 100);
            if (i == 1 and j == 0) {
                oneImage = greyWarped(roi);
            }
            if (i == 0 and j == 1) {
                twoImage = greyWarped(roi);
            }
            if (i == 1 and j == 1) {
                treImage = greyWarped(roi);
            }
        }
    }
    
    Mat th1;
    Mat th2;
    Mat th3;
    threshold(oneImage, th1, 96, 255, THRESH_BINARY_INV);
    threshold(twoImage, th2, 96, 255, THRESH_BINARY_INV);
    threshold(treImage, th3, 96, 255, THRESH_BINARY);
    
    sifter(th1, Mat(), kp1, noArray());
    sifter(twoImage, Mat(), kp2, noArray());
    sifter(treImage, Mat(), kp3, noArray());
    
    Mat kp1i;
    Mat kp2i;
    Mat kp3i;
    
    Mat e = getStructuringElement(MORPH_ELLIPSE, Size(8,8));
    
    drawKeypoints(th1, kp1, kp1i);
    drawKeypoints(th2, kp2, kp2i);
    drawKeypoints(th3, kp3, kp3i);
    
    imshow("1", oneImage);
    imshow("th1", kp1i);
    imshow("2", twoImage);
    imshow("th2", kp2i);
    imshow("3", treImage);
    imshow("th3", kp3i);
    
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