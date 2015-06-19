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

using namespace std;
using namespace cv;

TileInfo::TileInfo(cv::Mat image, int value) {
    this->image = image;
    this->value = value;
    IMProc::sifter().detect(image, this->keypoints);
    IMProc::sifter().compute(image, this->keypoints, this->descriptors);
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

void RealThreesBoard::connectAndStart(string portName) {
    this->fd = serialport_init(portName.c_str(), 9600);
    sleep(2); //Necessary to initialize the output for some
    if (this->fd >= 0) {
        serialport_write(this->fd, "b");
        serialport_flush(this->fd);
    }
}

int tileValue(Mat tileImage, const vector<TileInfo>& canonicalTiles) {
    FlannBasedMatcher matcher;
    
    vector<KeyPoint> tileKeypoints;
    Mat tileDescriptors;
    
    IMProc::sifter().detect(tileImage, tileKeypoints);
    IMProc::sifter().compute(tileImage, tileKeypoints, tileDescriptors);
    
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
        
        Mat matchDrawing;
        drawMatches(canonicalTile.image, canonicalTile.keypoints, tileImage, tileKeypoints, matches, matchDrawing);
        MYSHOW(matchDrawing);
        MYLOG(averageDistance);
        
        if (averageDistance < min) {
            MYLOG(canonicalTile.value)
            min = averageDistance;
            bestMatch = &canonicalTile;
            
            waitKey();
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

Mat RealThreesBoard::getAveragedImage(unsigned char numImages) {
    vector<Mat> images;
    for (unsigned char i = 0; i < numImages; i++) {
        Mat image;
        this->watcher >> image;
        images.push_back(image);
    }
    Mat averagedImage;
    if (numImages == 0) {
        return averagedImage;
    }
    averagedImage = Mat::zeros(images[0].rows, images[0].cols, images[0].type());
    for (unsigned char i = 0; i < numImages; i++) {
        averagedImage += images[i]/numImages;
    }
    return averagedImage;
}

RealThreesBoard::RealThreesBoard(string portName) : ThreesBoardBase(array<array<unsigned int, 4>, 4>({array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0})})), watcher(0) {
    
    this->connectAndStart(portName);
    Mat boardImage(this->getAveragedImage(5));
    
    boardImage = imread("/Users/drewgross/Projects/ThreesAI/TestCaseImages/x.png");
    this->board = boardState(IMProc::colorImageToBoard(boardImage), IMProc::canonicalTiles);
    MYSHOW(boardImage);
    waitKey();
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
    
    SimulatedThreesBoard expectedBoardAfterMove = this->simulatedCopy();
    expectedBoardAfterMove.moveWithoutAdd(d);
    vector<ThreesBoardBase::BoardIndex> possiblyEmptyTilesAfterMoveWithoutAdd = expectedBoardAfterMove.validIndicesForNewTile(d);
    
    Mat boardImage(this->getAveragedImage(5));
    this->isGameOverCacheIsValid = false;
    this->board = boardState(IMProc::colorImageToBoard(boardImage), IMProc::canonicalTiles);
    if (!this->hasSameTilesAs(expectedBoardAfterMove, possiblyEmptyTilesAfterMoveWithoutAdd)) {
        MYSHOW(boardImage);
        debug();
    }
    
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