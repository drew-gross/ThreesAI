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

size_t Chromosome::size() {
    return this->functions.size();
}

FuncAndWeight Chromosome::getFun(uint8_t i) {
    return this->functions.at(i);
}

Chromosome& Chromosome::operator=(Chromosome const& that) {
    this->functions = that.functions;
    return *this;
}

Chromosome::Chromosome(Chromosome::Mutate m, Chromosome const& c, default_random_engine& rng) {
    uniform_int_distribution<unsigned long> which_weight(0,c.functions.size());
    normal_distribution<> how_much(0,20);
    
    auto index = which_weight(rng);  //A sequence point is necessary to force the RNG to get used in the right order.
    float newWeight = how_much(rng);
    
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

BoardState::Score Chromosome::score(unsigned int averageCount, default_random_engine& rng) const {
    float totalScore = 0;
    unsigned int origAverageCount = averageCount;
    while (averageCount > 0) {
        averageCount--;
        auto board = SimulatedBoardOutput::randomBoard(rng);
        rng.discard(1);
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