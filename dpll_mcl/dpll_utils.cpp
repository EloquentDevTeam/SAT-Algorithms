#include "dpll_utils.h"
#include <iostream>
#include <cmath>

Sat::Sat(int nrClauses, int nrMax, const ClauseSet &clauses) {
    this->nrClauses = nrClauses;
    this->nrMax = nrMax;
    this->clauses = clauses;
}

Sat::Sat(const Sat &s) {
    this->clauses = s.clauses;
    this->nrClauses = s.nrClauses;
    this->nrMax = s.nrMax;
}

void max_freq(Literal &max, int &freq, const Sat &s) {
    max = 0;
    freq = 0;

    std::vector<int> f(s.nrMax + 1, 0);

    for (const auto& clause : s.clauses) {
        for (Literal lit : clause) {
            Literal k = std::abs(lit);
            if (k <= s.nrMax) {
                f[k]++;
                if (f[k] > freq) {
                    max = k;
                    freq = f[k];
                }
            }
        }
    }
}

void solve_chosen_clause(Sat &s, bool &no_vid_clause, Literal clause) {
    ClauseSet new_clauses;

    for (const auto& c : s.clauses) {
        if (c.contains(clause)) {
            s.nrClauses--;
            continue;
        }

        if (c.contains(-clause)) {
            Clause temp = c;
            temp.erase(-clause);

            if (temp.empty()) {
                no_vid_clause = false;
                return;
            }
            new_clauses.insert(temp);
        } else {
            new_clauses.insert(c);
        }
    }

    s.clauses = std::move(new_clauses);
}

void solve_unit_clauses(Sat &s, bool &no_vid_clause) {
    bool no_change = true;
    while (no_change) {
        no_change = false;

        for (const auto& clause : s.clauses) {
            if (clause.size() == 1) {
                no_change = true;
                Literal unit_lit = *clause.begin();
                solve_chosen_clause(s, no_vid_clause, unit_lit);
                break;
            }
        }
        if (!no_vid_clause) break;
    }
}

bool det_satisfiability(Sat &s) {
    bool no_vid_clause = true;

    solve_unit_clauses(s, no_vid_clause);
    if (s.clauses.empty()) return true;
    if (!no_vid_clause) return false;

    Literal max_lit = 0;
    int freq = 0;
    max_freq(max_lit, freq, s);

    if (freq <= 1) return true;

    Sat copy(s);
    solve_chosen_clause(copy, no_vid_clause, max_lit);

    if (no_vid_clause && det_satisfiability(copy)) return true;

    no_vid_clause = true;
    solve_chosen_clause(s, no_vid_clause, -max_lit);

    if (!no_vid_clause) return false;
    return det_satisfiability(s);
}