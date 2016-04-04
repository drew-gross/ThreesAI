//
//  Evaluators.hpp
//  ThreesAI
//
//  Created by Drew Gross on 1/25/16.
//  Copyright © 2016 DrewGross. All rights reserved.
//

#ifndef Evaluators_hpp
#define Evaluators_hpp

#include "BoardState.h"

float countEmptyTile(AboutToMoveBoard const& b);
float score(AboutToMoveBoard const& b) ;
float countAdjacentPair(AboutToMoveBoard const& b) ;
float countSplitPair(AboutToMoveBoard const& b);
float simScore(AboutToMoveBoard const& b);
float countAdjacentOffByOne(AboutToMoveBoard const& b);
float countTrappedTiles(AboutToMoveBoard const& b);
float highestIsInCorner(AboutToMoveBoard const& b);
float highestIsOnEdge(AboutToMoveBoard const& b);

#endif /* Evaluators_hpp */
