#!/bin/bash

# Used to exercise the system and record outputs
# Takes as parameters:
#	Number of locations in the city
#	Number of vehicles in the fleet
#	Verbosity input

# Exit on error
set -e

# Recompile changes
make routeFinder generateCity

# Execute code
./generateCity $1 | tee city$1.txt | ./routeFinder $2 $3 | tee route$1-$2.txt
