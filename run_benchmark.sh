#!/bin/bash

# Check if the directory and program name were provided
if [ $# -ne 2 ]; then
  echo "Usage: $0 <directory> <program>"
  exit 1
fi

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")

directory="$1"
program="./$2"
output_file="$2_$timestamp.benchmark"

# Check if the directory exists
if [ ! -d "$directory" ]; then
  echo "Error: '$directory' is not a valid directory."
  exit 1
fi

# Check if program is compiled
if ! command -v "$program" &> /dev/null; then
  echo "Error: Program '$program' not found in your PATH. Make sure you actually compiled it."
  exit 1
fi

# Clear the output file if it exists
> "$output_file"

# Loop over each file in the directory (ignores subdirectories)
for file in "$directory"/*; do
  if [ -f "$file" ]; then
    echo "On test: $file"
    printf '%0.s-' {1..40} >> "$output_file"
    echo -e "\nTest: $file" >> "$output_file"
    "$program" "$file" >> "$output_file" 2>&1
  fi
done

echo "All output has been written to $output_file"
