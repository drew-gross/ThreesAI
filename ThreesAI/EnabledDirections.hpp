//
//  EnabledDirections.hpp
//  ThreesAI
//
//  Created by Drew Gross on 11/10/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef EnabledDirections_hpp
#define EnabledDirections_hpp

#include <stdint.h>
#include <bitset>

#include "Direction.h"

class EnabledDirections {
public:
    static const int maxSize = 4;
private:
    std::bitset<maxSize> data;
    
public:
    EnabledDirections(std::initializer_list<Direction> indices = {});
    bool isEnabled(Direction i);
    void set(Direction i);
    
    size_t size();
    bool empty();
};

#endif /* EnabledDirections_hpp */
