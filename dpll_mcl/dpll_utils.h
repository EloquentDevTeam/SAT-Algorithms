#ifndef DPLL_UTILS_H
#define DPLL_UTILS_H

#include <set>
#include <vector>
#include <cstdint>
#include "common.h"


using namespace std;

class Sat {
    int nrClauses;
    int nrMax;
    ClauseSet clauses;
public:
    Sat(int, int, const ClauseSet&);
    Sat(const Sat&);
    friend bool det_satisfiability(Sat&);
private:
    friend void solve_chosen_clause(Sat&, bool&, Literal);
    friend void solve_unit_clauses(Sat&, bool&);
    friend void max_freq(Literal&, int&, const Sat&);
};

#endif