cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_RESOLUTION_NAIVE "" YES)
option(ENABLE_DP_NAIVE "Whether to build the Davis-Putnamm algorithm (naive approach)" YES)
option(ENABLE_DPLL_NAIVE "Whether to build the Davis–Putnam–Logemann–Loveland algorithm (naive approach)" YES)
project(sat-algorithms)


if (ENABLE_RESOLUTION_NAIVE)
    add_subdirectory(resolution_naive_first_fit)
endif ()
if (ENABLE_DPLL_NAIVE)
    add_subdirectory(dpll_naive)
endif ()