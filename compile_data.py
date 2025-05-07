#!/usr/bin/env python3
import os
import csv
import argparse

def get_files(folder: str):
    l = []
    for item in os.listdir(folder):
        path = f'{folder}/{item}'
        _, ext = os.path.splitext(item)
        if os.path.isfile(path) and ext == ".txt":
            l.append(f'{path}')
    return l

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog = "compile_data.py", description = "Data compiler for CNF test results.")

    parser.add_argument('--path', type=str, required=True, help="Directory of test results.")
    parser.add_argument('--output', type=str, required=True, help="Name of output csv file.")

    args = parser.parse_args()

    if not os.path.exists(args.path):
        print(f"Directory {args.path} does not exist.")
        exit(1)
    if not os.path.isdir(args.path):
        print(f"{args.path} is not a directory.")
        exit(1)
    
    files = get_files(args.path)

    with open(args.output, "w+") as outfile:
        writer = csv.writer(outfile)
        writer.writerow(["test", "result", "time (μs)", "memory (kb)", "clauses"])
        
        data = []

        for f in files:
            row = []
            with open(f, "r") as infile:
                name, _ = os.path.splitext(os.path.basename(infile.readline()))
                row.append(int(name.strip("test")))

                result = infile.readline()
                if "timeout" in result:
                    row.append("timeout")
                elif "Rezultat: SAT" in result:
                    row.append("SAT")
                else:
                    row.append("UNSAT")

                clause_no = int(infile.readline().strip().split()[-1])
                exec_time = int(infile.readline().strip().split()[-1][:-2])
                infile.readline()
                memory = int(infile.readline().strip().split()[-2])

                row.append(exec_time)
                row.append(memory)
                row.append(clause_no)
            data.append(row)
        
        writer.writerows(sorted(data, key=lambda x: x[0]))
