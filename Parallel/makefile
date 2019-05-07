routeFinder: routeFinder.cpp
	# floor only seems to like c++
	mpic++ -g -Wall -o routeFinder routeFinder.cpp

generateCity: generateCity.cpp
	# atoi only seems to like c++
	g++ -g -Wall -o generateCity generateCity.cpp

run: routeFinder generateCity
	./generateCity | tee city.txt | mpirun -n 4 ./routeFinder 4 | tee route.txt
	./generateCity 16 | tee city16.txt | mpirun -n 4 ./routeFinder 4 | tee route16.txt
