#ifndef SAT_ALGORITHMS_HEURISTICSDB_DP_H
#define SAT_ALGORITHMS_HEURISTICSDB_DP_H

#include "HeuristicEntryDP.h"
#include <vector>

class HeuristicsDB_DP {
    std::vector<HeuristicEntryDP> vector;
    size_t size = 0;
public:
    explicit HeuristicsDB_DP(const size_t size);
    explicit HeuristicsDB_DP() = default;
    HeuristicEntryDP& operator[](Literal lit);
    std::vector<HeuristicEntryDP>::iterator begin();
    std::vector<HeuristicEntryDP>::iterator end();

    void resize(const size_t new_size);
};

#endif //SAT_ALGORITHMS_HEURISTICSDB_DP_H