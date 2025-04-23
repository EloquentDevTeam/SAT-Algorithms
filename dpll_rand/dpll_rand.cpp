/**
 * Pentru acest program vom folosi strategia de alegere 'la întâmplare', numită și RAND
*/
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
#include <queue>
#include <map>
#include <filesystem>
#include <random>
#include <stack>


namespace fs = std::filesystem;

#pragma GCC optimize("O3,fast-math,unroll-loops")

using Literal = int64_t;
using Clause = std::set<Literal>;
using ClauseSet = std::set<Clause>;


std::size_t max_clauses = 0;
class HeuristicEntry {
    Literal literal;
    size_t occurrences;
public:
    explicit HeuristicEntry(Literal l, size_t occurences = 0)
        : literal(l),
          occurrences(occurences) {
    }
    HeuristicEntry() {
        literal = 0;
        occurrences = 0;
    }

    friend auto operator<=>(const HeuristicEntry &lhs, size_t rhs) {
        return lhs.occurrences <=> rhs;
    }
    HeuristicEntry operator++(int) {
        HeuristicEntry e( *this);
        this->occurrences++;
        return e;
    }
    HeuristicEntry &operator++() {
        this->occurrences++;
        return *this;
    }

    [[nodiscard]] Literal get_literal() const {
        return literal;
    }

    [[nodiscard]] size_t get_occurrences() const {
        return occurrences;
    }
    void set_literal(const Literal l) {
        this->literal = l;
    }

    HeuristicEntry &operator--() {
        if (occurrences != 0)
            this->occurrences--;
        else
            std::cerr<<"Decrementare ilegală pentru "<<this->literal<<'\n';
        return *this;
    }
    HeuristicEntry operator--(int) {
        const HeuristicEntry e = *this;
        if (occurrences != 0) {
            this->occurrences--;
        }
        else
            std::cerr<<"Decrementare ilegală pentru "<<this->literal<<'\n';

        return e;
    }

    friend bool operator==(const HeuristicEntry& lhs, size_t i) {
        return lhs.occurrences == i;
    }
};

class HeuristicsDB {
    std::vector<HeuristicEntry> vector;
    std::vector<Literal> existing_literals;
    size_t size = 0;

public:
    explicit HeuristicsDB(const size_t size): size(size) {
        this->vector.resize(2*size+2);
        this->existing_literals.resize(0);
    }
    explicit HeuristicsDB() = default;
    HeuristicEntry& operator[](Literal lit) {
        if (lit < 0)
            return this->vector[2*std::abs(lit)+1];
        return this->vector[2*lit];
    }
    auto begin() {
        return vector.begin();
    }
    auto end() {
        return vector.end();
    }
    void resize(const size_t new_size) {
        this->vector.resize(2*new_size+2);
        this->size = new_size;
    }
    void poppulate_literal_record() {
        existing_literals.clear();
        for (auto i = 2; i< this->vector.size(); ++i)
            if (vector[i].get_occurrences() > 0)
                this->existing_literals.emplace_back(vector[i].get_literal());
    }
    [[nodiscard]] std::vector<Literal>& get_record() {
        return existing_literals;
    }
};
class ProblemContext {
    HeuristicsDB db;
    ClauseSet cs;

public:
    [[nodiscard]] ProblemContext(const HeuristicsDB &db, const ClauseSet &cs)
        : db(db),
          cs(cs) {
    }
    [[nodiscard]] HeuristicsDB & get_db() {
        return db;
    }
    [[nodiscard]] ClauseSet & get_cs() {
        return cs;
    }
};
constexpr std::size_t THRESHOLD = 71000000;

void onCtrlC(int sig) {
    std::cerr<<"Programul a primit semnalul "<<sig<<"\n Rezultat: UNKNOWN.\n";
    size_t peakSize = getPeakRSS();
    std::cerr<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    std::cerr<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    exit(1);
}

void analyse(const Clause& c, HeuristicsDB& db) {
    for (auto& literal: c) {
        ++db[literal];
        db[literal].set_literal(literal);
        db[-literal].set_literal(-literal);
    }
}
[[nodiscard]] ClauseSet read_clauses(const char *file, HeuristicsDB& db) {
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
            is>>lit_total_count>>clause_count;
            db.resize(lit_total_count);
            continue;
        }

        std::istringstream is(line);
        Clause c;
        int64_t lit = 0;
        while (is>>lit) {
            if (lit == 0) break;
            c.emplace(lit);
        }
        analyse(c,db);
        clauses.emplace(c);
    }
    db.poppulate_literal_record();
    f.close();
    return clauses;
}

enum class SatState {
    SAT,
    UNSAT,
    UNKNOWN
};


void process_clause_removal(HeuristicsDB &db, const std::set<long>& clause) {
    for (auto &c_lit: clause)
        --db[c_lit];
}

