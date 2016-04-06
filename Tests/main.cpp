
#include <gtest/gtest.h>
#include <memory.h>

#include "Evaluators.hpp"
#include "Chromosome.hpp"
#include "Population.hpp"
#include "AdaptiveDepthAI.hpp"
#include "SimulatedBoardOutput.h"
#include "ManyPlayMonteCarloAI.h"
#include "FixedDepthAI.hpp"

using namespace std;
using namespace boost;

unique_ptr<SimulatedBoardOutput> makeOutput(string s) {
    return std::move(std::unique_ptr<SimulatedBoardOutput>(new SimulatedBoardOutput(std::make_shared<AboutToMoveBoard const>(s))));
}

TEST(countAdjacentPair, Works) {
    auto b = makeOutput("3,0,0,0,\
                        3,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0-2");
    EXPECT_EQ(countAdjacentPair(*b->sneakyState()), 1);
    b = makeOutput("3,0,0,0,\
                        2,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0-2");
    EXPECT_EQ(countAdjacentPair(*b->sneakyState()), 0);
    b = makeOutput("2,0,0,0,\
                        2,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0-2");
    EXPECT_EQ(countAdjacentPair(*b->sneakyState()), 0);
    b = makeOutput("2,0,2,1,\
                        1,0,0,0,\
                        0,0,1,0,\
                        12,12,2,0-2");
    EXPECT_EQ(countAdjacentPair(*b->sneakyState()), 4);
}

TEST(SpliPairCount, Works) {
    auto b = makeOutput("3,0,0,0,\
                        3,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0-2");
    EXPECT_EQ(countSplitPair(*b->sneakyState()), 0);
    b = makeOutput("3,0,0,0,\
                   0,0,0,0,\
                   0,0,0,0,\
                   0,0,0,0-2");
    EXPECT_EQ(countSplitPair(*b->sneakyState()), 0);
    b = makeOutput("3,0,0,0,\
                   1,0,0,0,\
                   0,0,0,0,\
                   0,0,0,0-2");
    EXPECT_EQ(countSplitPair(*b->sneakyState()), 0);
    
    b = makeOutput("3,0,0,0,\
                   0,0,0,0,\
                   3,0,0,0,\
                   0,0,0,0-2");
    EXPECT_EQ(countSplitPair(*b->sneakyState()), 2);
    b = makeOutput("3,0,3,0,\
                   3,0,0,0,\
                   0,0,0,0,\
                   3,0,3,0-2");
    EXPECT_EQ(countSplitPair(*b->sneakyState()), 3);
}

TEST(HighestOnCorner, Works) {
    AboutToMoveBoard b("0,0,1,0,\
                                        0,0,3,0,\
                                        96,0,24,0,\
                                        6,12,2,1-2");
    AboutToMoveBoard ul("96,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,1-2");
    AboutToMoveBoard ur("0,0,1,192,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,1-2");
    AboutToMoveBoard ll("0,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         1536,12,2,1-2");
    AboutToMoveBoard lr("0,0,1,0,\
                                         0,0,3,0,\
                                         96,0,24,0,\
                                         6,12,2,192-2");
    EXPECT_FALSE(highestIsInCorner(b));
    EXPECT_TRUE(highestIsInCorner(ul));
    EXPECT_TRUE(highestIsInCorner(ur));
    EXPECT_TRUE(highestIsInCorner(ll));
    EXPECT_TRUE(highestIsInCorner(lr));
}

