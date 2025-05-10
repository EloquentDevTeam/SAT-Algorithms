#!/usr/bin/env python3
import csv
import os
import sys

if __name__ == "__main__":
    args = sys.argv
    
    if(len(args) > 2):
        print("Invalid number of arguments.")
        exit(1)

    if not os.path.isfile(args[1]):
        print(f"File {args[1]} does not exist.")
        exit(1)

    tests = 0
    total_time = 0
    total_memory = 0
    total_clauses = 0

    with open(args[1], "r") as infile:
        reader = csv.reader(infile)
        header = next(reader)
        for line in reader:
            tests += 1
            total_time += int(line[2])
            total_memory += int(line[3])
            total_clauses += int(line[4])

    # Wonder how success rate is calculated?
    # It's this command:
    # $ cat file.csv | grep -e "SAT" -e "UNSAT" | wc -l

    print(f"FILE: {args[1]}")
    print(f"Total time (μs):\t\t {total_time:,}".replace(",", " "))
    print(f"Average time / case (μs):\t {total_time/tests:,}".replace(",", " "))
    print(f"Average memory / case (kb):\t {total_memory/tests:,}".replace(",", " "))
    print(f"Average clause number:\t\t {total_clauses/tests:,}".replace(",", " "))