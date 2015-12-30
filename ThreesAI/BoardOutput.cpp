//
//  BoardOutput.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/10/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "BoardOutput.h"

using namespace std;
using namespace chrono;

void BoardOutput::doWorkFor(int millis) {
    if (!this->doWork) return;
    milliseconds startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds timeElapsed = milliseconds(0);
    while (timeElapsed < milliseconds(millis)) {
        timeElapsed = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - startTime;
        this->doWork();
    }
}