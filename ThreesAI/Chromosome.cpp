//
//  Chromosome.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Chromosome.hpp"

#include "SimulatedBoardOutput.h"
#include "ZeroDepthAI.h"
#include "FixedDepthAI.hpp"
#include "Logging.h"

using namespace std;

Chromosome::Chromosome(vector<FuncAndWeight> c) : functions(c) {}

Chromosome::Chromosome(const Chromosome& that) {
    for (auto&& f : that.functions) {
        this->functions.push_back(f);
    }
}

size_t Chromosome::size() const {
    return this->functions.size();
}

FuncAndWeight Chromosome::getFun(uint8_t i) const {
    return this->functions.at(i);
}

Chromosome& Chromosome::operator=(Chromosome const& that) {
    this->functions = that.functions;
    return *this;
}

Chromosome Chromosome::cross(Chromosome const& other, default_random_engine& prng) const {
    uniform_int_distribution<bool> dist(0,1);
    
    vector<FuncAndWeight> newFunctions;
    newFunctions.reserve(other.size());
    for (int i = 0; i < other.size(); i++) {
        bool use1 = dist(prng);
        newFunctions.push_back(use1 ? this->getFun(i) : other.getFun(i));
    }
    
    return Chromosome(newFunctions);
}

Chromosome::Chromosome(Chromosome::Mutate m, Chromosome const& c, default_random_engine& rng) {
    unsigned long index = uniform_int_distribution<unsigned long>(0,c.functions.size() - 1)(rng);
    float newWeight = normal_distribution<>(0,20)(rng);
    
    this->functions = c.functions;
    this->functions[index].second = newWeight;
}

Heuristic Chromosome::to_f() const {
    Chromosome const* self = this;
    return [self](const BoardState & board){
        return accumulate(self->functions.begin(), self->functions.end(), 0, [&board](float prev, FuncAndWeight f){
            if (abs(f.second) < 1.f/100000000) {
                return prev;
            }
            return prev + f.second * f.first(board);
        });
    };
}

BoardState::Score Chromosome::score(unsigned int averageCount, prngSeed prngSeed) const {
    float totalScore = 0;
    default_random_engine prng(prngSeed.get());
    unsigned int origAverageCount = averageCount;
    while (averageCount > 0) {
        averageCount--;
        auto board = SimulatedBoardOutput::randomBoard(prng);
        prng.discard(1);
        BoardStateCPtr initialState = board->currentState(HiddenBoardState(0,1,1,1));
        FixedDepthAI ai(board->currentState(initialState->hiddenState), std::move(board), this->to_f(), 0);
        ai.playGame(false, false);
        totalScore += ai.currentState()->score();
    }
    return totalScore/origAverageCount;
}

ostream& operator<<(ostream &os, Chromosome const& c){
    os << "{";
    for (auto&& f : c.functions) {
        os << f.second << ", ";
    }
    os << "}";
    return os;
}