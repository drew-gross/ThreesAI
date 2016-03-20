//
//  Population.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/30/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Population.hpp"
#include "Logging.h"

using namespace std;

void Population::initializeWithChromosomes(vector<std::shared_ptr<Chromosome>> cs, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) {
    this->p.reserve(cs.size());
    for (auto&& c : cs) {
        this->p.emplace_back(c, c->score(averageCount, searchDepth, prngSeed));
    }
    sort(this->p.begin(), this->p.end(), [](pair<std::shared_ptr<Chromosome>, float> l, pair<std::shared_ptr<Chromosome>, float> r){
        return l.second > r.second;
    });
}

Population::Population(std::vector<std::shared_ptr<Chromosome>> p, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) {
    this->initializeWithChromosomes(p, averageCount, searchDepth, prngSeed);
};

size_t Population::size() {
    return this->p.size();
}

float Population::getScore(int index) const {
    return this->p[index].second;
}

Population::Population(vector<shared_ptr<Heuristic>> funcs, unsigned int size, unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) {
    default_random_engine initial_population_generator;
    normal_distribution<float> population_dist(0,5);
    
    vector<std::shared_ptr<Chromosome>> cs;
    while (size--) {
        vector<FuncAndWeight> weights;
        weights.reserve(funcs.size());
        for (auto&& func : funcs) {
            weights.push_back({func, population_dist(initial_population_generator)});
        }
        cs.emplace_back(shared_ptr<Chromosome>(new Chromosome(weights)));
    }
    this->initializeWithChromosomes(cs, averageCount, searchDepth, prngSeed);
}

Population Population::next(unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) const {
    default_random_engine prng(prngSeed.get());
    vector<shared_ptr<Chromosome>> next;
    shared_ptr<Chromosome> c = make_shared<Chromosome>(Chromosome::Mutate(), *this->p.front().first, prng);
    next.push_back(c);
    next.push_back(this->p[0].first);
    for (unsigned int i = 0; i < this->p.size(); i++) {
        unsigned int parent1 = i;
        unsigned int parent2 = i+1;
        if (parent2 == this->p.size()) {
            parent2 = 1;
        }
        auto candidate = this->p[parent1].first->cross(*this->p[parent2].first, prng);
        next.push_back(candidate);
        if (this->p.size() == next.size()) {
            return Population(next, averageCount, searchDepth, prngSeed);
        }
    }
    
    return Population(next, averageCount, searchDepth, prngSeed);
}

Population& Population::operator=(Population const& other) {
    this->p = other.p;
    return *this;
}

std::shared_ptr<Chromosome> Population::get(int i) {
    return this->p[i].first;
}

ostream& operator<<(ostream &os, const Population p){
    for (auto&& pair : p.p) {
        os << "Score: " << pair.second << " " << *pair.first << endl;
    }
    return os;
}