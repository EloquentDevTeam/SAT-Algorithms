#ifndef SAT_ALGORITHMS_COMMON_TYPES_H
#define SAT_ALGORITHMS_COMMON_TYPES_H

#include <set>
#include <filesystem>

enum class SatState {
    SAT,
    UNSAT,
    UNKNOWN
};

using Literal = int64_t;
using Clause = std::set<Literal>;
using ClauseSet = std::set<Clause>;

#endif //SAT_ALGORITHMS_COMMON_TYPES_H