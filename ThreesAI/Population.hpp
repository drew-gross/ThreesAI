//
//  Population.hpp
//  ThreesAI
//
//  Created by Drew Gross on 12/30/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Population_hpp
#define Population_hpp

#include <vector>

#include "Chromosome.hpp"

class Population {
public:
    std::vector<Chromosome> p;
    Population& operator=(Population const& other);
};

#endif /* Population_hpp */
