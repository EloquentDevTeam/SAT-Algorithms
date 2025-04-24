# SAT Algorithms
A collection of satisfiability algorithms to be merged into LibLogic, eventually.

The programs are guaranteed to work with the benchmarks available at https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html


## Dependencies
- CMake (>=3.30)
- Compiler that supports C++20
- Python 3

  
## Usage
1. Download an archive of tests from https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html and extract it to a directory (preferably a separate one; there are hundreds of files per archive).
2. Run CMake
   ```
   $ mkdir build
   $ cmake -B build -G Makefile # This configure to build all algorithms. Append -DENABLE_<algo>:OFF to disable <algo>. More info in the algorithms section
   $ cd build
   $ make -j$(nproc) # or other commands on non-UNIX platforms.
   ```
3. You are now ready to run the tests! You can either run each one of them individually, or you can use the convenient `run_benchmark.sh` script to run all of them for you (one directory at a time):
   ```
   $ python3 benchmark.py -t /path/to/folder/containing/cnf/files /path/to/algorithm
   ```
   This will generate a ``results`` directory inside the cnf files directory containing the output for each test and a CSV file with all runs.

All scripts are verbose and the latter will output a `<program>_<date>_<time>.benchmark` file with all results.

Enjoy! =D

## Algorithms 
Currently, this set contains the following algorithms:

1. Resolution - Exhaustive/Naive approach (``-DENABLE_RESOLUTION_NAIVE``)
2. Davis-Putnam (as described in his 1962 paper) - Exhaustive/Naive approach (``-DENABLE_DP_NAIVE``)
3. Davis-Putnam-Logemann-Loveland (with the RAND literal choice policy) (``-DENABLE_DPLL_RAND``)
4. Davis-Putnam-Logemann-Loveland (with the Most Common Literal (MCL) literal choice policy) (``-DENABLE_DPLL_MCL``)
