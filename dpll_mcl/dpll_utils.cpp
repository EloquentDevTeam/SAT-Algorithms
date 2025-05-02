#include "dpll_utils.h"
#include <iostream>

Sat::Sat(int nrClauses, int nrMax,const vector<vector<int>> &clauses) {
    this->nrClauses = nrClauses;
    this->nrMax = nrMax;
    this->clauses = clauses;
}

Sat::Sat(const Sat &s) {
    clauses = s.clauses;
    nrClauses = s.nrClauses;
    nrMax = s.nrMax;
}


void max_freq(int &max, int &freq, const Sat &s) {
    max = 0;
    freq = 0;
    int *f = new int[s.nrMax+1]();
    for (int i = 0; i < s.nrClauses; i++) {
        for (int j = 0; s.clauses[i][j] != 0; j++) {
            int k = s.clauses[i][j];
            k = k < 0 ? -k : k;
            f[k]++;
            if (f[k] > freq) {
                max = k;
                freq = f[max];
            }
        }
    }
    delete[] f;
}

void solve_chosen_clause(Sat &s,bool &no_vid_clause, int clause) {
    for (int i = 0; i < s.nrClauses; i++) {
        bool no_match = true;
        for (int j = 0; no_vid_clause and no_match and s.clauses[i][j] != 0; j++) {
            if (s.clauses[i][j] == clause) {
                no_match = false;
                s.clauses.erase(s.clauses.begin() + i);
                i--;
                s.nrClauses--;
            }
            else if (s.clauses[i][j] == -clause) {
                no_match = false;
                s.clauses[i].erase(s.clauses[i].begin() + j);
                if (s.clauses[i][0] == 0) no_vid_clause = false;
            }
        }
    }
}


void solve_unit_clauses(Sat &s, bool &no_vid_clause) {
    bool no_change = true;
    while (no_change) {
        no_change = false;
        for (int i = 0; i < s.nrClauses and !no_change and no_vid_clause; i++)
            if (s.clauses[i][1] == 0) {
                no_change = true;
                solve_chosen_clause(s,no_vid_clause,s.clauses[i][0]);
            }
    }
}

bool det_satisfiability(Sat &s) {
    bool no_vid_clause = true;
    solve_unit_clauses(s,no_vid_clause);
    if (s.nrClauses == 0) return true;
    int max=0, freq=0;
    max_freq(max, freq, s);
    if (freq == 1) return true;
    Sat copy(s);
    solve_chosen_clause(copy,no_vid_clause,max);
    if (no_vid_clause == false) return false;
    if (det_satisfiability(copy)) return true;
    solve_chosen_clause(s,no_vid_clause,-max);
    if (no_vid_clause == false) return false;
    return det_satisfiability(s);
}
