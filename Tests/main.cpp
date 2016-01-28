
#include <gtest/gtest.h>
#include <memory.h>

#include "Evaluators.hpp"
#include "Chromosome.hpp"
#include "AdaptiveDepthAI.hpp"
#include "SimulatedBoardOutput.h"
#include "ManyPlayMonteCarloAI.h"
#include "FixedDepthAI.hpp"

using namespace std;
using namespace boost;

unique_ptr<SimulatedBoardOutput> makeOutput(string s) {
    return std::move(std::unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(std::make_shared<BoardState const>(BoardState::FromString(s)))));
}

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
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        0,3,0,0,\
                        0,0,0,0-2");
    ManyPlayMonteCarloAI ai(b->currentState(HiddenBoardState(0,4,4,4)), std::move(b), 2);
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

TEST(FixedDepthAI, SearchesTheRightDepth) {
    int callCount = 0;
    auto incrementer = [&callCount](BoardState const&){
        callCount++;
        return 0;
    };
    Chromosome c({{incrementer, 1}});
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        0,3,0,0,\
                        0,0,0,0-2");
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 0);
    ai.playTurn();
    EXPECT_EQ(callCount, 16);
    
    callCount = 0;
    b = makeOutput("0,0,0,0,\
                   0,0,0,0,\
                   0,3,0,0,\
                   0,0,0,0-2");
    FixedDepthAI aiDepth1(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 1);
    aiDepth1.playTurn();
    EXPECT_EQ(callCount, 768);
    
    callCount = 0;
    b = makeOutput("0,0,0,0,\
                   0,0,0,0,\
                   0,3,0,0,\
                   0,0,0,0-2");
    FixedDepthAI aiDepth2(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 2);
    aiDepth2.playTurn();
    EXPECT_EQ(callCount, 36864);
}

TEST(HighestInCornerHeuristic, MovesALoneThreeToTheCorner) {
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        12,0,0,0,\
                        0,0,0,0-2");
    Chromosome c({{highestIsInCorner, 1}});
    FixedDepthAI aiDepth2(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 0);
    aiDepth2.playTurn();
    Tile cornerValue = aiDepth2.currentState()->at(BoardIndex(0,3));
    EXPECT_EQ(cornerValue, Tile::TILE_12);
}

TEST(HighestIsOnEdgeHeuristic, MovesFromCornerToEdge) {
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0,\
                        12,0,0,0-2");
    Chromosome c({{highestIsOnEdge, 1}});
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 0);
    ai.playTurn();
    Tile e1 = ai.currentState()->at(BoardIndex(0,2));
    Tile e2 = ai.currentState()->at(BoardIndex(1,3));
    EXPECT_TRUE(e1 == Tile::TILE_12 || e2 == Tile::TILE_12);
}

TEST(HighestIsOnEdgeHeuristic, MovesFromEdgeToEdge) {
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        12,0,0,0,\
                        0,0,0,0-2");
    Chromosome c({{highestIsOnEdge, 1}});
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(), 0);
    ai.playTurn();
    Tile e1 = ai.currentState()->at(BoardIndex(0,1));
    EXPECT_EQ(e1, Tile::TILE_12);
}

TEST(TrappedCount, Works) {
    auto b = makeOutput("0,0,0,12,\
                        0,0,0,3,\
                        3,12,0,12,\
                        0,0,0,0-2");
    EXPECT_EQ(countTrappedTiles(*b->sneakyState()), 2);
}

TEST(TrappedCount, DealsWith1sAnd2s) {
    auto b = makeOutput("0,0,0,2,\
                        0,0,0,3,\
                        2,1,0,2,\
                        0,0,0,1-2");
    EXPECT_EQ(countTrappedTiles(*b->sneakyState()), 1);
}

TEST(Mutating, MutatesOneValue) {
    Chromosome c({
        {highestIsOnEdge, 0},
        {highestIsInCorner, 0},
        {score, 0}
    });
    for (int rng_init = 0; rng_init < 1000; rng_init++) {
        default_random_engine rng(rng_init);
        Chromosome m(Chromosome::Mutate(), c, rng);
        int differentCount = 0;
        for (int i = 0; i < m.size(); i++) {
            if (m.getFun(i).second != c.getFun(i).second) {
                differentCount++;
            }
        }
        EXPECT_EQ(differentCount, 1);
    }
}

TEST(Mutating, MutatesEachValueEqually) {
    Chromosome c({
        {highestIsOnEdge, 0},
        {highestIsInCorner, 0},
        {score, 0}
    });
    vector<unsigned int> differentCount;
    differentCount.resize(c.size());
    for (int rng_init = 0; rng_init < 1000; rng_init++) {
        default_random_engine rng(rng_init);
        Chromosome m(Chromosome::Mutate(), c, rng);
        for (int i = 0; i < m.size(); i++) {
            if (m.getFun(i).second != c.getFun(i).second) {
                differentCount[i]++;
            }
        }
    }
    EXPECT_EQ(differentCount[0] + differentCount[1] + differentCount[2], 1000);
    unsigned int max = *max_element(differentCount.begin(), differentCount.end());
    unsigned int min = *min_element(differentCount.begin(), differentCount.end());
    EXPECT_LE(max/min, 1.1);
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
