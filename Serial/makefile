routeFinder: routeFinder.c
	gcc -g -Wall -o routeFinder routeFinder.c

generateCity: generateCity.cpp
	g++ -g -Wall -o generateCity generateCity.cpp

run: routeFinder generateCity
	./generateCity 16 | tee city.txt | ./routeFinder
