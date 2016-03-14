//
//  Chromosome.cpp
//  ThreesAI
//
//  Created by Drew Gross on 12/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Chromosome.hpp"

#include <functional>

#include "Heuristic.hpp"
#include "Evaluators.hpp"
#include "SimulatedBoardOutput.h"
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

shared_ptr<Chromosome> Chromosome::cross(Chromosome const& other, default_random_engine& prng) const {
    uniform_int_distribution<bool> dist(0,1);
    
    vector<FuncAndWeight> newFunctions;
    newFunctions.reserve(other.size());
    for (int i = 0; i < other.size(); i++) {
        bool use1 = dist(prng);
        newFunctions.push_back(use1 ? this->getFun(i) : other.getFun(i));
    }
    
    return make_shared<Chromosome>(newFunctions);
}

Chromosome::Chromosome(Chromosome::Mutate m, Chromosome const& c, default_random_engine& rng) {
    unsigned long index = uniform_int_distribution<unsigned long>(0,c.functions.size() - 1)(rng);
    float newWeight = normal_distribution<>(0,20)(rng);
    
    this->functions = c.functions;
    this->functions[index].second = newWeight;
}

Heuristic Chromosome::to_f() const {
    Chromosome const* self = this;
    function<EvalutationWithDescription(BoardState const&)> f = [self](const BoardState & board){
        auto begin = self->functions.begin();
        auto end = self->functions.end();
        EvalutationWithDescription result = accumulate(begin, end, EvalutationWithDescription({0, ""}), [&board](EvalutationWithDescription prev, FuncAndWeight f){
            if (abs(f.second) < 1.f/100000000) {
                return prev;
            }
            auto result = f.first.evaluateWithDescription(board);
            auto weightedResult = f.second * result.score;
            float newValue = prev.score + weightedResult;
            string newDescription = prev.desciption + result.desciption + ": " + to_string(weightedResult) + "\n";
            return EvalutationWithDescription({newValue, newDescription});
        });
        result.desciption = to_string(result.score) + "\n" + result.desciption;
        return result;
    };
    return Heuristic(f);
}

BoardState::Score Chromosome::score(unsigned int averageCount, unsigned int searchDepth, prngSeed prngSeed) const {
    float totalScore = 0;
    default_random_engine prng(prngSeed.get());
    unsigned int origAverageCount = averageCount;
    while (averageCount > 0) {
        averageCount--;
        auto board = SimulatedBoardOutput::randomBoard(prng);
        prng.discard(1);
        BoardStateCPtr initialState = board->currentState(HiddenBoardState(0,1,1,1));
        FixedDepthAI ai(board->currentState(initialState->hiddenState), std::move(board), this->to_f(), searchDepth);
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