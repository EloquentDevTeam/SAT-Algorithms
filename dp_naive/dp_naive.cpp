#pragma GCC optimize("O3,fast-math,unroll-loops")
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <memutils.h>
#include <csignal>
#include <queue>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

using Literal = int64_t;
using Clause = std::set<Literal>;
using ClauseSet = std::set<Clause>;

constexpr std::size_t THRESHOLD = 71000000;
constexpr auto TIMEOUT = std::chrono::seconds(60);

std::atomic<bool> should_stop = false;
ClauseSet clauses;
std::size_t max_clauses{0};

void onCtrlC(int sig) {
    std::cerr << "Programul a primit semnalul " << sig << "\n Rezultat: UNKNOWN.\n Nr. de clauze totale: " <<clauses.size()<< '\n';
    size_t peakSize{getPeakRSS()};
    std::cerr << "Memorie consumată: " << peakSize << " B.\n";
    std::cerr << "Memorie consumată: " << peakSize/1024 << " KB.\n";
    std::cerr << "Memorie consumată: " << peakSize/1024/1024 << " MB.\n";
    std::cerr << "Memorie consumată: " << peakSize/1024/1024/1024 << " GB.\n";
    exit(1);
}

enum class SatState {
    SAT,
    UNSAT,
    UNKNOWN
};

class HeuristicEntry {
    Literal literal;
    size_t occurrences;
public:
    explicit HeuristicEntry(Literal l, size_t occurences = 0)
        : literal(l),
          occurrences(occurences) {}
    HeuristicEntry() : literal{0}, occurrences{0}{}

    friend auto operator<=>(const HeuristicEntry &lhs, size_t rhs) {
        return lhs.occurrences <=> rhs;
    }

    HeuristicEntry operator++(int) {
        HeuristicEntry e(*this);
        this->occurrences++;
        return e;
    }

    HeuristicEntry &operator++() {
        this->occurrences++;
        return *this;
    }

    [[nodiscard]] Literal get_literal() const { return literal; }
    [[nodiscard]] size_t get_occurrences() const { return occurrences;}

    void set_literal(const Literal l) { this->literal = l;}

    HeuristicEntry &operator--() {
        if (occurrences != 0) this->occurrences--;
        else [[unlikely]]std::cerr << "Decrementare ilegală pentru " << this->literal << '\n';
        return *this;
    }

    HeuristicEntry operator--(int) {
        const HeuristicEntry e = *this;
        if (occurrences != 0) this->occurrences--;
        else [[unlikely]]std::cerr<<"Decrementare ilegală pentru "<<this->literal<<'\n';
        return e;
    }

    friend bool operator==(const HeuristicEntry& lhs, size_t i) {
        return lhs.occurrences == i;
    }
};

class HeuristicsDB {
    std::vector<HeuristicEntry> vector;
    size_t size{0};

public:
    explicit HeuristicsDB(const size_t size): size(size) {
        this->vector.resize(2*size+2);
    }
    explicit HeuristicsDB() = default;

    HeuristicEntry& operator[](Literal lit) {
        if (lit < 0) return this->vector[2*std::abs(lit)+1];
        return this->vector[2*lit];
    }

    auto begin() { return vector.begin();}
    auto end() { return vector.end();}

    void resize(const size_t new_size) {
        this->vector.resize(2*new_size+2);
        this->size = new_size;
    }
};

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
    size_t clause_count{0}, lit_total_count{0};
    ClauseSet read_clauses;

    while (!f.eof()) {
        std::string line;
        std::getline(f,line);
        
        if (line.empty()) break;
        if (line == "%" || line == "0") continue;

        while (line.starts_with('c')) std::getline(f,line);

        if (line.starts_with("p cnf ")) {
            line = line.substr(strlen("p cnf "));
            std::istringstream is(line);
            is >> lit_total_count >> clause_count;
            db.resize(lit_total_count);
            continue;
        }

        std::istringstream is(line);
        Clause c;
        int64_t lit{0};
        while (is >> lit) {
            if (lit == 0) break;
            c.emplace(lit);
        }
        analyse(c, db);
        read_clauses.emplace(c);
    }

    f.close();
    return read_clauses;
}

std::pair<bool,int64_t> can_join(const Clause& c1, const Clause& c2) {
    size_t pairs{0};
    int64_t lit{0};
    for (auto& literal: c1) {
        if (c2.contains(literal*-1)) {
            ++pairs;
            lit = literal;
        }
    }
    return {pairs == 1,lit};
}

void process_clause_removal(HeuristicsDB &db, const std::set<long>& clause) {
    for (auto &c_lit: clause) --db[c_lit];
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
                    //std::cerr<<"Șterg clauza prin literal pur\n";
                    process_clause_removal(db, clause);
                    it = next;
                    continue;
                }

                if (clause.contains(-lit)) {
                    cs.erase(clause);
                    //std::cerr<<"Șterg complementarul prin literal pur\n";
                    clause.erase(-lit);
                    if (clause.empty()) {
                        result = SatState::UNSAT;
                        return true;
                    }
                    --db[-lit];
                    cs.emplace(clause);
                }

                it=next;
            }
        }
        single_literals.clear();
        for (const auto& clause: cs) {
            if (clause.size() == 1) single_literals.emplace(*clause.begin());
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
                    //std::cerr<<"Șterg clauza prin literal cu aceiași polaritate\n";
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
                if (db[-literal] == 0) single_polarity_literals.emplace(literal);
            }
        }
    }

    return false;
}

