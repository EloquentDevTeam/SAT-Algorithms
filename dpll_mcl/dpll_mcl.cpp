#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <sstream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <memutils.h>
#include "dpll_utils.h"
#include "dpll_mcl.h"


namespace fs = std::filesystem;

int get_lit_total_count(const ClauseSet& c) {
    int lit_total_count = 0;

    for (const auto& clause : c) {
        if (clause.empty()) continue;

        int first_abs = std::abs(*clause.begin());
        int last_abs = std::abs(*clause.rbegin());

        int max_in_clause = std::max(first_abs, last_abs);
        if (max_in_clause > lit_total_count) {
            lit_total_count = max_in_clause;
        }
    }

    return lit_total_count;
}
[[nodiscard]] ClauseSet read_clauses(const char *file) {
    std::ifstream f(file);
    f.tie(nullptr);

    ClauseSet clauses;
    while (!f.eof()) {
        std::string line;
        std::getline(f,line);
        if (line.empty()) break;
        if (line =="%" || line=="0") continue;

        while (line.starts_with('c')) {
            std::getline(f,line);
        }
        if (line.starts_with("p cnf ")) {
            line = line.substr(strlen("p cnf "));
            std::istringstream is(line);
            continue;
        }

        std::istringstream is(line);
        Clause c;
        int64_t lit = 0;
        while (is>>lit) {
            if (lit == 0) break;
            c.emplace(lit);
        }
        clauses.emplace(c);
    }
    f.close();
    return clauses;
}

SatState dpll_mcl(ClauseSet c) {
    SatState state = SatState::UNKNOWN;
    int clause_total_count = c.size();
    int lit_total_count = get_lit_total_count(c);
    Sat s(clause_total_count, lit_total_count, c);

    state = det_satisfiability(s) == true ? SatState::SAT : SatState::UNSAT;

    return state;
}
/*
int main(int argc, char* argv[]) {
    if(argc != 3){
        std::cerr<<"Wrong input. Usage ./solver <path_to_cnf_file> <path_to_log_file>\n";
        return 1;
    }
    if(!fs::exists(argv[1])){
        std::cerr<<"File not found: "<< argv[1]<<'\n';
        return 1;
    }
    std::ofstream g(argv[2]);

    ClauseSet clauses = read_clauses(argv[1]);

    auto start = std::chrono::high_resolution_clock::now();
    g<<"Start SAT. Resultat: ";
    SatState result = dpll_mcl(clauses);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    size_t peakSize = getPeakRSS();

    switch (result) {
        case SatState::SAT:
            g<<"SAT";
            break;
        case SatState::UNSAT:
            g<<"UNSAT";
            break;
        case SatState::UNKNOWN:
            g<<"UNKNOWN";
            break;
    }

    g<<'\n';
    g<<"Timp de execuție: "<<elapsed<<"μs"<<'\n';
    g<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    g.close();
    return 0;
}
*/