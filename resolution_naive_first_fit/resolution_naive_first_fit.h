#ifndef SAT_ALGORITHMS_RESOLUTION_NAIVE_FIRST_FIT_CPP_H
#define SAT_ALGORITHMS_RESOLUTION_NAIVE_FIRST_FIT_CPP_H

#include "common.h"

SatState res(ClauseSet);
std::pair<bool,int64_t> can_join(const Clause&,const Clause&);
Clause join(const Clause&,const Clause&,const Literal);

#endif //SAT_ALGORITHMS_RESOLUTION_NAIVE_FIRST_FIT_CPP_H