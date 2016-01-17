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

using namespace std;

Chromosome::Chromosome(array<double, CHROMOSOME_SIZE> weights) : weights(weights), functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) , cachedScore(0) {
    
}

Chromosome::Chromosome(Chromosome::Cross c, Chromosome const& l, Chromosome const& r, default_random_engine& rng) :  functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) , cachedScore(0) {
    uniform_int_distribution<bool> picker(0,1);
    array<double, CHROMOSOME_SIZE> new_weights = {
        picker(rng) ? r.weights[0] : l.weights[0],
        picker(rng) ? r.weights[1] : l.weights[1],
        picker(rng) ? r.weights[2] : l.weights[2],
        picker(rng) ? r.weights[3] : l.weights[3],
        picker(rng) ? r.weights[4] : l.weights[4],
        picker(rng) ? r.weights[5] : l.weights[5],
    };
    this->weights = new_weights;
}

Chromosome::Chromosome(Chromosome::Mutate m, Chromosome const& c, default_random_engine& rng) : weights(c.weights), functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) , cachedScore(0) {
    
    uniform_int_distribution<unsigned long> which_weight(0,CHROMOSOME_SIZE - 1);
    normal_distribution<> how_much(-10,20);
    
    auto index = which_weight(rng);  //A sequence point is necessary to force the RNG to get used in the right order.
    float newWeight = how_much(rng);
    
    this->weights[index] = newWeight;
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

BoardState::Score Chromosome::score(unsigned int averageCount) const {
    if (this->cachedScore > 0) {
        return this->cachedScore;
    }
    default_random_engine rng(0);
    while (averageCount > 0) {
        averageCount--;
    }
    auto board = SimulatedBoardOutput::randomBoard(default_random_engine(0));
    BoardStateCPtr initialState = board->currentState(HiddenBoardState(0,1,1,1));
    FixedDepthAI ai(board->currentState(initialState->hiddenState), std::move(board), this->to_f(), 0);
    ai.playGame(false, false);
    this->cachedScore = ai.currentState()->score();
    return this->cachedScore;
}

ostream& operator<<(ostream &os, Chromosome const& c){
    os << "Empty: " << c.weights[0]
    << " Score: " << c.weights[1]
    << " Mergable: " << c.weights[2]
    << " Split: " << c.weights[3]
    << " Monte Carlo: " << c.weights[4]
    << " Close: " << c.weights[5];
    return os;
}