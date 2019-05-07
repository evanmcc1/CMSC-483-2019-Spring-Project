#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define defaultStops 8

int main(int argc, char* argv[])
{
	int stopsToGenerate = defaultStops;
	
	int i, j;
	
	// Get how many stops we should generate from arguments
	if (argc > 1)
	{
		stopsToGenerate = atoi(argv[1]);
	}
	
	// Seed randomness
	srand48(time(NULL));
	
	// Generate the city!
	// One data point per line
	
	// Record how many stops we're generating
	printf("%d\n", stopsToGenerate);
	
	// Generate internal data about stops
	for (i = 0; i < stopsToGenerate; i++)
	// each stop
	{
		// Replenishment rate
		printf("%f\n", drand48());
		
		// Maximum customers
		printf("%f\n", ceil(drand48() * stopsToGenerate));
	}
	
	// Generate the transportation matrix
	for (i = 0; i < stopsToGenerate; i++)
	// each starting point
	{
		for (j = 0; j < stopsToGenerate; j++)
		// each destination
		{
			if (i == j)
			// a stop is trying to go to itself
			{
				// No fare
				printf("%f\n", 0.0);
				
				// No distance
				printf("%f\n", 0.0);
			}
			else
			// we need this route's information
			{
				// Distance (20 miles max)
				double distance = drand48() * 20;
				
				// Fare
				// $2.50 initial charge plus 50 cents per 1/5 mile or 60 seconds
				// 2.5 to start, .5 / 1/5 = 2.5 per mile
				// https://www1.nyc.gov/site/tlc/passengers/taxi-fare.page
				printf("%f\n", 2.5 + 2.5 * distance);
				
				// Distance (precalculated)
				printf("%f\n", distance);
			}
		}
	}
}