Clause join(const Clause& c1, const Clause& c2, const Literal l) {
    Clause c12;

    for (auto& lit : c1) c12.emplace(lit);
    for (auto& lit : c2) c12.emplace(lit);

    c12.erase(l);
    c12.erase(-l);

    return c12;
}

bool resolution(ClauseSet& cs, HeuristicsDB& hdb, std::set<Literal>& single_polarity_literals, std::set<Literal>& single_literals, SatState& sat_result) {
    bool canMakeNewClause{false};
    size_t iindex{0}, jindex{0};

    do {
        canMakeNewClause = false;
        iindex = 0;
        jindex = 0;

        for (auto i = cs.begin(); i != cs.end(); ++i,++iindex) {
            auto j = i;
            std::advance(j,1);

            for (jindex=iindex+1; j != cs.end(); ++j,++jindex) {
                if (should_stop.load() || cs.size() >= THRESHOLD) {
                    sat_result = SatState::UNKNOWN;
                    return true;
                }

                auto result = can_join(*i,*j);
                if (result.first == false) continue;

                auto new_clause = join(*i,*j,result.second);
                if (new_clause.empty()) {
                    sat_result = SatState::UNSAT;
                    return true;
                }

                if (cs.contains(new_clause)) continue;
                for (const auto& lit: new_clause) ++hdb[lit];

                bool should_repeat_dp{false};

                if (hdb[result.second] > 0 && hdb[-result.second] == 0) {
                    single_polarity_literals.emplace(result.second);
                    should_repeat_dp = true;
                }
                else if (hdb[-result.second] > 0 && hdb[result.second] == 0) {
                    single_polarity_literals.emplace(-result.second);
                    should_repeat_dp = true;
                }

                if (new_clause.size() == 1) {
                    single_literals.emplace(*new_clause.begin());
                    should_repeat_dp = true;
                }
                canMakeNewClause = true;
                cs.emplace(new_clause);
                //reset_flag = true;

                max_clauses = std::max(max_clauses,clauses.size());

                //nu am rezolvat, dar am găsit ceva "interesant"
                if (should_repeat_dp) return false;
            }
        }
    } while (canMakeNewClause && !should_stop.load());

    sat_result = SatState::SAT;
    return true;
}

SatState davis_putnam(ClauseSet& cs, HeuristicsDB& db) {
    SatState result{SatState::UNKNOWN};
    std::set<Literal> single_polarity_literals;
    std::set<Literal> single_literals;

    for (const auto& clause: cs) {
        if (clause.size() == 1) single_literals.emplace(*clause.begin());
        for (const auto& literal: clause) {
            if (db[-literal] == 0) single_polarity_literals.emplace(literal);
        }
    }

    while (!should_stop.load()) {
        if (one_literal_clause_rule(cs, single_literals, result, db)) return result;
        if (single_polarity_rule(cs, db, single_polarity_literals, result)) return result;
        if (resolution(cs,db,single_polarity_literals,single_literals,result)) return result;
    }

    return SatState::UNKNOWN;
}

int main(int argc, const char* argv[]) {
    std::ios::sync_with_stdio(false);
    signal(SIGINT, &onCtrlC);
    signal(SIGKILL, &onCtrlC);
    signal(SIGABRT, &onCtrlC);
    signal(SIGTERM, &onCtrlC);

    if (argc != 3) {
        std::cerr << "Wrong input. Usage ./resolution_naive_first_fit <path_to_cnf_file> <path_to_log_file>";
        return 1;
    }

    if (!fs::exists(argv[1])) {
        std::cerr << "File " << argv[1] << " does not exist.\n";
        return 1;
    }

    HeuristicsDB db;
    clauses = read_clauses(argv[1],db);
    max_clauses = std::max(max_clauses,clauses.size());

    std::ofstream g(argv[2]);
    g << "S-a citit " << argv[1] << '\n';
    g << "Start SAT. Rezultat: ";
    g.flush();

    SatState sat_state{SatState::UNKNOWN};
    std::chrono::_V2::system_clock::time_point end{};

    auto start = std::chrono::high_resolution_clock::now();
    std::future<SatState> result = std::async(std::launch::async, davis_putnam, std::ref(clauses), std::ref(db));
    if(result.wait_for(TIMEOUT) == std::future_status::ready){
        end = std::chrono::high_resolution_clock::now();
        sat_state = result.get();
    }else{
        end = std::chrono::high_resolution_clock::now();
        should_stop = true;
    }
    
    size_t peakSize{getPeakRSS()};
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

    switch (sat_state) {
        case SatState::SAT:
        g << "SAT";
        break;
        case SatState::UNSAT:
        g << "UNSAT";
        break;
        case SatState::UNKNOWN:
        g << "UNKNOWN";
        if(should_stop.load()) g << " (timeout)";
        break;
    }
    g << '\n';
    

    g << "Număr maxim de clauze: " << (sat_state == SatState::UNSAT ? max_clauses+1 : max_clauses) << '\n';
    g << "Timp de execuție: " << elapsed << "μs\n";
    g << "Memorie consumată: " << peakSize << " B.\n";
    g << "Memorie consumată: " << peakSize/1024 << " KB.\n";
    g << "Memorie consumată: " << peakSize/1024/1024 << " MB.\n";
    g << "Memorie consumată: " << peakSize/1024/1024/1024 << " GB.\n";
    
    g.close();
}
