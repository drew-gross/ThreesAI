//
//  Chromosome.hpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef Chromosome_hpp
#define Chromosome_hpp

#include <array>
#include <functional>
#include <ostream>

#include "BoardState.h"
#include "Heuristic.hpp"

#define CHROMOSOME_SIZE 6

class Chromosome {
    std::array<double, CHROMOSOME_SIZE> weights;
    std::array<std::function<float(BoardState const&)>, CHROMOSOME_SIZE> functions;
    
    friend std::ostream& operator<<(std::ostream &os, const Chromosome c);
public:
    
    explicit Chromosome(std::array<double, CHROMOSOME_SIZE> weights);
    
    Heuristic to_f() const;
    Chromosome cross_with(Chromosome const&) const;
    Chromosome mutate(std::default_random_engine& rng) const;
};

std::ostream& operator<<(std::ostream &os, const Chromosome c);

#endif /* Chromosome_hpp */
