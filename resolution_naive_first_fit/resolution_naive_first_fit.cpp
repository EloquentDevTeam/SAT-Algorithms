#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <memutils.h>
#include <csignal>


#pragma GCC optimize("O3,fast-math,unroll-loops")


using Literal = int64_t;
using Clause = std::set<Literal>;
using ClauseSet = std::set<Clause>;

ClauseSet clauses;

constexpr std::size_t THRESHOLD = 71000000;

void onCtrlC(int sig) {
    std::cerr<<"Programul a primit semnalul "<<sig<<"\n Rezultat: UNKNOWN.\n Nr. de clauze totale: "<<clauses.size()<<'\n';
    size_t peakSize    = getPeakRSS( );
    std::cerr<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    exit(1);
}
[[nodiscard]] ClauseSet read_clauses(const char *file) {
    std::ifstream f(file);
    f.tie(nullptr);
    size_t clause_count=0, lit_total_count=0;
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
            is>>clause_count>>lit_total_count;
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

enum class SatState {
    SAT,
    UNSAT,
    UNKNOWN
};

std::pair<bool,int64_t> can_join(const Clause& c1, const Clause& c2) {
    size_t pairs = 0;
    int64_t lit = 0;
    for (auto& literal: c1) {
        if (c2.contains(literal*-1)) {
            ++pairs;
            lit = literal;
        }
    }
    return {pairs == 1,lit};
}
Clause join(const Clause& c1, const Clause& c2, const Literal l) {
    Clause c12;
    for (auto& lit : c1) {
        c12.emplace(lit);
    }
    for (auto& lit : c2) {
        c12.emplace(lit);
    }
    c12.erase(l);
    c12.erase(-l);
    return c12;
}
[[nodiscard]] SatState resolution(ClauseSet& cs) {
    bool canMakeNewClause = false;
    do
    {
        canMakeNewClause = false;
        size_t iindex = 0,jindex=0;
        for (auto i = cs.begin(); i != cs.end(); ++i,++iindex) {
            auto j = i;
            std::advance(j,1);

            for (jindex=iindex+1; j != cs.end(); ++j,++jindex) {
                if (cs.size() >= THRESHOLD)
                    return SatState::UNKNOWN;
                auto result = can_join(*i,*j);
                if (result.first == false) continue;

                auto new_clause = join(*i,*j,result.second);

                if (new_clause.empty())
                    return SatState::UNSAT;

                if (cs.contains(new_clause)) continue;

                canMakeNewClause = true;
                cs.emplace(new_clause);
            }
        }
    }
    while (canMakeNewClause);
    return SatState::SAT;
}
int main(int argc, const char* argv[]) {
    std::ios::sync_with_stdio(false);
    signal(SIGINT, &onCtrlC);
    signal(SIGKILL, &onCtrlC);
    signal(SIGABRT, &onCtrlC);
    signal(SIGTERM, &onCtrlC);
    if (argc != 3) {
        std::cerr<<"Wrong input. Usage ./resolution_naive_first_fit <path_to_cnf_file> <path_to_log_file>";
        return 1;
    }

    clauses = read_clauses(argv[1]);
    std::ofstream g(argv[2]);
    g<<"Start SAT. Result: ";
    g.flush();
    auto start = std::chrono::high_resolution_clock::now();
    auto result = resolution(clauses);
    auto end = std::chrono::high_resolution_clock::now();
    size_t peakSize    = getPeakRSS( );
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
    g<<"Clauze totale: "<<(result==SatState::UNSAT ? clauses.size()+1 : clauses.size())<<'\n';
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    g<<"Timp de execuție: "<<elapsed<<"μs"<<'\n';
    g<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    g.close();
    return 0;
}
