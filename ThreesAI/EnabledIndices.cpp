//
//  EnabledIndices.cpp
//  ThreesAI
//
//  Created by Drew Gross on 11/8/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "EnabledIndices.hpp"

#include "BoardIndex.hpp"

EnabledIndices::EnabledIndices(std::initializer_list<BoardIndex> indices) : data(0) {
    for (auto&& i : indices) {
        this->data.set(i.toRegularIndex());
    }
}

bool EnabledIndices::isEnabled(BoardIndex i) {
    return this->data.test(i.toRegularIndex());
}

void EnabledIndices::set(BoardIndex i) {
    this->data.set(i.toRegularIndex());
}

size_t EnabledIndices::size() {
    return this->data.count();
}