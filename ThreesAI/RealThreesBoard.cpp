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
#include <array>

#include "arduino-serial-lib.h"

#include "RealThreesBoard.h"
#include "SimulatedThreesBoard.h"
#include "IMProc.h"

using namespace std;
using namespace cv;

TileInfo::TileInfo(cv::Mat image, int value) {
    this->image = image;
    this->value = value;
    SIFT sifter = SIFT();
    sifter.detect(image, this->keypoints);
    sifter.compute(image, this->keypoints, this->descriptors);
}

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

const array<Point2f, 4> getQuadrilateral(Mat m) {
    imshow("get rect", m);
    return array<Point2f, 4>{{getpoint("get rect"),getpoint("get rect"),getpoint("get rect"),getpoint("get rect")}};
}

void imShowVector(vector<Mat> v) {
    int totalWidth = accumulate(v.begin(), v.end(), 0, [](int s, Mat m){
        return s + m.cols;
    });
    int maxHeight = std::max_element(v.begin(), v.end(), [](Mat first, Mat second){
        return first.rows < second.rows;
    })->rows;
    
    Mat combined(maxHeight, totalWidth, v[0].type());
    
    int widthSoFar = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        it->copyTo(combined(Rect(widthSoFar, 0, it->cols, it->rows)));
        widthSoFar += it->cols;
    }
    
    MYSHOW(combined);
}

void RealThreesBoard::connectAndStart(string portName) {
    this->fd = serialport_init(portName.c_str(), 9600);
    sleep(2); //Necessary to initialize the output for some reason
    if (this->fd >= 0) {
        serialport_write(this->fd, "b");
        serialport_flush(this->fd);
    }
}

int tileValue(Mat tileImage, const vector<TileInfo>& canonicalTiles) {
    SIFT sifter = SIFT();
    FlannBasedMatcher matcher;
    
    vector<KeyPoint> tileKeypoints;
    Mat tileDescriptors;
    
    sifter.detect(tileImage, tileKeypoints);
    sifter.compute(tileImage, tileKeypoints, tileDescriptors);
    
    if (tileDescriptors.empty()) {
        //Probably blank
        return 0;
    }
    
    vector<float> distances;
    float min = INFINITY;
    const TileInfo *bestMatch = &canonicalTiles[0];
    
    for (auto&& canonicalTile : canonicalTiles) {
        vector<DMatch> matches;
        matcher.match(canonicalTile.descriptors, tileDescriptors, matches);
        
        float averageDistance = accumulate(matches.begin(), matches.end(), float(0), [](float sum, DMatch d) {
            return sum + d.distance;
        })/float(matches.size());
        
        if (averageDistance < min) {
            min = averageDistance;
            bestMatch = &canonicalTile;
        }
    }
    
    return bestMatch->value;
}

array<array<unsigned int, 4>, 4> boardState(Mat boardImage, const vector<TileInfo>& canonicalTiles) {
    array<array<unsigned int, 4>, 4> board;
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect tileRoi = Rect(200*i, 200*j, 200, 200);
            const Mat currentTile = boardImage(tileRoi);
            board[j][i] = tileValue(currentTile, canonicalTiles);
        }
    }
    return board;
}

RealThreesBoard::RealThreesBoard(string portName) : ThreesBoardBase(array<array<unsigned int, 4>, 4>({array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0})})), watcher(0) {
    this->connectAndStart(portName);
    Mat colorBoardImage;
    this->watcher >> colorBoardImage;
    this->board = boardState(IMProc::colorImageToBoard(colorBoardImage), IMProc::canonicalTiles);
}

RealThreesBoard::~RealThreesBoard() {
    if (this->fd >= 0) {
        serialport_close(this->fd);
    }
}

pair<unsigned int, ThreesBoardBase::BoardIndex> RealThreesBoard::move(Direction d) {
    if (this->fd >= 0) {
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
    }
    
    //TODO: Sanity check against what I expect the new board to look like.
    Mat colorBoardImage;
    this->watcher >> colorBoardImage;
    this->isGameOverCacheIsValid = false;
    this->board = boardState(IMProc::colorImageToBoard(colorBoardImage), IMProc::canonicalTiles);
    //TODO: Get the actual location and value of the new tile
    return {0,{0,0}};
}

SimulatedThreesBoard RealThreesBoard::simulatedCopy() const {
    return SimulatedThreesBoard(std::move(this->board));
}

deque<unsigned int> RealThreesBoard::nextTileHint() const {
    //TODO: get this from the image;
    return {3};
}