TEST(AdaptiveDepthAI, CallsHeuristic) {
    BoardStateCPtr b = make_shared<AboutToMoveBoard const>("0,0,1,0,\
                                                                            0,0,3,0,\
                                                                            3,0,24,0,\
                                                                            6,3,2,1-2");
    int callCount = 0;
    std::function<EvalutationWithDescription(AboutToMoveBoard const&)> incrementer = [&callCount](AboutToMoveBoard const&){
        callCount++;
        EvalutationWithDescription result = {0, ""};
        return result;
    };
    FuncAndWeight f(make_shared<Heuristic>(incrementer), 1);
    std::vector<FuncAndWeight> v = {f};
    Chromosome c(v);
    c.to_f(false)->evaluateWithoutDescription(*b);
    EXPECT_EQ(callCount, 1);
    AdaptiveDepthAI ai(b, unique_ptr<BoardOutput>(new SimulatedBoardOutput(b)), c.to_f(false), 1);
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
    AboutToMoveBoard preMove("0,0,1,0,\
                                              0,0,3,0,\
                                              96,0,24,0,\
                                              6,12,2,1-2");
    AboutToAddTileBoard postMove = preMove.moveWithoutAdd(Direction::LEFT);
    AboutToMoveBoard expected("0,1,0,0,\
                                               0,3,0,0,\
                                               96,24,0,0,\
                                               6,12,3,0-3");
    EXPECT_TRUE(expected.hasSameTilesAs(postMove));
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
    AboutToMoveBoard b1("0,0,1,1,\
                                         0,0,1,1,\
                                         0,0,0,0,\
                                         0,0,0,0-1");
    AboutToAddTileBoard b2 = b1.moveWithoutAdd(Direction::LEFT);
    AboutToMoveBoard expected("0,1,1,0,\
                              0,1,1,0,\
                              0,0,0,0,\
                              0,0,0,0-1");
    EXPECT_TRUE(expected.hasSameTilesAs(b2));
    AboutToAddTileBoard b3 = expected.moveWithoutAdd(Direction::DOWN);
    expected = AboutToMoveBoard("0,0,0,0,\
                                0,1,1,0,\
                                0,1,1,0,\
                                0,0,0,0-1");
    EXPECT_TRUE(expected.hasSameTilesAs(b3));
    AboutToAddTileBoard b4 = expected.moveWithoutAdd(Direction::RIGHT);
    expected = AboutToMoveBoard("0,0,0,0,\
                                 0,0,1,1,\
                                 0,0,1,1,\
                                 0,0,0,0-1");
    EXPECT_TRUE(expected.hasSameTilesAs(b4));
    AboutToAddTileBoard b5 = expected.moveWithoutAdd(Direction::UP);
    expected = AboutToMoveBoard("0,0,1,1,\
                                0,0,1,1,\
                                0,0,0,0,\
                                0,0,0,0-1");
    EXPECT_TRUE(expected.hasSameTilesAs(b5));
}

TEST(FixedDepthAI, SearchesTheRightDepth) {
    int callCount = 0;
    std::function<EvalutationWithDescription(AboutToMoveBoard const&)> incrementer = [&callCount](AboutToMoveBoard const&){
        callCount++;
        EvalutationWithDescription result = {0, ""};
        return result;
    };
    Chromosome c({{make_shared<Heuristic>(incrementer), 1}});
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        0,3,0,0,\
                        0,0,0,0-2");
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 0);
    ai.playTurn();
    EXPECT_EQ(callCount, 16);
    
    callCount = 0;
    b = makeOutput("0,0,0,0,\
                   0,0,0,0,\
                   0,3,0,0,\
                   0,0,0,0-2");
    FixedDepthAI aiDepth1(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 1);
    aiDepth1.playTurn();
    EXPECT_EQ(callCount, 768);
    
    callCount = 0;
    b = makeOutput("0,0,0,0,\
                   0,0,0,0,\
                   0,3,0,0,\
                   0,0,0,0-2");
    FixedDepthAI aiDepth2(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 2);
    aiDepth2.playTurn();
    EXPECT_EQ(callCount, 36864);
}

TEST(HighestInCornerHeuristic, MovesALoneThreeToTheCorner) {
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        12,0,0,0,\
                        0,0,0,0-2");
    Chromosome c({{makeHeuristic(highestIsInCorner), 1}});
    FixedDepthAI aiDepth2(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 0);
    aiDepth2.playTurn();
    Tile cornerValue = aiDepth2.currentState()->at(BoardIndex(0,3));
    EXPECT_EQ(cornerValue, Tile::TILE_12);
}

TEST(HighestIsOnEdgeHeuristic, MovesFromCornerToEdge) {
    auto b = makeOutput("0,0,0,0,\
                        0,0,0,0,\
                        0,0,0,0,\
                        12,0,0,0-2");
    Chromosome c({{makeHeuristic(highestIsOnEdge), 1}});
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 0);
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
    Chromosome c({{makeHeuristic(highestIsOnEdge), 1}});
    FixedDepthAI ai(b->currentState(HiddenBoardState(0,4,4,3)), std::move(b), c.to_f(false), 0);
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
        {makeHeuristic(highestIsOnEdge), 0},
        {makeHeuristic(highestIsInCorner), 0},
        {makeHeuristic(score), 0}
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
        {makeHeuristic(highestIsOnEdge), 0},
        {makeHeuristic(highestIsInCorner), 0},
        {makeHeuristic(score), 0}
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

