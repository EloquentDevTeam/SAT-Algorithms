#include "HeuristicEntryDP.h"
#include <iostream>

HeuristicEntryDP::HeuristicEntryDP(Literal l, size_t occurences)
    : literal(l), occurrences(occurences) {
}

HeuristicEntryDP::HeuristicEntryDP() {
    literal = 0;
    occurrences = 0;
}

HeuristicEntryDP HeuristicEntryDP::operator++(int) {
    HeuristicEntryDP e(*this);
    this->occurrences++;
    return e;
}

HeuristicEntryDP &HeuristicEntryDP::operator++() {
    this->occurrences++;
    return *this;
}

Literal HeuristicEntryDP::get_literal() const {
    return literal;
}

size_t HeuristicEntryDP::get_occurrences() const {
    return occurrences;
}

void HeuristicEntryDP::set_literal(const Literal l) {
    this->literal = l;
}

HeuristicEntryDP &HeuristicEntryDP::operator--() {
    if (occurrences != 0)
        this->occurrences--;
    else [[unlikely]]
        std::cerr << "Decrementare ilegală pentru " << this->literal << '\n';
    return *this;
}

HeuristicEntryDP HeuristicEntryDP::operator--(int) {
    const HeuristicEntryDP e = *this;
    if (occurrences != 0) {
        this->occurrences--;
    }
    else [[unlikely]]
        std::cerr << "Decrementare ilegală pentru " << this->literal << '\n';

    return e;
}