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

class prngSeed {
    int s;
public:
    explicit prngSeed(unsigned int val) {
        if (val == 0) {
            throw std::exception();
        }
        s = val;
    }
    unsigned int get() {return this->s;}
};

typedef std::pair<Heuristic, float> FuncAndWeight;

class Chromosome {
    std::vector<FuncAndWeight> functions;
    
    friend std::ostream& operator<<(std::ostream &os, Chromosome const& c);
    
public:
    ~Chromosome() {}
    size_t size() const;
    FuncAndWeight getFun(uint8_t i) const;
    
    class Mutate {
    public:
        Mutate() {};
    };
    
    explicit Chromosome(const Chromosome& that);
    Chromosome(Chromosome&& that) {debug();};
    Chromosome& operator=(Chromosome&& that) {debug();return *this;};
    Chromosome& operator=(Chromosome const& that);
    
    //Must cross with chromosome that has same functions in same order
    std::shared_ptr<Chromosome> cross(Chromosome const& other, std::default_random_engine& rng) const;
    Chromosome(Mutate m, Chromosome const& c, std::default_random_engine& rng);
    
    explicit Chromosome(std::vector<FuncAndWeight> weights);
    
    Heuristic to_f() const;
    BoardState::Score score(unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) const;
};

std::ostream& operator<<(std::ostream &os, Chromosome const& c);

#endif /* Chromosome_hpp */
