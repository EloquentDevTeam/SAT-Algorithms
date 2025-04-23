#ifndef SAT_H
#define SAT_H
#include <vector>
using namespace std;

class Sat {
    int nrClauses;
    int nrMax;
    vector<vector<int>> clauses;
public:
    Sat(int,int,const vector<vector<int>>&);
    Sat(const Sat&);
    friend bool det_satisfiability(Sat&);
private:
    friend void solve_chosen_clause(Sat&, int);
    friend void solve_unit_clauses(Sat&);
    friend void max_freq(int&,int&,const Sat&);
};



#endif //SAT_H
