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
    
    friend std::ostream& operator<<(std::ostream &os, Chromosome const& c);
    
    mutable BoardState::Score cachedScore;
public:
    class Cross {
    public:
        Cross() {};
    };
    
    class Mutate {
    public:
        Mutate() {};
    };
    explicit Chromosome(const Chromosome& that) = default;
    Chromosome(Chromosome&& that) = default;
    Chromosome& operator=(Chromosome&& that) = default;
    Chromosome& operator=(Chromosome const& that) = default;
    Chromosome(Cross c, Chromosome const& l, Chromosome const& r, std::default_random_engine& rng);
    Chromosome(Mutate m, Chromosome const& c, std::default_random_engine& rng);
    
    explicit Chromosome(std::array<double, CHROMOSOME_SIZE> weights);
    
    Heuristic to_f() const;
    BoardState::Score score(unsigned int averageCount) const;
};

std::ostream& operator<<(std::ostream &os, Chromosome const& c);

#endif /* Chromosome_hpp */
