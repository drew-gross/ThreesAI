//
//  Population.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/30/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Population.hpp"

using namespace std;

Population::Population(std::vector<Chromosome> p) : p(p){};

Population& Population::operator=(Population const& other) {
    this->p = other.p;
    return *this;
}

void Population::populateScoresAndSort(int averageCount, default_random_engine& rng) {
    this->scores.resize(this->p.size());
    for (int i = 0; i < this->p.size(); i++) {
        this->scores[i] = this->p[i].score(averageCount, rng);
        rng.discard(1);
    }
    sortResult.resize(this->p.size());
    for (int i = 0; i < this->p.size(); i++) {
        sortResult[i] = {this->scores[i], i};
    };
    
    sort(sortResult.begin(), sortResult.end(), [](pair<float, int> l, pair<float, int>r){
        return l.first > r.first;
    });
}

Chromosome Population::cross(int i1, int i2, default_random_engine& rng) {
    uniform_int_distribution<bool> dist(0,1);
    
    Chromosome pick1(this->p[this->sortResult[i1].second]);
    Chromosome pick2(this->p[this->sortResult[i2].second]);
    
    vector<FuncAndWeight> newFunctions;
    for (int i = 0; i < pick1.size(); i++) {
        bool use1 = dist(rng);
        newFunctions.push_back(use1 ? pick1.getFun(i) : pick2.getFun(i));
    }
    
    return Chromosome(newFunctions);
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