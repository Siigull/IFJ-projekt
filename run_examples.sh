#!/bin/bash

EXAMPLES_DIR="examples"

if [[ ! -d "$EXAMPLES_DIR" ]]; then
    echo "Directory $EXAMPLES_DIR does not exist. Exiting."
    exit 1
fi

for file in "$EXAMPLES_DIR"/*; do
    if [[ -f "$file" ]]; then
        echo "Processing file: $file"
        
        ./bin/app < "$file" > out.txt
        if [[ $? -ne 0 ]]; then
            echo "Error running ./bin/app for $file. Skipping."
            continue
        fi
        
        ./ic24int out.txt > /dev/null
        if [[ $? -ne 0 ]]; then
            echo "Error running ./ic24int for $file. Skipping."
            continue
        fi
        
        echo "Successfully processed $file."
    fi
done

echo "All files processed."