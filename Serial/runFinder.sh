#!/bin/bash

# Exit on error
set -e

# Recompile changes
make routeFinder generateCity

# Execute code
./generateCity $1 | tee city$1.txt | ./routeFinder $2 $3 | tee route$1-$2.txt
