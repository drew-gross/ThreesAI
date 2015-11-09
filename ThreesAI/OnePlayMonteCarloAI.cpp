//
//  ZeroDepthMonteCarloAI.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/10/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#include "OnePlayMonteCarloAI.h"

#include "Logging.h"

using namespace std;

OnePlayMonteCarloAI::OnePlayMonteCarloAI(shared_ptr<BoardState const> board, unique_ptr<BoardOutput> output) : ThreesAIBase(board, move(output)) {}

void OnePlayMonteCarloAI::receiveState(Direction d, BoardState const & newState){}
void OnePlayMonteCarloAI::prepareDirection(){}

Direction OnePlayMonteCarloAI::getDirection() const {
    int bestScore = 0;
    Direction bestDirection = Direction::LEFT;
    vector<Direction> validMoves = this->currentState()->validMoves();
    for (Direction d : validMoves) {
        shared_ptr<BoardState> copyToExplore = make_shared<BoardState>(BoardState::DifferentFuture(1), *this->currentState());
        copyToExplore = make_shared<BoardState>(BoardState::Move(d), *copyToExplore);
        while (!copyToExplore->isGameOver()) {
            copyToExplore = make_shared<BoardState>(BoardState::Move(copyToExplore->randomValidMoveFromInternalGenerator()), *copyToExplore);
        }
        if (copyToExplore->score() > bestScore) {
            bestDirection = d;
            bestScore = copyToExplore->score();
        }
    }
    return bestDirection;
}