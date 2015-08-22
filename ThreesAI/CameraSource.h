//
//  CameraSource.h
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__CameraSource__
#define __ThreesAI__CameraSource__

#include "GameStateSource.h"

class CameraSource : public GameStateSource {
public:
    CameraSource(int camNumber);
    
    BoardInfo getGameState();
    
private:
    cv::VideoCapture watcher;
};

#endif /* defined(__ThreesAI__CameraSource__) */
