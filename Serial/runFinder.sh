#!/bin/bash
# Workflow script for particle sim

# Exit on error
set -e

# Recompile changes
make routeFinder generateCity

# Execute code
./generateCity $1 | tee city.txt | ./routeFinder
