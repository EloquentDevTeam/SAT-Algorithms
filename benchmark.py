#!/usr/bin/env python3
import argparse
import csv
import os
import sys
import psutil
import subprocess
import time

from psutil import NoSuchProcess

term_width, _ = os.get_terminal_size()

def print_progress_bar(progress:int, total:int, algo: str, bar_length = 40):
    percent = progress / total
    filled_length = int(bar_length * percent)
    bar = '=' * filled_length + '-' * (bar_length - filled_length)

    sys.stdout.write('\x1b[2A')
    sys.stdout.write('\x1b[2K\r')
    print(f"Algorithm: {algo}")
    sys.stdout.write('\x1b[2K\r')
    print(f"Progress: |{bar}| {percent:.1%} ({progress}/{total})", flush=True)

def spawn_processes(algorithm: str, fls: list[str], jobs:int, result_path: str) -> (list[subprocess.Popen],list[str]):
    apath = os.path.abspath(algorithm)
    processees = []
    for _ in range(jobs):
        if len(fls) == 0:
            break
        processees.append((psutil.Popen([apath, fls[0], f'{result_path}/{os.path.basename(fls[0])}.result.txt']),fls[0][:]))
        fls.remove(fls[0])
    return processees


def get_files(folder: str):
    l = []
    for item in os.listdir(folder):
        path = f'{folder}/{item}'
        if os.path.isfile(path):
            l.append(f'{path}')
    return l
class PlotData:

    def __init__(self, algo: str, test: str):
        self.algo = os.path.basename(algo)
        self.test = os.path.basename(test)
        self.peak_memory_usage = 0
        self.cputime = 0.0

    def update(self, memusage, cputime):
        self.peak_memory_usage = max(self.peak_memory_usage, memusage)
        self.cputime = cputime

    def __str__(self):
        return f'Testul {self.test} - Max RSS: {self.peak_memory_usage}; CPU Time: {self.cputime}'

    def __repr__(self):
        return self.__str__()

def start_benchmarks(algorithm: str, files: list[str], jobs: int, result_dir: str) -> {str: PlotData}:
    results: {str: PlotData} = {}
    nmax = len(files)
    print("\n")
    while len(files) > 0:
        print_progress_bar(nmax - len(files) + 1, nmax, algorithm, term_width-40)

        procs = spawn_processes(algorithm,files,jobs,result_dir)
        active_jobs = []
        while True:
            for proc, test in procs:
                try:
                    if proc.status() == psutil.STATUS_ZOMBIE and proc in active_jobs:
                        active_jobs.remove(proc)
                    if proc.poll() is not None:
                        if proc.poll() != 0:
                            print(f'WARNING - ERROR EXIT CODE {proc.poll()} FOR TEST {os.path.basename(test)}')
                        if proc in active_jobs:
                            active_jobs.remove(proc)
                        continue
                    if active_jobs.count(proc) == 0:
                        active_jobs.append(proc)
                        results[test] = PlotData(algorithm,test)
                    with proc.oneshot():
                        memory_usage = proc.memory_info().rss / 1024
                        cpu_time = proc.cpu_times().system + proc.cpu_times().user
                        results[test].update(memory_usage,cpu_time)
                except NoSuchProcess:
                    if proc in active_jobs:
                        active_jobs.remove(proc)
                    continue

            if len(active_jobs) == 0:
                break
    return results

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="benchmark.py",
                                     description="Benchmarks SAT algorithms")

    parser.add_argument('-j', '--jobs', default=1,type=int)
    parser.add_argument('-t', '--test-directory',
                        required=True,
                        help="Path to folder containing .cnf files")

    parser.add_argument('algorithm')

    args = parser.parse_args()

    if not os.path.exists(args.test_directory):
        print(f'{args.test_directory} does not exist')
        exit(1)
    if not os.path.exists(args.algorithm):
        print(f'Algorithm {args.algorithm} does not exist')
        exit(1)
    if not os.path.isdir(args.test_directory):
        print(f'{args.test_directory} is not a directory.')
        exit(1)
    if not os.path.isfile(args.algorithm):
        print(f'{args.algorithm} is not a file')
        exit(1)

    result_dir = f'{args.test_directory}/{os.path.basename(args.algorithm)}_results'
    try:
        os.mkdir(result_dir)
    except FileExistsError:
        pass

    files = get_files(args.test_directory)
    res = start_benchmarks(args.algorithm, files, args.jobs,result_dir)
    with open(f'{result_dir}/benchmark_results.csv','w+') as result_writer:
        c_writer = csv.writer(result_writer)
        c_writer.writerow(['Test','Max RSS','Max CPU Time'])
        for key in res.keys():
            c_writer.writerow([res[key].test,res[key].peak_memory_usage, res[key].cputime])
    print("Done")
