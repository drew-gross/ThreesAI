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
    std::array<std::function<float(BoardState const&)>, CHROMOSOME_SIZE> functions;
    
    friend std::ostream& operator<<(std::ostream &os, Chromosome const& c);
    
public:
    std::array<float, CHROMOSOME_SIZE> weights;
    
    class Mutate {
    public:
        Mutate() {};
    };
    explicit Chromosome(const Chromosome& that) = default;
    Chromosome(Chromosome&& that) = default;
    Chromosome& operator=(Chromosome&& that) = default;
    Chromosome& operator=(Chromosome const& that) = default;
    Chromosome(Mutate m, Chromosome const& c, std::default_random_engine& rng);
    
    explicit Chromosome(std::array<float, CHROMOSOME_SIZE> weights);
    
    Heuristic to_f() const;
    BoardState::Score score(unsigned int averageCount, std::default_random_engine& rng) const;
};

std::ostream& operator<<(std::ostream &os, Chromosome const& c);

#endif /* Chromosome_hpp */
