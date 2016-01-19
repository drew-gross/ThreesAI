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
    std::vector<Chromosome> p;
    std::vector<float> scores;
    std::vector<std::pair<float, int>> sortResult;
public:
    friend std::ostream& operator<<(std::ostream &os, const Population d);
    Population(std::vector<Chromosome> p);
    Population& operator=(Population const& other);
    void populateScoresAndSort(int averageCount, std::default_random_engine& rng);
    Chromosome cross(int index1, int index2, std::default_random_engine& rng);
    Chromosome& get(int index);
};

std::ostream& operator<<(std::ostream &os, const Population d);

#endif /* Population_hpp */
