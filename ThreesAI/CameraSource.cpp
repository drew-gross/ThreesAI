//
//  CameraSource.cpp
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "CameraSource.h"

using namespace cv;
using namespace IMProc;

CameraSource::CameraSource(int camNumber) : watcher(camNumber) {}

BoardInfo CameraSource::getGameState() {
    Mat newImage(getAveragedImage(this->watcher, 8));
    return boardState(screenImage(newImage), newImage, canonicalTiles());
}