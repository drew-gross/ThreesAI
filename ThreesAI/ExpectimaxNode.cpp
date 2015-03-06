//
//  ExpectimaxNode.cpp
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "ExpectimaxNode.h"

int ExpectimaxNodeBase::num_existing_nodes = 0;

ExpectimaxNodeBase::ExpectimaxNodeBase(ThreesBoard const& board) : board(board){
    this->num_existing_nodes++;
}

ExpectimaxNodeBase::~ExpectimaxNodeBase(){
    ExpectimaxNodeBase::num_existing_nodes--;
    //std::cout << num_existing_nodes << std::endl;
};