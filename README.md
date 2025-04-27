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
3. You are now ready to run the tests! You can either run each one of them individually, or you can use the convenient `benchmark.py` script to run all of them for you (one directory at a time):
   ```
   $ python3 benchmark.py -t /path/to/folder/containing/cnf/files /path/to/algorithm
   ```

   You may also use the ``-j`` parameter to speed up the procedure, by letting the script spawn more instances of the same process, like so
   ```
   # python3 benchmark.py -t -j 10 /path/to/folder/containing/cnf/files /path/to/algorithm
   ```

    **NOTE** Some algorithms (in particular resolution) on big datasets require large amounts of RAM. **The benchmark script does NOT employ ANY memory limits on the spawned subprocesses.**
   **You have been officially warned**

This will generate a ``results`` directory inside the cnf files directory containing the output for each test and a CSV file with all runs.


## Creating tests

To create tests you may use the `mkcnf.py` script to create tests in a subfolder, as follows:
```
usage: mkcnf.py [-h] --clause-count CLAUSE_COUNT --clause-max-size CLAUSE_MAX_SIZE [--clause-min-size CLAUSE_MIN_SIZE] --literal-count LITERAL_COUNT
                [--batch-size BATCH_SIZE] [--name NAME] [--seed SEED] [--path PATH] [-f]
```

- `--clause-count` - how many clauses should there be generated
- `--clause-max-size` - maximum size of clauses (must be in \[0, literal_count\])
- `--literal-count` - how many literals should the test have (literals may appear in either polarity. This argument only controls the maximum absolute value of literals) - corresponds to `literal_count`
- `--batch-size` - how many tests should be generated (default: `1`)
- `--name` - how should the folder where the tests will be stored called
- `--seed` - seed for the random number generator to use (**MUST BE A NUMBER**)
- `--path` - where to place the subfolder (default: directory where the script is located)
- `-f`,  `--force` - overwrite all tests present in a given subdirectory. If this argument is not supplied, then the program will ask to overwrite the tests if it detects that the subfolder at `<--path>/<--name>` exists

This will generate a number of `.cnf` files you can use with the algorithms supplied in this repository

## Algorithms 
Currently, this set contains the following algorithms:

1. Resolution - Exhaustive/Naive approach (``-DENABLE_RESOLUTION_NAIVE``) as described in [this paper](https://dl.acm.org/doi/abs/10.1145/321033.321034)
2. Davis-Putnam (as described in [his 1962 paper](https://dl.acm.org/doi/abs/10.1145/321033.321034)) - Exhaustive/Naive approach (``-DENABLE_DP_NAIVE``)
3. Davis-Putnam-Logemann-Loveland (with the RAND literal choice policy) (``-DENABLE_DPLL_RAND``) - adapted to an iterative approach from [Implementing the Davis–Putnam Method by H. Shang and Mark E Stickel](https://www.math.ucdavis.edu/~deloera/TEACHING/MATH165/davisputnam.pdf)
4. Davis-Putnam-Logemann-Loveland (with the Most Common Literal (MCL) literal choice policy) (``-DENABLE_DPLL_MCL``) - same as above
