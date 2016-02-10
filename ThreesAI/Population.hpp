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
    std::vector<std::pair<std::shared_ptr<Chromosome>, float>> p;
    
    void initializeWithChromosomes(std::vector<std::shared_ptr<Chromosome>> c, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed);
public:
    
    size_t size();
    friend std::ostream& operator<<(std::ostream &os, const Population d);
    Population(std::vector<std::shared_ptr<Chromosome>> p, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed);
    Population(std::vector<Heuristic>, unsigned int size, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed);
    Population& operator=(Population const& other);
    std::shared_ptr<Chromosome> get(int index);
    float getScore(int index) const;
    Population next(unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) const;
};

std::ostream& operator<<(std::ostream &os, const Population d);

#endif /* Population_hpp */
