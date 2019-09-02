all: routeFinder generateCity
	
routeFinder: routeFinder.c
	gcc -g -Wall -o routeFinder routeFinder.c -lm

generateCity: generateCity.c
	gcc -g -Wall -o generateCity generateCity.c -lm

run: routeFinder generateCity
	./generateCity | tee city.txt | ./routeFinder | tee route.txt
	./generateCity 16 | tee city16.txt | ./routeFinder | tee route16.txt
