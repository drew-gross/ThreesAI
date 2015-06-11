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

const vector<TileInfo> RealThreesBoard::loadCanonicalTiles() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat t;
    
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat t2;
    
    vector<TileInfo> results;
    
    const int L12 = 80;
    const int R12 = 200;
    const int T12 = 310;
    const int B12 = 630;
    
    const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
    const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};
    warpPerspective(image12, t2, getPerspectiveTransform(fromPoints12, toPoints12), Size(200,400));
    
    Mat image1;
    t2(Rect(0,200,200,200)).copyTo(image1);
    
    Mat image2;
    t2(Rect(0,0,200,200)).copyTo(image2);
    
    results.push_back(TileInfo(image2, 2));
    results.push_back(TileInfo(image1, 1));
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    warpPerspective(image, t, transform, Size(800,600));
    
    
    const array<int, 13> indexToTile = {1,2,3,6,12,24,48,96,192,384,768,1536,3072};
    
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Mat tile;
            t(Rect(200*j, 200*i, 200, 200)).copyTo(tile);
            results.push_back(TileInfo(tile, indexToTile[results.size()]));
        }
    }
    
    return results;
}

Mat RealThreesBoard::captureBoardImage() {
    Mat colorBoardImage;
    Mat greyBoardImage;
    Mat screenImage;
    Mat outputImage;
    
    this->watcher >> colorBoardImage;
    cvtColor(colorBoardImage, greyBoardImage, CV_RGB2GRAY);
    greyBoardImage = imread("/Users/drewgross/Projects/ThreesAI/SampleData/CameraSample1.png", 0);

    vector<Point> screenContour = IMProc::findScreenContour(greyBoardImage);
    
    const Point2f fromCameraPoints[4] = {screenContour[0], screenContour[1], screenContour[2], screenContour[3]};
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    
    warpPerspective(greyBoardImage, screenImage, getPerspectiveTransform(fromCameraPoints, toPoints), Size(800,800));
    
    const Point2f fromScreenPoints[4] = {{100,210},{100,670},{700,670},{700,210}};
    
    warpPerspective(screenImage, outputImage, getPerspectiveTransform(fromScreenPoints, toPoints), Size(800,800));
    
    return outputImage;
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

RealThreesBoard::RealThreesBoard(string portName) : ThreesBoardBase(array<array<unsigned int, 4>, 4>({array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0}),array<unsigned int, 4>({0,0,0,0})})), watcher(0) , canonicalTiles(this->loadCanonicalTiles()) {
    this->connectAndStart(portName);
    this->board = boardState(this->captureBoardImage(), this->canonicalTiles);
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
    this->board = boardState(this->captureBoardImage(), this->canonicalTiles);
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