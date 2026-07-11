add_executable(dp_naive dp_naive.cpp
        dp_naive.h
        ../Heuristics/HeuristicEntryDP.cpp
        ../Heuristics/HeuristicEntryDP.h
        ../Heuristics/HeuristicsDB_DP.cpp
        ../Heuristics/HeuristicsDB_DP.h)
target_link_libraries(dp_naive PRIVATE memutils)