bool one_literal_clause_rule(ClauseSet &cs, std::set<Literal>& single_literals, SatState &result, HeuristicsDB& db) {
    while (!single_literals.empty()) {
        for (const auto& lit: single_literals) {
            auto it = cs.begin();
            while (it != cs.end()) {
                auto clause = *it;
                auto next = it;
                std::advance(next,1);
                if (clause.contains(lit)) {
                    cs.erase(clause);
                    if (cs.empty()) {
                        result = SatState::SAT;
                        return true;
                    }
                    std::cerr<<"Șterg clauza prin literal pur\n";
                    process_clause_removal(db, clause);
                    it = next;
                    continue;
                }
                if (clause.contains(-lit)) {
                    cs.erase(clause);
                    std::cerr<<"Șterg complementarul prin literal pur\n";
                    clause.erase(-lit);
                    if (clause.empty()) {
                        result = SatState::UNSAT;
                        return true;
                    }
                    --db[-lit];
                    cs.emplace(clause);
                }
                ++it;
            }
        }
        single_literals.clear();
        for (const auto& clause: cs) {
            if (clause.size() == 1) {
                single_literals.emplace(*clause.begin());
            }
        }
    }
    return false;
}

bool single_polarity_rule(ClauseSet &cs, HeuristicsDB &db, std::set<Literal>& single_polarity_literals, SatState &result) {
    while (!single_polarity_literals.empty()) {
        for (const auto& literal: single_polarity_literals) {
            for (auto it = cs.begin(); it != cs.end(); ++it) {
                const auto clause = *it;
                auto next = it;
                std::advance(next,1);
                if (clause.contains(literal)) {
                    cs.erase(clause);
                    std::cerr<<"Șterg clauza prin literal cu aceiași polaritate\n";
                    process_clause_removal(db,clause);
                    if (cs.empty()) {
                        result = SatState::SAT;
                        return true;
                    }
                    it = next;
                    std::advance(it,-1);
                }
            }
        }
        single_polarity_literals.clear();
        for (const auto& clause: cs) {
            for (const auto& literal: clause) {
                if (db[-literal] == 0) {
                    single_polarity_literals.emplace(literal);
                }
            }
        }
    }
    return false;
}

bool davis_putnam(ProblemContext& pc, SatState& result) {

    std::set<Literal> single_polarity_literals;
    std::set<Literal> single_literals;
    for (const auto& clause: pc.get_cs()) {
        if (clause.size() == 1) {
            single_literals.emplace(*clause.begin());
        }
        for (const auto& literal: clause) {
            if (pc.get_db()[-literal] == 0) {
                single_polarity_literals.emplace(literal);
            }
        }
    }

    if (one_literal_clause_rule(pc.get_cs(), single_literals, result, pc.get_db())) return true;
    if (single_polarity_rule(pc.get_cs(), pc.get_db(), single_polarity_literals, result)) return true;
    return false;
}


Literal pick_literal(ProblemContext & ctx) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distrib(0, ctx.get_db().get_record().size()-1);

    return ctx.get_db().get_record()[distrib(gen)];
}

SatState davis_putnam_logemann_loveland(const HeuristicsDB &db, const ClauseSet &cs) {
    SatState result = SatState::UNKNOWN;
    std::stack<ProblemContext> context_stack;
    context_stack.emplace(db,cs);

    while (!context_stack.empty()) {
        auto c_context = context_stack.top();
        context_stack.pop();
        auto c_solved = davis_putnam(c_context,result);
        c_context.get_db().poppulate_literal_record();
        if (c_solved && result == SatState::SAT) {
            return  SatState::SAT;
        }
        if (!c_solved){
            Literal l = pick_literal(c_context);
            Clause literal({l});
            Clause notLiteral({-l});
            ProblemContext literal_ctx = c_context;
            ProblemContext negative_literal_ctx = c_context;
            literal_ctx.get_cs().emplace(literal);
            ++literal_ctx.get_db()[l];
            literal_ctx.get_db().poppulate_literal_record();

            negative_literal_ctx.get_cs().emplace(notLiteral);
            ++negative_literal_ctx.get_db()[-l];
            negative_literal_ctx.get_db().poppulate_literal_record();

            context_stack.push(negative_literal_ctx);
            context_stack.push(literal_ctx);
        }

    }
    return SatState::UNSAT;
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
    if (!fs::exists(argv[1])) {
        std::cerr<<"File "<<argv[1]<<" does not exist\n";
        return 1;
    }

    HeuristicsDB db;

    ClauseSet clauses = read_clauses(argv[1],db);
    max_clauses = std::max(max_clauses,clauses.size());
    std::ofstream g(argv[2]);
    g<<"S-a citit "<<argv[1]<<'\n';
    g<<"Start SAT. Resultat: ";
    g.flush();
    auto start = std::chrono::high_resolution_clock::now();
    auto result = davis_putnam_logemann_loveland(db, clauses);
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
    g<<"Număr maxim de clauze "<<(result==SatState::UNSAT ? max_clauses+1 : max_clauses)<<'\n';
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    g<<"Timp de execuție: "<<elapsed<<"μs"<<'\n';
    g<<"Memorie consumată: "<< peakSize<<"B."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024<<"KB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024<<"MB."<<'\n';
    g<<"Memorie consumată: "<< peakSize/1024/1024/1024<<"GB."<<'\n';
    g.close();
    return 0;
}
