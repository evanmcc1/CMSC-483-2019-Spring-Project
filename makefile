all: routeFinder generateCity
	
routeFinder: routeFinder.cpp
	gcc -g -Wall -o routeFinder routeFinder.cpp -lm

generateCity: generateCity.cpp
	gcc -g -Wall -o generateCity generateCity.cpp -lm

run: routeFinder generateCity
	./generateCity | tee city.txt | ./routeFinder | tee route.txt
	./generateCity 16 | tee city16.txt | ./routeFinder | tee route16.txt
