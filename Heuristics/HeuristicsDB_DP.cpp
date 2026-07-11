#include "HeuristicsDB_DP.h"
#include <cmath>

HeuristicsDB_DP::HeuristicsDB_DP(const size_t size) : size(size) {
    this->vector.resize(2 * size + 2);
}

HeuristicEntryDP& HeuristicsDB_DP::operator[](Literal lit) {
    if (lit < 0)
        return this->vector[2 * std::abs(lit) + 1];
    return this->vector[2 * lit];
}

std::vector<HeuristicEntryDP>::iterator HeuristicsDB_DP::begin() {
    return vector.begin();
}

std::vector<HeuristicEntryDP>::iterator HeuristicsDB_DP::end() {
    return vector.end();
}

void HeuristicsDB_DP::resize(const size_t new_size) {
    this->vector.resize(2 * new_size + 2);
    this->size = new_size;
}