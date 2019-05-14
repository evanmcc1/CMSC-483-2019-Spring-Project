#!/bin/bash
# Used to gather data across a wide range of input

# Record date to avoid identical filenames
date=$(date --iso-8601=ns)

# Recompile changes
make routeFinder generateCity

# Remove load from loops gracefully on kill
finish=0
trap 'finish=1' SIGTERM

# Explain easy exit options
echo "You can stop this script with \"kill $$\", optionally preceeded by \"^Z\" and anteceded by \"fg $$\", and still compile summary files"
echo "You can also do \"^Z\", check the output of \"jobs\" for the correct job number of $0, and do \"kill %1; fg %1\""
echo "In bash, you can do \"^Z\", \"job=\$(jobs | grep $0 | awk '{ print \$1 }' | sed 's/\[//;s/\]//;s/+//'); kill %\$job; fg %\$job\""


# Create primary data

# Increment vehicles
for i in {1..32}
do
# Increment locations
for j in {2..256}
do
	if [ $finish -eq 0 ]
	# the script hasn't been killed
	then
		# Record run settings and give an update on how far we are
		echo "$i $j" | tee -a routeSettings$date.txt
		# Time a run and record results
		{ time ./generateCity $j | ./routeFinder $i 0; } >> routeOutput$date.txt 2>> routeTimes$date.txt;
	fi
done
done


# Compile secondary data

# Collect running times
cat routeTimes$date.txt | sed -e '/^real/d' -e '/^sys/d' -e '/^$/d' -e 's/user\t//' -e 's/s//' -e 's/0m//' > routeUsertimes$date.txt
cat routeTimes$date.txt | sed -e '/^real/d' -e '/^user/d' -e '/^$/d' -e 's/sys\t//' -e 's/s//' -e 's/0m//' > routeSystimes$date.txt
# Collect total scores
cat routeOutput$date.txt | sed -e '/^Overall score: /!d' -e 's/Overall score: //' > routeScores$date.txt
# Collect route length statistics
cat routeOutput$date.txt | sed -e '/^Average route length: /!d' -e 's/Average route length: //' > routeLengthAverages$date.txt
cat routeOutput$date.txt | sed -e '/^Maximum route length: /!d' -e 's/Maximum route length: //' > routeLengthMaximums$date.txt
cat routeOutput$date.txt | sed -e '/^Minimum route length: /!d' -e 's/Minimum route length: //' > routeLengthMinimums$date.txt
# Separate route settings per variable
cat routeSettings$date.txt | awk '{ print $1 }' > routeVehicles$date.txt
cat routeSettings$date.txt | awk '{ print $2 }' > routeLocations$date.txt
