# SAT Algorithms
A collection of satisfiability algorithms to be merged into LibLogic, eventually.

The programs are guaranteed to work with the benchmarks available at https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html


## Dependencies
- CMake (>=3.30)
- Compiler that supports C++20
- GNU Make

  
## Usage
1. Download an archive of tests from https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html and extract it to a directory (preferably a separate one; there are hundreds of files per archive).
2. Run CMake to choose which algorithms to build. By default **all algorithms** will be built. If you want to only build some algorithms, use
   ```
   $ mkdir build
   $ cmake -B build -G Makefile # This will build all algorithms. Append -DENABLE_<algo>:OFF to disable <algo>. More info in the algorithms section
   $ cd build
   $ make -j$(nproc)
   ```
3. You are now ready to run the tests! You can either run each one of them individually, or you can use the convenient `run_benchmark.sh` script to run all of them for you (one directory at a time):
   ```
   $ chmod +x run_benchmark.sh    # makes sure the script is executable
   $ ./run_benchmark.sh <test_case_directory> <algorithm>
   ```

All scripts are verbose and the latter will output a `<program>_<date>_<time>.benchmark` file with all results.

Enjoy! =D

## Algorithms 
Currently, there are 3 algorithms offered

1. Resolution - Exhaustive/Naive approach (``-DENABLE_RESOLUTION_NAIVE``)
2. Davis-Putnam (as described in his 1962 paper) - Exhaustive/Naive approach (``-DENABLE_DP_NAIVE``)
3. Davis-Putnam-Logemann-Loveland (with the RAND literal choice policy) (``-DENABLE_DPLL_RAND``)
4. Davis-Putnam-Logemann-Loveland (with the Most Common Literal (MCL) literal choice policy) (``-DENABLE_DPLL_MCL``)