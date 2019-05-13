#!/bin/bash
# Used to gather data across a wide range of input

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

# Remove old data
rm routeSettings.txt
rm routeOutput.txt
rm routeTimes.txt

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
		echo "$i $j" | tee -a routeSettings.txt
		# Time a run and record results
		{ time ./generateCity $j | ./routeFinder $i 0; } >> routeOutput.txt 2>> routeTimes.txt;
	fi
done
done


# Compile secondary data

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
