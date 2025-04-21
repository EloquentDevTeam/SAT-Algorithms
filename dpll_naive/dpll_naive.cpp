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
#include <stack>

using ll = long long;
using Literal = long long;
using Clause = std::unordered_set<Literal>;
using ClauseSet = std::unordered_map<size_t, Clause>;
using NodeType = std::pair<Literal, size_t>;

size_t rec_cnt{0};

struct ContextVariables {
    std::queue<Literal> units;
    ClauseSet clauses;
};

using Context = std::stack<ContextVariables>;


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
    while (clauses.contains(id)) ++id;
    cs.emplace(id, c);
    return cs;
}

bool DPP(ContextVariables& initial_context_variables){

    Context ctx_stack;
    ctx_stack.emplace(initial_context_variables);
    ++rec_cnt;
    bool result = false;
    while (!ctx_stack.empty()) {
        auto ctx = ctx_stack.top();
        ctx_stack.pop();
        ctx.units = getUnitClauses(ctx.clauses);
        while(!ctx.units.empty()) {
            bool should_start_over = false;
            auto L{ctx.units.front()};
            ctx.units.pop();

            /* unit subsumption */
            for (auto it = ctx.clauses.begin(); it != ctx.clauses.end(); ) {
                if (it->second.contains(L)) it = ctx.clauses.erase(it);
                else ++it;
            }

            L *= (-1);

            /* unit resolution */
            for(auto& cl : ctx.clauses){
                if(cl.second.contains(L)) {
                    cl.second.erase(L);
                    if(cl.second.empty()) {
                        return false;
                    }
                    if(cl.second.size() == 1) ctx.units.push(*(cl.second.begin()));
                }
            }
            if(ctx.clauses.empty()) {
               result = true;
            }
        }

        /* attempting resolution */
        Literal l{getBestLiteral(ctx.clauses)};

        ctx_stack.push({.clauses = unionWithLiteral(ctx.clauses,l*(-1))});
        ctx_stack.push({.clauses = unionWithLiteral(ctx.clauses,l)});
    }
    return result;
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
    ContextVariables cv;
    cv.clauses = clauses;
    auto result = DPP(cv);
    auto end = std::chrono::high_resolution_clock::now();

    if(result) std::cout << "Result: SAT\n";
    else std::cout << "Result: UNSAT\n";

    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    std::cout << "Recursion count: " << rec_cnt << "\n";
    std::cout << "Elapsed time: " << elapsed << " microseconds\n";

}