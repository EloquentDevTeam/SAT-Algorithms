/*
 *      DPLL procedure - naive implementation
 *      Created by xcell (12.04.2025)
 *      Credit: Zhang, Stickel (2000) - Implementing the Davis-Putnam Method,
 *              Journal of Automated Reasoning no. 24
 */

#pragma GCC optimize("O3,fast-math,unroll-loops")
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <filesystem>

#include "memutils.h"

namespace fs = std::filesystem;

using ll = long long;
using Literal = long long;
using Clause = std::unordered_set<Literal>;
using ClauseSet = std::unordered_map<size_t, Clause>;

void incrementFreq(std::unordered_map<Literal, size_t> &freqMap, const Clause &clause) {
    for (Literal lit : clause)
        ++freqMap[lit];
}

void decrementFreq(std::unordered_map<Literal, size_t> &freqMap, const Clause &clause) {
    for (Literal lit : clause) {
        if (--freqMap[lit] == 0)
            freqMap.erase(lit);
    }
}

std::queue<Literal> getUnitClauses(ClauseSet &clauses) {
    std::queue<Literal> q;
    for (const auto &cl : clauses) {
        if (cl.second.size() == 1)
        q.push(*(cl.second.begin()));
    }
    return q;
}

Literal getBestLiteral(const std::unordered_map<Literal, size_t> &freqMap) {
    Literal bestLiteral = 0;
    size_t maxFreq = 0;
    for (const auto &[lit, freq] : freqMap) {
        if (freq > maxFreq) {
            maxFreq = freq;
            bestLiteral = lit;
        }
    }
    return bestLiteral;
}

ClauseSet unionWithLiteral(ClauseSet &clauses, Literal L) {
    ClauseSet cs = clauses;
    Clause c = {L};
    size_t id{0};
    while (clauses.find(id) != clauses.end())
        ++id;
    cs.emplace(id, c);
    return cs;
}

bool DPLL(ClauseSet &clauses, std::unordered_map<Literal, size_t> &freqMap){
    /* unit propagation */
    std::queue<Literal> units = getUnitClauses(clauses);
    while (!units.empty()) {
        auto L{units.front()};
        units.pop();

        /* unit subsumption */
        for (auto it = clauses.begin(); it != clauses.end();) {
            if (it->second.find(L) != it->second.end()) {
                decrementFreq(freqMap, it->second);
                it = clauses.erase(it);
            }else{
                ++it;
            }
        }

        L *= (-1);

        /* unit resolution */
        for (auto &cl : clauses) {
            if (cl.second.find(L) != cl.second.end()) {
                cl.second.erase(L);
                --freqMap[L];
                if(freqMap[L] == 0) freqMap.erase(L);
                if (cl.second.empty())
                return false;
                if (cl.second.size() == 1)
                units.push(*(cl.second.begin()));
            }
        }

        if (clauses.empty())
        return true;
    }

    /* attempting resolution */
    Literal l{getBestLiteral(freqMap)};

    auto clausesCopy = clauses;
    auto freqCopy = freqMap;
    clausesCopy = unionWithLiteral(clausesCopy, l);
    incrementFreq(freqCopy, Clause{l});
    if (DPLL(clausesCopy, freqCopy)) return true;

    clausesCopy = clauses;
    freqCopy = freqMap;
    clausesCopy = unionWithLiteral(clausesCopy, -l);
    incrementFreq(freqCopy, Clause{-l});
    if (DPLL(clausesCopy, freqCopy)) return true;
    return false;
}

int main(int argc, char **argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (argc != 3){
        std::cerr << "Wrong input. Usage ./dpll_mcl <path_to_cnf_file> <path_to_log_file>\n";
        exit(EXIT_FAILURE);
    }

    if (!fs::exists(argv[1])){
        std::cerr << "File not found: " << argv[1] << "\n";
        exit(EXIT_FAILURE);
    }

    std::ifstream fin(argv[1]);
    fin.tie(nullptr);

    std::string line;
    size_t llNoVariables{}, llNoClauses{};
    Literal x{};

    while(std::getline(fin, line)){
        if(line.empty() || line.find_first_not_of(" \t\n\v\f\r") == std::string::npos) continue;

        std::stringstream ss(line);
        char firstchar{};
        ss >> firstchar;

        if(firstchar == 'c') continue;
        if(firstchar == 'p'){
            std::string format;
            ss >> format >> llNoVariables >> llNoClauses;
            break;
        }
    }

    ClauseSet clauses(llNoClauses);
    for (size_t i = 0; i < llNoClauses; ++i) {
        Clause c;
        while (fin >> x && x != 0)
        c.emplace(x);
        clauses[i] = c;
    }
    
    fin.close();
    
    std::unordered_map<Literal, size_t> freqMap;
    for(const auto &[id, clause] : clauses){
        incrementFreq(freqMap, clause);
    }

    std::ofstream g(argv[2]);
    g << "S-a citit " << argv[1] << "\n";
    g << "Start SAT. Rezultat: ";

    auto start = std::chrono::high_resolution_clock::now();
    auto result = DPLL(clauses, freqMap);
    auto end = std::chrono::high_resolution_clock::now();

    size_t peakSize = getPeakRSS();

    if (result) g << "SAT\n";
    else g << "UNSAT\n";

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    g<<"Timp de execuție: "<<elapsed<<"μs"<<'\n';
    g<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    g.close();
}
