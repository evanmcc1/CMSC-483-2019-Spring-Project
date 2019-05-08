#!/bin/bash

# Recompile changes
make routeFinder generateCity

# Remove old data
rm routeSettings.txt
rm routeResults.txt
rm routeOutput.txt

# Increment vehicles
for i in {1..32}
do
# Increment locations
# RouteFinder crashes if there are fewer locations than vehicles
# Ensure there are at least 3 locations
for j in $( seq $(($i>3?$i:3)) 32 )
do
	echo "$i $j" | tee -a routeSettings.txt
	{ time ./generateCity $j | ./routeFinder $i 0; } >> routeOutput.txt 2>> routeResults.txt;
done
done

cat routeResults.txt | sed -e '/^real/d' -e '/^sys/d' -e '/^$/d' -e 's/user\t//' -e 's/s//' > routeTimes.txt
cat routeOutput.txt | sed -e '/^S/!d' -e 's/Score: //' > routeScores.txt
# TODO: Collect average route length
