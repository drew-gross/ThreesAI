//
//  Chromosome.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Chromosome.hpp"

using namespace std;

Chromosome::Chromosome(array<double, CHROMOSOME_SIZE> weights) : weights(weights), functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) {
    
}

Heuristic Chromosome::to_f() const {
    return [this](const BoardState & board){
        float result = 0;
        for (char i = 0; i < CHROMOSOME_SIZE; i++) {
            result += this->weights[i] * this->functions[i](board);
        }
        return result;
    };
}

Chromosome Chromosome::cross_with(Chromosome const& other) const {
    array<double, CHROMOSOME_SIZE> new_weights = {
        //TODO pick randomly
        this->weights[0],
        other.weights[1],
        this->weights[2],
        other.weights[3],
        this->weights[4],
        other.weights[5],
    };
    return Chromosome(new_weights);
}

Chromosome Chromosome::mutate(default_random_engine& rng) const {
    uniform_int_distribution<unsigned long> which_weight(0,CHROMOSOME_SIZE - 1);
    uniform_real_distribution<> how_much(-1,2);
    
    auto index = which_weight(rng);  //A sequence point is necessary to force the RNG to get used in the right order.
    float newWeight = this->weights[index] * how_much(rng);
    
    Chromosome mutant(this->weights);
    mutant.weights[index] = newWeight;
    return mutant;
}

ostream& operator<<(ostream &os, const Chromosome c){
    os << "Empty: " << c.weights[0]
    << " Score: " << c.weights[1]
    << " Mergable: " << c.weights[2]
    << " Split: " << c.weights[3]
    << " Monte Carlo: " << c.weights[4]
    << " Close: " << c.weights[5];
    return os;
}