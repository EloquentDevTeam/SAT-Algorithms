import argparse
import os
import platform

import psutil
import subprocess
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
from collections import defaultdict

from psutil import NoSuchProcess


def spawn_processes(algorithm: str, fls: list[str], jobs:int, result_path: str) -> list[subprocess.Popen]:
    apath = os.path.abspath(algorithm)
    processees = []
    for _ in range(jobs):
        if len(fls) == 0:
            break
        processees.append(subprocess.Popen([apath, fls[0], f'{result_path}/{os.path.basename(fls[0])}.result.txt']))
        fls.remove(fls[0])
    return processees


def get_files(folder: str):
    l = []
    for item in os.listdir(folder):
        path = f'{folder}/{item}'
        if os.path.isfile(path):
            l.append(f'{path}')
    return l

def start_benchmarks(algorithm: str, files: list[str], jobs: int, result_dir: str):
    while len(files) > 0:
        procs = spawn_processes(algorithm,files,jobs,result_dir)
        active_jobs = []

        while True:
            for proc in procs:
                pid = proc.pid
                try:
                    process = psutil.Process(pid)
                    if active_jobs.count(proc) != 0:
                        active_jobs.append(proc)
                    with process.oneshot():
                        name = process.name()
                        memory_usage = process.memory_info().rss / 1024
                        cpu_time = process.cpu_times().system + process.cpu_times().user

                        print(f'Name: {name}.\nMemory: {memory_usage}.\nCPU Time: {cpu_time}\n')
                except NoSuchProcess:
                    print(f"Could not find {pid}. Possibly closed")
                    active_jobs.remove(proc)
                    continue

            if len(active_jobs) == 0:
                break

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="benchmark.py",
                                     description="Benchmarks SAT algorithms")

    parser.add_argument('-j', '--jobs', default=1,type=int)
    parser.add_argument('-t', '--test-directory',
                        required=True,
                        help="Path to folder containing .cnf files")

    parser.add_argument('algorithm')

    args = parser.parse_args()

    plt.style.use("dark_background")
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

    result_dir = f'{args.test_directory}/results'
    try:
        os.mkdir(result_dir)
    except FileExistsError:
        pass

    files = get_files(args.test_directory)

    start_benchmarks(args.algorithm, files, args.jobs,result_dir)

    print("Done")