TEST(PopulationsFromChromosomes, SortThemselves) {
    unsigned int averageCount = 3;
    unsigned int pop_size = 10;
    unsigned int searchDepth = 0;
    Population p({
        makeHeuristic(score),
        makeHeuristic(countEmptyTile),
        makeHeuristic(countAdjacentPair),
        makeHeuristic(countSplitPair),
        makeHeuristic(countAdjacentOffByOne)
    }, pop_size, averageCount, searchDepth, prngSeed(1));
    EXPECT_EQ(pop_size, p.size());
    for (int i = 0; i < p.size() - 1; i++) {
        EXPECT_GT(p.get(i)->score(averageCount, 0, prngSeed(1)) + 1, p.get(i+1)->score(averageCount, 0, prngSeed(1)));
    }
}

TEST(ChromosomeCross, Crosses) {
    Chromosome c1({
        {makeHeuristic(score), 0},
        {makeHeuristic(countEmptyTile), 0},
        {makeHeuristic(countAdjacentPair), 0},
        {makeHeuristic(countAdjacentOffByOne), 0},
    });
    Chromosome c2({
        {makeHeuristic(score), 1},
        {makeHeuristic(countEmptyTile), 1},
        {makeHeuristic(countAdjacentPair), 1},
        {makeHeuristic(countAdjacentOffByOne), 1},
    });
    
    default_random_engine prng(0);
    auto cross = c1.cross(c2, prng);
    bool has1 = false;
    bool has0 = false;
    for (int i = 0; i < c1.size(); i++) {
        if (cross->getFun(i).second == 0) {
            has0 = true;
        }
        if (cross->getFun(i).second == 1) {
            has1 = true;
        }
    }
    EXPECT_TRUE(has1);
    EXPECT_TRUE(has0);
}

TEST(PopulationNext, MakesSense) {
    unsigned int averageCount = 3;
    unsigned int pop_size = 10;
    unsigned int searchDepth = 1;
    Population p({
        makeHeuristic(score),
        makeHeuristic(countEmptyTile),
        makeHeuristic(countAdjacentPair),
        makeHeuristic(countSplitPair),
        makeHeuristic(countAdjacentOffByOne)
    }, pop_size, averageCount, searchDepth, prngSeed(1));
    auto p2 = p.next(averageCount, searchDepth, prngSeed(3));
    EXPECT_EQ(p.size(), p2.size());
    auto p3 = p.next(averageCount, searchDepth, prngSeed(3));
    EXPECT_EQ(p2.size(), p3.size());
}

TEST(SameChromosomeWithDifferentSeeds, HaveDifferentScores) {
    Chromosome c({{makeHeuristic(score),1}});
    EXPECT_NE(c.score(1,0,prngSeed(1)), c.score(1,0,prngSeed(2)));
}

TEST(ChromosomeAveraging, GivesBetterResults) {
    Chromosome c({{makeHeuristic(score), 1}});
    EXPECT_NE(c.score(1, 0, prngSeed(1)), c.score(2, 0, prngSeed(1)));
}

TEST(DifferenctChromosome, GiveDifferentResults) {
    Chromosome c1({{makeHeuristic(score), 1}});
    Chromosome c2({{makeHeuristic(countEmptyTile), -1}});
    EXPECT_NE(c1.score(1, 0, prngSeed(1)), c2.score(1, 0, prngSeed(1)));
    EXPECT_NE(c1.score(1, 0, prngSeed(2)), c2.score(1, 0, prngSeed(2)));
    EXPECT_NE(c1.score(2, 0, prngSeed(1)), c2.score(2, 0, prngSeed(1)));
    EXPECT_NE(c1.score(2, 0, prngSeed(2)), c2.score(2, 0, prngSeed(2)));
}

TEST(PopulationNext, HasDifferentScore) {
    unsigned int popSize = 4;
    unsigned int averageCount = 1;
    unsigned int searchDepth = 2;
    Population p({makeHeuristic(score), makeHeuristic(countEmptyTile)}, popSize, averageCount, searchDepth, prngSeed(1));
    Population p2 = p.next(averageCount, searchDepth, prngSeed(1));
    EXPECT_NE(p.getScore(1), p2.getScore(1));
}

int main(int argc, char * argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
