#!/bin/bash

# Check if a directory was provided
if [ $# -ne 1 ]; then
  echo "Usage: $0 /path/to/directory"
  exit 1
fi

dir="$1"

# Validate directory
if [ ! -d "$dir" ]; then
  echo "Error: '$dir' is not a directory."
  exit 1
fi

# Loop through all regular files in the directory
find "$dir" -type f | while IFS= read -r file; do
  echo "Cleaning: $file"
  gawk '
    BEGIN {skip=0}
    /^c[[:print:]]*/ {skip=1; next}
    skip {
      if ($0 ~ /p cnf[[:space:]]+/) {
        sub(/^.*p cnf[[:space:]]*/, "")
        skip=0
        print
      }
      next
    }
    {print}
  ' "$file" > "$file.tmp" && mv "$file.tmp" "$file"
done
