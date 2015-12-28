//
//  Heuristic.hpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Heuristic_hpp
#define Heuristic_hpp

#include "BoardState.h"
#include <functional>

typedef std::function<float(BoardState const&)> Heuristic;

#endif /* Heuristic_hpp */
