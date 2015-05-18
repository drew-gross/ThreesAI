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

const vector<Mat> RealThreesBoard::loadSampleImages() {
    Mat image = imread("/Users/drewgross/Projects/ThreesAI/SampleData/Tiles.png", 0);
    Mat t;
    
    Mat image12 = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat t2;
    
    vector<Mat> results;
    
    const int L12 = 80;
    const int R12 = 200;
    const int T12 = 310;
    const int B12 = 630;
    
    const Point2f fromPoints12[4] = {{L12,T12},{L12,B12},{R12,B12},{R12,T12}};
    const Point2f toPoints12[4] = {{0,0},{0,400},{200,400},{200,0}};
    warpPerspective(image12, t2, getPerspectiveTransform(fromPoints12, toPoints12), Size(200,400));
    
    Mat image1;
    t2(Rect(0,0,200,200)).copyTo(image1);
    
    Mat image2;
    t2(Rect(0,200,200,200)).copyTo(image2);
    
    results.push_back(image2);
    results.push_back(image1);
    
    const int L = 80;
    const int R = 560;
    const int T = 310;
    const int B = 800;
    
    const Point2f fromPoints[4] = {{L,T},{L,B},{R,B},{R,T}};
    const Point2f toPoints[4] = {{0,0},{0,600},{800,600},{800,0}};
    Mat transform = getPerspectiveTransform(fromPoints, toPoints);
    
    warpPerspective(image, t, transform, Size(800,600));
    
    for (unsigned char i = 0; i < 3; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Mat tile;
            t(Rect(200*j, 200*i, 200, 200)).copyTo(tile);
            results.push_back(tile);
        }
    }
    
    imShowVector(results);
    
    return results;
}

RealThreesBoard::RealThreesBoard(string portName) : watcher(0) , sampleImages(loadSampleImages()) {
    //Get descriptors
    SIFT sifter = SIFT();
    
    for (auto&& image : this->sampleImages) {
        vector<KeyPoint> kp;
        sifter.detect(image, kp);
        this->sampleKeyPoints.emplace_back(kp);
        
        Mat descriptor;
        sifter.compute(image, kp, descriptor);
        this->sampleDescriptors.emplace_back(descriptor);
    }
    
    //Get photo of initial board
    Mat cameraSample = imread("/Users/drewgross/Projects/ThreesAI/SampleData/12.png", 0);
    Mat sampleBoard;
    
    auto fromPoints = getQuadrilateral(cameraSample);
    const Point2f toPoints[4] = {{0,0},{0,800},{800,800},{800,0}};
    
    warpPerspective(cameraSample, sampleBoard, getPerspectiveTransform(fromPoints.data(), toPoints), Size(800,800));
    MYSHOW(sampleBoard);
    waitKey();
    
    for (unsigned char i = 0; i < 4; i++) {
        for (unsigned char j = 0; j < 4; j++) {
            Rect roi = Rect(200*i, 200*j, 200, 200);
            const Mat currentTile = sampleBoard(roi);
            vector<KeyPoint> currentTileKeypoints;
            Mat currentTileDescriptors;

            sifter.detect(currentTile, currentTileKeypoints);
            sifter.compute(currentTile, currentTileKeypoints, currentTileDescriptors);
            
            FlannBasedMatcher matcher;
            if (!currentTileDescriptors.empty()) {
                for (int i = 0; i < this->sampleDescriptors.size(); i++) {
                    auto sampleDescriptor = this->sampleDescriptors[i];
                    
                    vector<DMatch> matches;
                    Mat out;
                    matcher.match(sampleDescriptor, currentTileDescriptors, matches);
                    drawMatches(this->sampleImages[i], this->sampleKeyPoints[i], currentTile, currentTileKeypoints, matches, out);
                    MYSHOW(out);
                    
                    float averageDistance = accumulate(matches.begin(), matches.end(), float(0), [](float sum, DMatch d) {
                        return sum + d.distance;
                    })/float(matches.size());
                    
                    MYLOG(averageDistance);
                    
                    waitKey();
                }
            } else {
                MYSHOW(currentTile);
                waitKey();
            }
        }
    }
    
    //TODO this needs to go back before getting the first image
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