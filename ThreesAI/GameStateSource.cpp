//
//  GameStateSource.cpp
//  ThreesAI
//
//  Created by Drew Gross on 8/18/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "GameStateSource.h"

std::shared_ptr<AboutToMoveBoard const> GameStateSource::getInitialState() {
    auto reading = this->getGameState(HiddenBoardState(0,4,4,4));
    long ones = 4-reading->countOfTile(T::_1);
    long twos = 4-reading->countOfTile(T::_2);
    long threes = 4-reading->countOfTile(T::_3);
    return std::make_shared<AboutToMoveBoard const>(AboutToMoveBoard::SetHiddenState(HiddenBoardState(0, (unsigned int)ones, (unsigned int)twos, (unsigned int)threes)), *reading);
}