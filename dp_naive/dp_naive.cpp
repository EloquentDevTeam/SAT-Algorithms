/*
 *      DP procedure - naive implementation
 *      Created by xcell (12.04.2025)
 *      Credit: Zhang, Stickel (2000) - Implementing the Davis-Putnam Method,
 *              Journal of Automated Reasoning no. 24
 */

#pragma GCC optimize("O3,fast-math,unroll-loops")
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <chrono>
using ll = long long;
using Literal = long long;
using Clause = std::unordered_set<Literal>;
using ClauseSet = std::unordered_map<size_t, Clause>;
using NodeType = std::pair<Literal, size_t>;

size_t rec_cnt{0};

std::queue<Literal> getUnitClauses(ClauseSet& clauses){
    std::queue<Literal> q;
    for (const auto& cl : clauses) { 
        if (cl.second.size() == 1) q.push(*(cl.second.begin())); 
    }
    return q;
}

struct LiteralFrequencyComparator {
    bool operator()(const NodeType& a, const NodeType& b) {
        return a.second < b.second;
    }
};

Literal getBestLiteral(const ClauseSet& clauses){
    std::unordered_map<Literal, size_t> freqMap;
    for(const auto& cl : clauses){
        for(const auto& lit : cl.second) ++(freqMap[lit]);
    }

    std::priority_queue<NodeType, std::vector<NodeType>, LiteralFrequencyComparator> maxheap;
    for(const auto& entry : freqMap) maxheap.push(entry);

    return maxheap.top().first;
}

ClauseSet unionWithLiteral(ClauseSet& clauses, Literal L){
    ClauseSet cs = clauses;
    Clause c = {L};
    size_t id{0};
    while (clauses.find(id) != clauses.end()) ++id;
    cs.emplace(id, c);
    return cs;
}

bool DPP(ClauseSet clauses){
    ++rec_cnt;

    /* unit propagation */
    std::queue<Literal> units = getUnitClauses(clauses);
    while(!units.empty()){
        auto L{units.front()};
        units.pop();

        /* unit subsumption */
        for (auto it = clauses.begin(); it != clauses.end(); ) {
            if (it->second.find(L) != it->second.end()) it = clauses.erase(it);
            else ++it;
        }        

        L *= (-1);

        /* unit resolution */
        for(auto& cl : clauses){
            if(cl.second.find(L) != cl.second.end()) {
                cl.second.erase(L);
                if(cl.second.empty()) return false;
                if(cl.second.size() == 1) units.push(*(cl.second.begin()));
            }
        }

        if(clauses.empty()) return true;
    }

    /* attempting resolution */
    Literal l{getBestLiteral(clauses)};

    if(DPP(unionWithLiteral(clauses, l))) return true; 
    if(DPP(unionWithLiteral(clauses, l*(-1)))) return true;
    return false;
}

int main(int argc, char** argv){

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    if(argc <= 1){
        std::cout << "Usage: ./dp_naive [test_file]\n";
        exit(EXIT_FAILURE);
    }

    std::ifstream fin(argv[1]);
    fin.tie(nullptr);

    size_t llNoVariables{}, llNoClauses{};
    Literal x{};

    fin >> llNoVariables >> llNoClauses;

    ClauseSet clauses(llNoClauses);
    for(size_t i = 0; i < llNoClauses; ++i){
        Clause c;
        while(fin >> x && x != 0) c.emplace(x);
        clauses[i] = c;
    }

    fin.close();

    auto start = std::chrono::high_resolution_clock::now();
    auto result = DPP(clauses);
    auto end = std::chrono::high_resolution_clock::now();

    if(result) std::cout << "Result: SAT\n";
    else std::cout << "Result: UNSAT\n";

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    std::cout << "Recursion count: " << rec_cnt << "\n";
    std::cout << "Elapsed time: " << elapsed << " microseconds\n";

}