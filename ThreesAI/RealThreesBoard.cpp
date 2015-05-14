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

const Point2f getpoint(const string& window) {
    Point2f p;
    setMouseCallback(window, [](int event, int x, int y, int flags, void* userdata){
        Point2f *p = (Point2f*)userdata;
        p->x = x;
        p->y = y;
    }, &p);
    waitKey();
    return p;
}

const vector<Mat> RealThreesBoard::loadSampleImages() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png");
    Mat t;
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    warpPerspective(image, t, transform, Size(800,600));
    MYSHOW(t);
    debug();
    cvtColor(image, image, CV_BGR2GRAY);
    
    Mat pic1;
    Mat pic2;
    Mat pic3;
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect roi = Rect(200*i+50, 200*j+50, 100, 100);
            if (i == 1 and j == 0) {
                image(roi).copyTo(pic1);
            }
            if (i == 0 and j == 1) {
                image(roi).copyTo(pic2);
            }
            if (i == 1 and j == 1) {
                image(roi).copyTo(pic3);
            }
        }
    }
    
    MYSHOW(pic1);
    MYSHOW(pic2);
    MYSHOW(pic3);
    return {};
}

RealThreesBoard::RealThreesBoard(string portName) : watcher(0) , sampleImages(loadSampleImages()) {
    Mat greyWarped;
    
    SIFT sifter = SIFT();
    vector<KeyPoint> kp1;
    vector<KeyPoint> kp2;
    vector<KeyPoint> kp3;
    Mat desc1;
    Mat desc2;
    Mat desc3;
    
    vector<DMatch> matches1;
    vector<DMatch> matches2;
    vector<DMatch> matches3;
    
    Mat pic1,pic2,pic3;
    
    GaussianBlur(pic1, pic1, Size(5,5), 0);
    GaussianBlur(pic2, pic2, Size(5,5), 0);
    GaussianBlur(pic3, pic3, Size(5,5), 0);
    
    threshold(pic1, pic1, 96, 255, THRESH_BINARY);
    threshold(pic2, pic2, 96, 255, THRESH_BINARY);
    threshold(pic3, pic3, 96, 255, THRESH_BINARY);
    
    sifter.detect(pic1, kp1);
    sifter.detect(pic2, kp2);
    sifter.detect(pic3, kp3);
    
    sifter.compute(pic1, kp1, desc1);
    sifter.compute(pic2, kp2, desc2);
    sifter.compute(pic3, kp3, desc3);
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect roi = Rect(200*i+50, 200*j+50, 100, 100);
            Mat target;
            greyWarped(roi).copyTo(target);
            
            threshold(target, target, 110, 255, THRESH_BINARY);
            
            vector<KeyPoint> kp;
            Mat desc;
            sifter.detect(target, kp);
            sifter.compute(target, kp, desc);
            
            FlannBasedMatcher matcher;
            if (!desc.empty()) {
                matcher.match(desc1, desc, matches1);
                Mat matchImage1;
                matcher.match(desc2, desc, matches2);
                Mat matchImage2;
                matcher.match(desc3, desc, matches3);
                Mat matchImage3;
                drawMatches(pic1, kp1, target, kp, matches1, matchImage1);
                drawMatches(pic2, kp2, target, kp, matches2, matchImage2);
                drawMatches(pic3, kp3, target, kp, matches3, matchImage3);
                MYSHOW(matchImage1);
                MYSHOW(matchImage2);
                MYSHOW(matchImage3);
                waitKey();
            }
        }
    }
    
    vector<vector<DMatch>> m = {matches1, matches2, matches3};

    
    drawKeypoints(pic1, kp1, pic1);
    drawKeypoints(pic2, kp2, pic2);
    drawKeypoints(pic3, kp3, pic3);
    
    MYSHOW(pic1);
    MYSHOW(pic2);
    MYSHOW(pic3);
    MYSHOW(greyWarped);
    
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