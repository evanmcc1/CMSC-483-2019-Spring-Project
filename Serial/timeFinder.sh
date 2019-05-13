#!/bin/bash

# Used to gather data across a wide range of input

# Recompile changes
make routeFinder generateCity

# Remove old data
rm routeSettings.txt
rm routeOutput.txt
rm routeResults.txt

# Increment vehicles
for i in {1..32}
do
# Increment locations
for j in {2..256}
do
	echo "$i $j" | tee -a routeSettings.txt
	{ time ./generateCity $j | ./routeFinder $i 0; } >> routeOutput.txt 2>> routeTimes.txt;
done
done

# Collect running times
cat routeTimes.txt | sed -e '/^real/d' -e '/^sys/d' -e '/^$/d' -e 's/user\t//' -e 's/s//' -e 's/0m//' > routeUsertimes.txt
cat routeTimes.txt | sed -e '/^real/d' -e '/^user/d' -e '/^$/d' -e 's/sys\t//' -e 's/s//' -e 's/0m//' > routeSystimes.txt
# Collect total scores
cat routeOutput.txt | sed -e '/^Overall score: /!d' -e 's/Overall score: //' > routeScores.txt
# Collect average route length
cat routeOutput.txt | sed -e '/^Average route length: /!d' -e 's/Average route length: //' > routeLengths.txt
# Separate route settings per variable
cat routeSettings.txt | awk '{ print $1 }' > routeVehicles.txt
cat routeSettings.txt | awk '{ print $2 }' > routeLocations.txt
