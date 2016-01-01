//
//  Population.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/30/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Population.hpp"

Population& Population::operator=(Population const& other) {
    this->p = other.p;
    return *this;
}