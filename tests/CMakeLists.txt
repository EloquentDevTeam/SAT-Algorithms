include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(TestDP
        dp_naive.cpp
        ../dp_naive/dp_naive.cpp
        ../Heuristics/HeuristicEntryDP.cpp
        ../Heuristics/HeuristicsDB_DP.cpp
)

add_executable(TestDPLL_RAND
        dpll_rand.cpp
        ../dpll_rand/dpll_rand.cpp
)

add_executable(TestDPLL_MCL
        dpll_mcl.cpp
        ../dpll_mcl/dpll_mcl.cpp
        ../dpll_mcl/dpll_utils.cpp
)

add_executable(TestResolution
        resolution_naive_first_fit.cpp
        ../resolution_naive_first_fit/resolution_naive_first_fit.cpp
)

target_include_directories(TestDP PRIVATE
        ../utilities
        ../dp_naive
        ../Heuristics
)

target_include_directories(TestDPLL_RAND PRIVATE
        ../utilities
        ../dpll_rand
)

target_include_directories(TestDPLL_MCL PRIVATE
        ../utilities
        ../dpll_mcl
)

target_include_directories(TestResolution PRIVATE
        ../utilities
        ../resolution_naive_first_fit
)

target_link_libraries(TestResolution PRIVATE
        GTest::gtest
        GTest::gtest_main
        memutils
)

target_link_libraries(TestDP PRIVATE
        GTest::gtest
        GTest::gtest_main
        memutils
)

target_link_libraries(TestDPLL_RAND PRIVATE
        GTest::gtest
        GTest::gtest_main
        memutils
)

target_link_libraries(TestDPLL_MCL PRIVATE
        GTest::gtest
        GTest::gtest_main
        memutils
)

include(GoogleTest)
gtest_discover_tests(TestDP)
gtest_discover_tests(TestDPLL_RAND)
gtest_discover_tests(TestDPLL_MCL)
gtest_discover_tests(TestResolution)

