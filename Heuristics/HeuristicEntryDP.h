#ifndef SAT_ALGORITHMS_HEURISTICENTRYDP_H
#define SAT_ALGORITHMS_HEURISTICENTRYDP_H

#include "common.h"

class HeuristicEntryDP {
    Literal literal;
    size_t occurrences;
public:
    explicit HeuristicEntryDP(Literal l, size_t occurences = 0);
    HeuristicEntryDP();
    friend auto operator<=>(const HeuristicEntryDP &lhs, size_t rhs) {
        return lhs.occurrences <=> rhs;
    }

    friend bool operator==(const HeuristicEntryDP& lhs, size_t i) {
        return lhs.occurrences == i;
    }
    HeuristicEntryDP operator++(int);
    HeuristicEntryDP &operator++();
    HeuristicEntryDP &operator--();
    HeuristicEntryDP operator--(int);
    [[nodiscard]] Literal get_literal() const;
    [[nodiscard]] size_t get_occurrences() const;
    void set_literal(const Literal l);
};

#endif //SAT_ALGORITHMS_HEURISTICENTRYDP_H