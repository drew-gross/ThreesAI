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

#define CHROMOSOME_SIZE 8

typedef std::function<float(BoardState const&)> BoardEvaluator;
typedef std::pair<BoardEvaluator, float> FuncAndWeight;

class Chromosome {
    std::vector<FuncAndWeight> functions;
    
    friend std::ostream& operator<<(std::ostream &os, Chromosome const& c);
    
public:
    ~Chromosome() {}
    size_t size();
    FuncAndWeight getFun(uint8_t i);
    
    class Mutate {
    public:
        Mutate() {};
    };
    
    explicit Chromosome(const Chromosome& that);
    Chromosome(Chromosome&& that) {debug();};
    Chromosome& operator=(Chromosome&& that) {debug();return *this;};
    Chromosome& operator=(Chromosome const& that);
    Chromosome(Mutate m, Chromosome const& c, std::default_random_engine& rng);
    
    explicit Chromosome(std::vector<FuncAndWeight> weights);
    
    Heuristic to_f() const;
    BoardState::Score score(unsigned int averageCount, std::default_random_engine& rng) const;
};

std::ostream& operator<<(std::ostream &os, Chromosome const& c);

#endif /* Chromosome_hpp */
