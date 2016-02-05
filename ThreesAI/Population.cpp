//
//  Population.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/30/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Population.hpp"

using namespace std;

Population::Population(std::vector<Chromosome> p, unsigned int averageCount, prngSeed prngSeed) : p(p){
    this->populateScoresAndSort(averageCount, prngSeed);
};

size_t Population::size() {
    return this->p.size();
}

Population::Population(vector<Heuristic> funcs, unsigned int size, unsigned int averageCount, prngSeed prngSeed) {
    default_random_engine initial_population_generator;
    normal_distribution<float> population_dist(0,5);
    for (int i = 0; i < size; i++) {
        vector<FuncAndWeight> weights;
        weights.reserve(funcs.size());
        for (auto&& func : funcs) {
            weights.push_back({func, population_dist(initial_population_generator)});
        }
        p.emplace_back(weights);
    }
    this->populateScoresAndSort(averageCount, prngSeed);
}

Population Population::next(unsigned int averageCount, prngSeed prngSeed) const {
    default_random_engine prng(prngSeed.get());
    vector<Chromosome> next;
    next.emplace_back(Chromosome::Mutate(), this->p.front(), prng);
    next.push_back(this->p[0]);
    for (unsigned int i = 0; i < this->p.size(); i++) {
        unsigned int parent1 = i;
        unsigned int parent2 = i+1;
        if (parent2 == this->p.size()) {
            parent2 = 1;
        }
        Chromosome candidate = this->p[parent1].cross(this->p[parent2], prng);
        next.push_back(candidate);
        if (this->p.size() == next.size()) {
            return Population(next, averageCount, prngSeed);
        }
    }
    
    return Population(next, averageCount, prngSeed);
}

Population& Population::operator=(Population const& other) {
    this->p = other.p;
    return *this;
}

void Population::populateScoresAndSort(int averageCount, prngSeed prngSeed) {
    this->scores.resize(this->p.size());
    for (int i = 0; i < this->p.size(); i++) {
        this->scores[i] = this->p[i].score(averageCount, prngSeed);
    }
    sortResult.resize(this->p.size());
    for (int i = 0; i < this->p.size(); i++) {
        sortResult[i] = {this->scores[i], i};
    };
    
    sort(sortResult.begin(), sortResult.end(), [](pair<float, int> l, pair<float, int>r){
        return l.first > r.first;
    });
}

Chromosome& Population::get(int i) {
    return this->p[this->sortResult[i].second];
}

ostream& operator<<(ostream &os, const Population p){
    debug(p.sortResult.empty());
    for (auto&& pair : p.sortResult) {
        os << "Score: " << p.scores[pair.second] << " " << p.p[pair.second] << endl;
    }
    return os;
}