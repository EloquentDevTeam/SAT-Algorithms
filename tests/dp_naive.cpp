#include <gtest/gtest.h>
#include <dp_naive.h>

TEST(HeuristicsTest, GetLitTotalCount_Standard) {
    ClauseSet cs = {
        {1, -2, 3},
        {-5, 4},
        {2}
    };

    EXPECT_EQ(get_lit_total_count(cs), 5);
}

TEST(HeuristicsTest, GetLitTotalCount_EmptyClauseSet) {
    ClauseSet cs;

    EXPECT_EQ(get_lit_total_count(cs), 0);
}

TEST(HeuristicsTest, DBIndexing_PositiveAndNegativeMapping) {
    HeuristicsDB_DP db(3);

    db[2]++;
    db[-2]++;
    db[-2]++;

    EXPECT_TRUE(db[2] == 1);
    EXPECT_TRUE(db[-2] == 2);
    EXPECT_EQ(db[2].get_occurrences(), 1);
    EXPECT_EQ(db[-2].get_occurrences(), 2);
    EXPECT_EQ(db[1].get_occurrences(), 0);
    EXPECT_EQ(db[-1].get_occurrences(), 0);
}

TEST(HeuristicsTest, AnalyseClause_UpdatesCountsAndLiterals) {
    HeuristicsDB_DP db(4);
    Clause c = {1, -3, 4};

    analyse(c, db);

    EXPECT_EQ(db[1].get_occurrences(), 1);
    EXPECT_EQ(db[-3].get_occurrences(), 1);
    EXPECT_EQ(db[4].get_occurrences(), 1);
    EXPECT_EQ(db[1].get_literal(), 1);
    EXPECT_EQ(db[-3].get_literal(), -3);
    EXPECT_EQ(db[-1].get_occurrences(), 0);
    EXPECT_EQ(db[3].get_occurrences(), 0);
}

TEST(HeuristicsTest, BuildHeuristicsDB_FullIntegration) {
    ClauseSet cs = {
        {1, -2},
        {-1, -2, 3},
        {3}
    };

    HeuristicsDB_DP db;
    build_heuristics_db(cs, db);

    EXPECT_EQ(db[1].get_occurrences(), 1);
    EXPECT_EQ(db[-1].get_occurrences(), 1);
    EXPECT_EQ(db[2].get_occurrences(), 0);
    EXPECT_EQ(db[-2].get_occurrences(), 2);
    EXPECT_EQ(db[3].get_occurrences(), 2);
    EXPECT_EQ(db[-3].get_occurrences(), 0);
}

TEST(HeuristicsTest, BuildHeuristicsDB_EmptyFormula) {
    ClauseSet cs;
    HeuristicsDB_DP db;

    build_heuristics_db(cs, db);

    SUCCEED();
}

TEST(DPTest, SATSmallSet) {
    ClauseSet clauses = {
        {1, 2, 3},
        {-1, 2},
        {-2, 3},
        {1, -3},
        {-1, -2, 3}
    };

    EXPECT_EQ(dp(clauses), SatState::SAT);
}

TEST(DPTest, UNSATSmallSet) {
    ClauseSet clauses = {
        {1, 2},
        {-1, 2},
        {1, -2, 3},
        {-3},
        {-1}
    };

    EXPECT_EQ(dp(clauses), SatState::UNSAT);
}

// 10 var, 15 clauze
TEST(DPTest, SATMediumSet) {
    ClauseSet clauses = {
        {1, 2, 3},
        {-1, -2, 4},
        {2, -3, 5},
        {-4, 6, 7},
        {-5, -6, -8},
        {8, 9, 10},
    };

    EXPECT_EQ(dp(clauses), SatState::SAT);
}