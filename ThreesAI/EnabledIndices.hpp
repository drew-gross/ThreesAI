//
//  EnabledIndices.hpp
//  ThreesAI
//
//  Created by Drew Gross on 11/8/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef EnabledIndices_hpp
#define EnabledIndices_hpp

#include <stdint.h>
#include <bitset>

#include "BoardIndex.hpp"

class EnabledIndices {
public:
    static const int maxSize = 16;
private:
    std::bitset<maxSize> data;
    
public:
    EnabledIndices(std::initializer_list<BoardIndex> indices);
    bool isEnabled(BoardIndex i);
    void set(BoardIndex i);

    size_t size();
};

#endif /* EnabledIndices_hpp */
