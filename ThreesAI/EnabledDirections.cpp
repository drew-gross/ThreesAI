//
//  EnabledDirections.cpp
//  ThreesAI
//
//  Created by Drew Gross on 11/10/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "EnabledDirections.hpp"

#include "BoardIndex.hpp"

EnabledDirections::EnabledDirections(std::initializer_list<Direction> indices) : data(0) {
    for (auto&& i : indices) {
        this->data.set(toIndex(i));
    }
}

bool EnabledDirections::isEnabled(Direction i) {
    return this->data.test(toIndex(i));
}

void EnabledDirections::set(Direction i) {
    this->data.set(toIndex(i));
}

size_t EnabledDirections::size() {
    return this->data.count();
}

bool EnabledDirections::empty() {
    return this->data.none();
}