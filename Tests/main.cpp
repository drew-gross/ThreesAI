
#include <gtest/gtest.h>

#include "Evaluators.hpp"
#include "Chromosome.hpp"
#include "AdaptiveDepthAI.hpp"
#include "SimulatedBoardOutput.h"
#include "ManyPlayMonteCarloAI.h"

using namespace std;
using namespace boost;

TEST(HighestOnCorner, Works) {
    BoardState b(BoardState::FromString("0,0,1,0,\
                                        0,0,3,0,\
                                        96,0,24,0,\
                                        6,12,2,1-2"));
    BoardState ul(BoardState::FromString("96,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,1-2"));
    BoardState ur(BoardState::FromString("0,0,1,192,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,1-2"));
    BoardState ll(BoardState::FromString("0,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         1536,12,2,1-2"));
    BoardState lr(BoardState::FromString("0,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,192-2"));
    EXPECT_FALSE(highestIsInCorner(b));
    EXPECT_TRUE(highestIsInCorner(ul));
    EXPECT_TRUE(highestIsInCorner(ur));
    EXPECT_TRUE(highestIsInCorner(ll));
    EXPECT_TRUE(highestIsInCorner(lr));
}

TEST(AdaptiveDepthAI, CallsHeuristic) {
    BoardStateCPtr b = make_shared<BoardState const>(BoardState::FromString("0,0,1,0,\
                                                                            0,0,3,0,\
                                                                            3,0,24,0,\
                                                                            6,3,2,1-2"));
    int callCount = 0;
    auto incrementer = [&callCount](BoardState const&){
        callCount++;
        return 0;
    };
    Chromosome c({{incrementer, 1}});
    c.to_f()(*b);
    EXPECT_EQ(callCount, 1);
    AdaptiveDepthAI ai(b, unique_ptr<BoardOutput>(new SimulatedBoardOutput(b)), c.to_f(), 1);
    ai.playTurn();
    EXPECT_EQ(callCount, 427);
}

TEST(MonteCarloAI, PicksDown) {
    std::unique_ptr<SimulatedBoardOutput> board = std::unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(make_shared<BoardState const>(BoardState::FromString("2,6,3,1,\
                                                                                                                                                                      3,24,384,6,\
                                                                                                                                                                      6,24,96,192,\
                                                                                                                                                                      3,6,1,3-2"))));
    ManyPlayMonteCarloAI ai(board->currentState(HiddenBoardState(0,4,4,4)), std::move(board), 2);
    EXPECT_EQ(ai.getDirection(), Direction::UP);
}

TEST(MovementAndFinding, Work) {
    BoardState preMove(BoardState::FromString("0,0,1,0,\
                                              0,0,3,0,\
                                              96,0,24,0,\
                                              6,12,2,1-2"));
    BoardState postMove(BoardState::MoveWithoutAdd(Direction::LEFT), preMove);
    BoardState expected(BoardState::FromString("0,1,0,0,\
                                               0,3,0,0,\
                                               96,24,0,0,\
                                               6,12,3,0-3"));
    EXPECT_TRUE(postMove.hasSameTilesAs(expected, {}));
    BoardIndex i(1,2);
    EXPECT_EQ(postMove.at(i), Tile::TILE_24);
    EXPECT_EQ(postMove.at(i.left().get()), Tile::TILE_96);
    EXPECT_EQ(postMove.at(i.up().get()), Tile::TILE_3);
    EXPECT_EQ(postMove.at(i.down().get()), Tile::TILE_12);
    EXPECT_EQ(postMove.at(i.right().get()), Tile::EMPTY);
    EXPECT_EQ(BoardIndex(0,0).left(), none);
    EXPECT_EQ(BoardIndex(0,0).up(), none);
    EXPECT_EQ(BoardIndex(3,3).right(), none);
    EXPECT_EQ(BoardIndex(3,3).down(), none);
}


TEST(Movement, Works) {
    BoardState b1(BoardState::FromString("0,0,1,1,\
                                         0,0,1,1,\
                                         0,0,0,0,\
                                         0,0,0,0-1"));
    BoardState b2(BoardState::MoveWithoutAdd(Direction::LEFT), b1);
    EXPECT_TRUE(b2.hasSameTilesAs(BoardState::FromString("0,1,1,0,\
                                                    0,1,1,0,\
                                                    0,0,0,0,\
                                                    0,0,0,0-1"), {}));
    BoardState b3(BoardState::MoveWithoutAdd(Direction::DOWN), b2);
    EXPECT_TRUE(b3.hasSameTilesAs(BoardState::FromString("0,0,0,0,\
                                                    0,1,1,0,\
                                                    0,1,1,0,\
                                                    0,0,0,0-1"), {}));
    BoardState b4(BoardState::MoveWithoutAdd(Direction::RIGHT), b3);
    EXPECT_TRUE(b4.hasSameTilesAs(BoardState::FromString("0,0,0,0,\
                                                    0,0,1,1,\
                                                    0,0,1,1,\
                                                    0,0,0,0-1"), {}));
    BoardState b5(BoardState::MoveWithoutAdd(Direction::UP), b4);
    EXPECT_TRUE(b5.hasSameTilesAs(BoardState::FromString("0,0,1,1,\
                                                    0,0,1,1,\
                                                    0,0,0,0,\
                                                    0,0,0,0-1"), {}));
    
    
    BoardState x(BoardState::FromString("6,0,0,0,\
                                        0,0,1,0,\
                                        0,0,6,0,\
                                        0,6,0,0-1"));
    
    BoardState y(BoardState::FromString("3,0,0,0,\
                                        0,0,1,0,\
                                        0,0,3,0,\
                                        0,0,2,0-1"));
    EXPECT_TRUE(x.hasSameTilesAs(y, {{0,0}, {1,3}, {2,2}, {2,3}}));
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
