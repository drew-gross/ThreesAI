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

float countEmptyTile(BoardState const& b);
float score(BoardState const& b) ;
float countAdjacentPair(BoardState const& b) ;
float countSplitPair(BoardState const& b);
float simScore(BoardState const& b);
float countAdjacentOffByOne(BoardState const& b);
float countTrappedTiles(BoardState const& b);
float highestIsInCorner(BoardState const& b) ;

#endif /* Evaluators_hpp */
