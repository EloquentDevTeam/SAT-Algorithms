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
#include <filesystem>


using Literal = int64_t;
using Clause = std::set<Literal>;
using ClauseSet = std::set<Clause>;
namespace fs = std::filesystem;

ClauseSet clauses;
std::atomic<bool> should_stop = false;

constexpr std::size_t THRESHOLD = 71000000;
constexpr auto TIMEOUT = std::chrono::seconds(60);


void onCtrlC(int sig) {
    std::cerr << "Programul a primit semnalul " << sig << "\n Rezultat: UNKNOWN.\n Nr. de clauze totale: " << clauses.size()<< '\n';

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

[[nodiscard]] ClauseSet read_clauses(const char *file) {
    std::ifstream f(file);
    f.tie(nullptr);

    size_t clause_count{0}, lit_total_count{0};
    ClauseSet read_clauses;

    while (!f.eof()) {
        std::string line;
        std::getline(f, line);

        if (line.empty())               break;
        if (line == "%" || line == "0") continue;

        while (line.starts_with('c')) std::getline(f, line);
        
        if (line.starts_with("p cnf ")) {
            line = line.substr(strlen("p cnf "));
            std::istringstream is(line);
            is >> clause_count >> lit_total_count;
            continue;
        }

        std::istringstream is(line);
        Clause c;
        int64_t lit{0};

        while (is >> lit) {
            if (lit == 0) break;
            c.emplace(lit);
        }
        read_clauses.emplace(c);
    }

    f.close();
    return read_clauses;
}

std::pair<bool, int64_t> can_join(const Clause& c1, const Clause& c2) {
    size_t pairs{0};
    int64_t lit{0};

    for (auto& literal: c1) {
        if (c2.contains(literal*-1)) {
            ++pairs;
            lit = literal;
        }
    }

    return {pairs == 1, lit};
}

Clause join(const Clause& c1, const Clause& c2, const Literal l) {
    Clause c12;

    for (auto& lit : c1) c12.emplace(lit);
    for (auto& lit : c2) c12.emplace(lit);
    
    c12.erase(l);
    c12.erase(-l);

    return c12;
}

[[nodiscard]] SatState resolution(ClauseSet& cs) {
    bool canMakeNewClause{false};
    size_t iindex{0}, jindex{0};

    do {
        canMakeNewClause = false;
        iindex = 0;
        jindex=0;
        
        for (auto i = cs.begin(); i != cs.end(); ++i,++iindex) {
            auto j = i;
            std::advance(j, 1);

            for (jindex = iindex + 1; j != cs.end(); ++j,++jindex) {
                if(should_stop.load() || cs.size() >= THRESHOLD) return SatState::UNKNOWN;

                auto result = can_join(*i,*j);
                if (result.first == false) continue;

                auto new_clause = join(*i,*j,result.second);
                if (new_clause.empty()) return SatState::UNSAT;
                if (cs.contains(new_clause)) continue;

                canMakeNewClause = true;
                cs.emplace(new_clause);
            }
        }
    } while (canMakeNewClause && !should_stop.load());

    return SatState::SAT;
}

int main(int argc, const char* argv[]) {
    std::ios::sync_with_stdio(false);

    signal(SIGINT, &onCtrlC);
    signal(SIGKILL, &onCtrlC);
    signal(SIGTERM, &onCtrlC);
    signal(SIGABRT, &onCtrlC);

    if (argc != 3) {
        std::cerr << "Wrong input. Usage ./resolution_naive_first_fit <path_to_cnf_file> <path_to_log_file>";
        return 1;
    }

    if (!fs::exists(argv[1])) {
        std::cerr << "File " << argv[1] << " does not exist.\n";
        return 1;
    }

    clauses = read_clauses(argv[1]);

    std::ofstream g(argv[2]);
    g << "S-a citit " << argv[1] << '\n';
    g << "Start SAT. Rezultat: ";
    g.flush();

    SatState sat_state{SatState::UNKNOWN};
    std::chrono::_V2::system_clock::time_point end{};

    auto start = std::chrono::high_resolution_clock::now();
    std::future<SatState> result = std::async(std::launch::async, resolution, std::ref(clauses));
    if(result.wait_for(TIMEOUT) == std::future_status::ready) {
        end = std::chrono::high_resolution_clock::now();
        sat_state = result.get();
    } else {
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
    

    g << "Total clauze: " << (sat_state == SatState::UNSAT ? clauses.size()+1 : clauses.size()) << '\n';
    g << "Timp de execuție: " << elapsed << "μs\n";
    g << "Memorie consumată: " << peakSize << " B.\n";
    g << "Memorie consumată: " << peakSize/1024 << " KB.\n";
    g << "Memorie consumată: " << peakSize/1024/1024 << " MB.\n";
    g << "Memorie consumată: " << peakSize/1024/1024/1024 << " GB.\n";

    g.close();
}
