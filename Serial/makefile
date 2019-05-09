all: routefinder generateCity
	
routeFinder: routeFinder.cpp
	# floor only seems to like c++
	g++ -g -Wall -o routeFinder routeFinder.cpp

generateCity: generateCity.cpp
	# atoi only seems to like c++
	g++ -g -Wall -o generateCity generateCity.cpp

run: routeFinder generateCity
	./generateCity | tee city.txt | ./routeFinder | tee route.txt
	./generateCity 16 | tee city16.txt | ./routeFinder | tee route16.txt
