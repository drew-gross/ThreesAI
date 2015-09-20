//
//  ExpectimaxNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxNode.h"

ExpectimaxNodeBase::ExpectimaxNodeBase(BoardState const& board, unsigned int depth) : board(board), depth(depth) {
}
