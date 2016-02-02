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
#include <random>

#include "Chromosome.hpp"

class Population {
    std::vector<Chromosome> p;
    std::vector<float> scores;
    std::vector<std::pair<float, int>> sortResult;
    
    void populateScoresAndSort(int averageCount, unsigned int prngSeed);
public:
    
    size_t size();
    friend std::ostream& operator<<(std::ostream &os, const Population d);
    Population(std::vector<Chromosome> p, unsigned int averageCount, unsigned int prngSeed);
    Population(std::vector<Heuristic>, unsigned int size, unsigned int averageCount, unsigned int prngSeed);
    Population& operator=(Population const& other);
    Chromosome cross(int index1, int index2, std::default_random_engine& rng) const;
    Chromosome& get(int index);
    Population next(unsigned int averageCount, unsigned int prngSeed) const;
};

std::ostream& operator<<(std::ostream &os, const Population d);

#endif /* Population_hpp */
