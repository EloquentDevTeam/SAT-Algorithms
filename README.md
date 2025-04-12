# SAT Algorithms
A collection of satisfiability algorithms to be merged into LibLogic, eventually.

The programs are guaranteed to work with the benchmarks available at https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

## Intended use
(UNIX and compile dependencies are assumed existent)

1. Download an archive of tests from https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html and extract it to a directory (preferably a separate one; there are hundreds of files per archive).
2. Each test file contains a header according to the DIMACS standard which we do not need. Clean up the test files using the `clean.sh` script:
   ```
   $ chmod +x clean.sh    # makes sure the script is executable
   $ ./clean.sh <test_case_directory>
   ```
3. Compile your algorithm of choice (make sure you're in the right directory):
   ```
   $ g++ -O3 ./<directory/algorithm>.cpp -o <algorithm>
   ```
4. You are now ready to run the tests! You can either run each one of them individually, or you can use the convenient `run_benchmark.sh` script to run all of them for you (one directory at a time):
   ```
   $ chmod +x run_benchmark.sh    # makes sure the script is executable
   $ ./run_benchmark.sh <test_case_directory> <algorithm>
   ```

All scripts are verbose and the latter will output a `<program>_<date>_<time>.benchmark` file with all results.

Enjoy! =D
