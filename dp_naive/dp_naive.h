#ifndef SAT_ALGORITHMS_DP_NAIVE_H
#define SAT_ALGORITHMS_DP_NAIVE_H

#include "../Heuristics/HeuristicsDB_DP.h"

SatState dp(ClauseSet);
int get_lit_total_count(const ClauseSet&);
void build_heuristics_db(const ClauseSet&,HeuristicsDB_DP&);
void analyse(const Clause&,HeuristicsDB_DP&);

#endif //SAT_ALGORITHMS_DP_NAIVE_H