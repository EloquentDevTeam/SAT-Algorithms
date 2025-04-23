add_executable(dpll_mcl dpll_mcl.cpp)
target_sources(dpll_mcl PRIVATE dpll_utils.cpp)
target_sources(dpll_mcl PRIVATE FILE_SET HEADERS FILES dpll_utils.h)
target_link_libraries(dpll_mcl PRIVATE memutils)
