#include <gtest/gtest.h>
#include <resolution_naive_first_fit.h>

TEST(ResolutionUtilsTest, CanJoin_O_SinguraPerecheComplementara) {
    Clause c1 = {1, 2, 3};
    Clause c2 = {-1, 4, 5};

    auto result = can_join(c1, c2);

    EXPECT_TRUE(result.first);
    EXPECT_EQ(result.second, 1);
}

TEST(ResolutionUtilsTest, CanJoin_FaraPerechiComplementare) {
    Clause c1 = {1, 2};
    Clause c2 = {3, 4};

    auto result = can_join(c1, c2);

    EXPECT_FALSE(result.first);
}

TEST(ResolutionUtilsTest, CanJoin_MaiMultePerechiComplementare) {
    Clause c1 = {1, 2};
    Clause c2 = {-1, -2};

    auto result = can_join(c1, c2);

    EXPECT_FALSE(result.first);
}

TEST(ResolutionUtilsTest, CanJoin_AceeasiPolaritate) {
    Clause c1 = {1, 2};
    Clause c2 = {1, 3};

    auto result = can_join(c1, c2);

    EXPECT_FALSE(result.first);
}

TEST(ResolutionUtilsTest, CanJoin_ClauzeVide) {
    Clause c1 = {};
    Clause c2 = {};

    auto result = can_join(c1, c2);

    EXPECT_FALSE(result.first);
}

TEST(ResolutionUtilsTest, Join_GenerareClauzaNouaStandard) {
    Clause c1 = {1, 2};
    Clause c2 = {-1, 3};
    Literal pivot = 1;

    Clause expected = {2, 3};

    EXPECT_EQ(join(c1, c2, pivot), expected);
}

TEST(ResolutionUtilsTest, Join_GenerareClauzaVida_Contradictie) {
    Clause c1 = {1};
    Clause c2 = {-1};
    Literal pivot = 1;

    Clause expected = {};

    EXPECT_EQ(join(c1, c2, pivot), expected);
}

TEST(ResolutionUtilsTest, Join_EliminaDuplicateleSetului) {
    Clause c1 = {1, 2, 5};
    Clause c2 = {-1, 3, 5};
    Literal pivot = 1;

    Clause expected = {2, 3, 5};

    EXPECT_EQ(join(c1, c2, pivot), expected);
}

TEST(ResolutionUtilsTest, Join_PivotNegativCaParametru) {
    Clause c1 = {-2, 4};
    Clause c2 = {2, 5};

    Clause expected = {4, 5};

    EXPECT_EQ(join(c1, c2, -2), expected);
    EXPECT_EQ(join(c1, c2, 2), expected);
}

TEST(ResolutionTest, SATSmallSet) {
    ClauseSet clauses = {
        {1, 2, 3},
        {-1, 2},
        {-2, 3},
        {1, -3},
        {-1, -2, 3}
    };

    EXPECT_EQ(res(clauses), SatState::SAT);
}

TEST(ResolutionTest, UNSATSmallSet) {
    ClauseSet clauses = {
        {1, 2},
        {-1, 2},
        {1, -2, 3},
        {-3},
        {-1}
    };

    EXPECT_EQ(res(clauses), SatState::UNSAT);
}

// 10 var, 6 clauze
TEST(ResolutionTest, SATMediumSet) {
    ClauseSet clauses = {
        {1, 2, 3},
        {-1, -2, 4},
        {2, -3, 5},
        {-4, 6, 7},
        {-5, -6, -8},
        {8, 9, 10},
    };

    EXPECT_EQ(res(clauses), SatState::SAT);
}