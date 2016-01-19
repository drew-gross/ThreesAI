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

Chromosome::Chromosome(array<float, CHROMOSOME_SIZE> weights) : weights(weights), functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) {
    
}

Chromosome::Chromosome(Chromosome::Mutate m, Chromosome const& c, default_random_engine& rng) : weights(c.weights), functions({
    [](BoardState const& b){return b.countOfTile(Tile::EMPTY);},
    [](BoardState const& b){return b.score();},
    [](BoardState const& b){return b.adjacentPairCount();},
    [](BoardState const& b){return b.splitPairCount();},
    [](BoardState const& b){return b.runRandomSimulation(0);},
    [](BoardState const& b){return b.adjacentOffByOneCount();},
}) {
    
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
    os << "{" << c.weights[0]
    << ", " << c.weights[1]
    << ", " << c.weights[2]
    << ", " << c.weights[3]
    << ", " << c.weights[4]
    << ", " << c.weights[5] << "}";
    return os;
}