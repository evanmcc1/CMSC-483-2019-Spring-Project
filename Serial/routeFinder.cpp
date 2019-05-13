// Base from: 
//https://sahebg.github.io/computersceince/travelling-salesman-problem-c-program/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DEFAULTSTOPS 8
#define MINSTOPS 3
//TODO: Is this a good number of cycles?
#define CYCLECOUNT 16
#define CITYWIDTH 20

#define ERRORVERBOSITY 1
#define INSTRUCTIONVERBOSITY 2
#define SANITYVERBOSITY 3
#define INSANITYVERBOSITY 4

void bulkRoute(int);
void pruneLoops(int);

typedef struct {
	double fare;
	double distance;
} connection;

typedef struct {
	double replenishRate;
	int maxCustomers;
} location;

// Total number of stops in the city
int stopCount = DEFAULTSTOPS;
// How many vehicles are in our company
int vehicleCount = 1;
// Matrix for how to navigate the city
connection **routes;
// Array of information about locations
location *stops;
// Route information
int **vehicleRoute, **bestRoute;
int *vehicleRouteLength, *bestRouteLength;
// How much we make from a vehicleRoute ($/mile)
double score, bestScore;
// How much the vehicleRoute can be edited
// Inverse is the probability of replacing bad vehicleRoute
int temperature = DEFAULTSTOPS;
// How much output the program gives
int verbosity = 0;

// Place stop into vehicleRoute at position
void insertIntoRoute(int stop, int vehicle, int position)
{
	int i;
	
	// Exit if vehicleRoute is too long already
	if (vehicleRouteLength[vehicle] >= stopCount)
	{
		return;
	}
	
	if (vehicleRoute[vehicle][position] == stop || (position-1 >= 0 && vehicleRoute[vehicle][position-1] == stop))
	// stop would be next to itself (position or position-1 == stop)
	{
		return;
	}
	
	// Shift existing stops to the right
	for (i = vehicleRouteLength[vehicle]; i > position; i--)
	{
		vehicleRoute[vehicle][i] = vehicleRoute[vehicle][i-1];
	}
	
	// Put stop into array
	vehicleRoute[vehicle][position] = stop;
	
	// Update length
	vehicleRouteLength[vehicle]++;
}

void deleteFromRoute(int vehicle, int position)
{
	int i;
	
	// Make sure there will be a valid vehicleRoute left
	bulkRoute(vehicle);
	
	// Shift stops to the left
	for (i = position; i+1 < vehicleRouteLength[vehicle]; i++)
	{
		routes[vehicle][i] = routes[vehicle][i+1];
	}
	
	// Update length
	vehicleRouteLength[vehicle]--;
	
	// Remove stops next to themselves
	pruneLoops(vehicle);
}

void insertRandomStop(int vehicle)
{
	insertIntoRoute((int)floor(stopCount * drand48()), vehicle, (int)floor(vehicleRouteLength[vehicle] * drand48()));
}

void removeRandomStop(int vehicle)
{
	deleteFromRoute(vehicle, (int)floor(vehicleRouteLength[vehicle] * drand48()));
}

void bulkRoute(int vehicle)
{
	while (vehicleRouteLength[vehicle] < MINSTOPS)
	// there are too few stops
	{
		// Add more!
		insertRandomStop(vehicle);
	}
}

void pruneLoops(int vehicle)
{
	int i;
	
	//TODO: Remove stops that are next to themselves
	for (i = 0; i+1 < vehicleRouteLength[vehicle]; i++)
	{
		if (vehicleRoute[vehicle][i] == vehicleRoute[vehicle][i+1])
		{
			deleteFromRoute(vehicle, i);
			
			// We have to check what moves in
			i--;
		}
	}
	
	// Add stops to small routes
	bulkRoute(vehicle);
}

// Add and remove random stops to a vehicleRoute
void generateRoute(int vehicle)
{
	// Add stops randomly
	// Randomize how many stops to add
	int addStops = (int)ceil(temperature * drand48());
	
	if (addStops + vehicleRouteLength[vehicle] > stopCount)
		addStops = stopCount - vehicleRouteLength[vehicle];
	
	while (addStops > 0)
	{
		insertRandomStop(vehicle);
		addStops--;
	}
	
	// Remove stops randomly
	// Randomize how many stops to remove
	int removeStops = (int)floor(temperature * drand48());
	
	if (removeStops > stopCount - MINSTOPS)
		removeStops = stopCount - MINSTOPS;
	
	while (removeStops > 0)
	{
		removeRandomStop(vehicle);
		removeStops--;
	}
}

// Edits all routes
void generateRoutes()
{
	int i;
	
	//TODO: This could be parallelized (threads)
	for (i = 0; i < vehicleCount; i++)
	{
		generateRoute(i);
	}
}

// Score a vehicleRoute based on fare per mile
double testRoutes()
{
	int i;
	
	// Reset score to track this test
	score = 0;
	
	// Counters to know when we're done testing
	double targetMiles = CITYWIDTH * 8;
	double currentMiles = 0;
	
	// Locations of the vehicles
	int *vehicleLocations = (int*)malloc(sizeof(int) * vehicleCount);
	// Where (and whether) the vehicles are on their routes
	int *vehicleRoutePositions = (int*)malloc(sizeof(int) * vehicleCount);
	// When the vehicles will arrive
	double *vehicleArrivals = (double*)malloc(sizeof(double) * vehicleCount);
	
	// How many customers have accumulated
	double *readyCustomers = (double*)malloc(sizeof(double) * stopCount);
	
	// Set customers at each stop
	for (i = 0; i < stopCount; i++)
	{
		readyCustomers[i] = stops[i].replenishRate;
		
		// Ensure there aren't too many customers
		if (readyCustomers[i] > stops[i].maxCustomers)
		{
			readyCustomers[i] = stops[i].maxCustomers;
		}
	}
	
	// Set each vehicle at its starting point and arrive
	for (i = 0; i < vehicleCount; i++)
	{
		vehicleLocations[i] = vehicleRoute[i][0];
		vehicleRoutePositions[i] = 0;
		vehicleArrivals[i] = 0;
	}
	
	while (currentMiles < targetMiles)
	{
		// Find vehicle with lowest arrival miles
		// That means it arrived before other vehicles, so it needs to get fares first
		double minimumArrival;
		int arrivedVehicle = 0;
		
		#ifdef INFINITY
		minimumArrival = INFINITY;
		#else
		minimumArrival = 999999;
		#endif
		
		for (i = 0; i < vehicleCount; i++)
		{
			if (vehicleArrivals[i] < minimumArrival)
			{
				arrivedVehicle = i;
				minimumArrival = vehicleArrivals[i];
			}
		}
		
		// Step time forward to when the vehicle arrives
		// This means adding passengers to locations
		for (i = 0; i < stopCount; i++)
		// each stop
		{
			// Customers replenish at 1 replenishRate per mile traveled
			// Therefore, track how many miles were traveled!
			readyCustomers[i] += stops[i].replenishRate * (minimumArrival - currentMiles);
			
			// Ensure there aren't too many customers
			if (readyCustomers[i] > stops[i].maxCustomers)
			{
				readyCustomers[i] = stops[i].maxCustomers;
			}
		}
		
		// Step time forward
		currentMiles = minimumArrival;
		
		int destination;
		
		if (readyCustomers[vehicleLocations[arrivedVehicle]] >= 1)
		// I can pick up a passenger
		{
			// Pick up this passenger
			readyCustomers[vehicleLocations[arrivedVehicle]]--;
			
			// Randomize destination
			//TODO: Base destination on replenishment rate
			destination = (int)floor(stopCount * drand48());
			
			// Assume we collect fare
			score += routes[vehicleLocations[arrivedVehicle]][destination].fare;
			
			// Set next vehicle arrival mileage
			// Minimum of 1 mile
			vehicleArrivals[arrivedVehicle] = currentMiles + fmax(routes[vehicleLocations[arrivedVehicle]][destination].distance, 1);
			
			// Assume the vehicle got there
			vehicleLocations[arrivedVehicle] = destination;
			
			// This may have diverted the vehicle from its route
			// Mark that it needs to recalculate where it is / go to location 0
			vehicleRoutePositions[arrivedVehicle] = -1;
		}
		else
		{
			// Continue to route
			// Find what the destination is
			if (vehicleRoutePositions[arrivedVehicle] >= 0)
			// currently at a stop on my route
			{
				// Increment route position
				vehicleRoutePositions[arrivedVehicle]++;
				
				if (vehicleRoutePositions[arrivedVehicle] >= vehicleRouteLength[arrivedVehicle])
					vehicleRoutePositions[arrivedVehicle] = 0;
			}
			else
			{
				// We are in the middle of nowhere, so go to the start
				// Go to route element 0
				vehicleRoutePositions[arrivedVehicle] = 0;
			}
			
			// Find what the actual destination is
			destination = vehicleRoute[arrivedVehicle][vehicleRoutePositions[arrivedVehicle]];
			
			// Set next vehicle arrival mileage
			vehicleArrivals[arrivedVehicle] = currentMiles + routes[vehicleLocations[arrivedVehicle]][destination].distance;
			
			// Assume the vehicle gets there
			vehicleLocations[arrivedVehicle] = destination;
		}
	}
	
	// Divide score by miles * vehicles
	// We only tracked fare, but we want maximum fare/mile
	score /= currentMiles;
	score /= vehicleCount;
	
	return score;
}

// Generate test routes until we're satisfied the best is found
void findRoutes()
{
	// Set temperature proportional to problem size
	temperature = stopCount * vehicleCount;
	
	if (verbosity >= SANITYVERBOSITY)
	{
		printf("\nin findRoutes\n");
	}
	
	int i, j, k;
	
	// Seed randomness
	srand48(time(NULL));
	
	// Check that every vehicleRoute has at least two locations
	for (i = 0; i < vehicleCount; i++)
	{
		bulkRoute(i);
	}
	
	// Reset vehicleRoute scores
	score = 0;
	bestScore = 0;
	
	while (temperature >= 1)
	{
		for (i = 0; i < CYCLECOUNT; i++)
		{
			//Test routes
			testRoutes();
			
			// Save best vehicleRoute
			if (score > bestScore)
			{
				if (verbosity >= SANITYVERBOSITY)
				{
					printf("Before best replacement:\n");
					printf("best vehicle routes (working, best):\n");
					printf("score: %lf, %lf\n", score, bestScore);
					
					for(i = 0; i < vehicleCount; i++)
					{
						printf("length: %d, %d\n", vehicleRouteLength[i], bestRouteLength[i]);
						
						printf("elements:\n");
						
						for(j = 0; j < stopCount; j++)
						{
							if (j < vehicleRouteLength[i]) printf("\t");
							printf("%d %d\n", vehicleRoute[i][j], bestRoute[i][j]);
						}
					}
				}
				
				// Make best vehicle route equal to working vehicle route
				for(j = 0; j < vehicleCount; j++)
				{
					for(k = 0; k < stopCount; k++)
					{
						bestRoute[j][k] = vehicleRoute[j][k];
					}
					
					bestRouteLength[j] = vehicleRouteLength[j];
				}
				
				bestScore = score;
				
				if (verbosity >= SANITYVERBOSITY)
				{
					printf("After best replacement:\n");
					printf("best vehicle routes (working, best):\n");
					printf("score: %lf, %lf\n", score, bestScore);
					
					for(i = 0; i < vehicleCount; i++)
					{
						printf("length: %d, %d\n", vehicleRouteLength[i], bestRouteLength[i]);
						
						printf("elements:\n");
						
						for(j = 0; j < stopCount; j++)
						{
							if (j < vehicleRouteLength[i]) printf("\t");
							printf("%d %d\n", vehicleRoute[i][j], bestRoute[i][j]);
						}
					}
				}
			}
			
			// Sometimes abandon working copy and use best routes
			if (drand48() < 1.0 / temperature)
			{
				if (verbosity >= INSANITYVERBOSITY)
				{
					printf("Before working replacement:\n");
					printf("best vehicle routes (working, best):\n");
					printf("score: %lf, %lf\n", score, bestScore);
					
					for(i = 0; i < vehicleCount; i++)
					{
						printf("length: %d, %d\n", vehicleRouteLength[i], bestRouteLength[i]);
						
						printf("elements:\n");
						
						for(j = 0; j < stopCount; j++)
						{
							if (j < bestRouteLength[i]) printf("\t");
							printf("%d %d\n", vehicleRoute[i][j], bestRoute[i][j]);
						}
					}
				}
				
				//Copy best route back to working set
				for(j = 0; j < vehicleCount; j++)
				{
					for(k = 0; k < stopCount; k++)
					{
						vehicleRoute[j][k] = bestRoute[j][k];
					}
					
					vehicleRouteLength[j] = bestRouteLength[j];
				}
				
				score = bestScore;
				
				if (verbosity >= INSANITYVERBOSITY)
				{
					printf("After working replacement:\n");
					printf("best vehicle routes (working, best):\n");
					printf("score: %lf, %lf\n", score, bestScore);
					
					for(i = 0; i < vehicleCount; i++)
					{
						printf("length: %d, %d\n", vehicleRouteLength[i], bestRouteLength[i]);
						
						printf("elements:\n");
						
						for(j = 0; j < stopCount; j++)
						{
							if (j < bestRouteLength[i]) printf("\t");
							printf("%d %d\n", vehicleRoute[i][j], bestRoute[i][j]);
						}
					}
				}
			}
			
			// Edit routes
			generateRoutes();
		}
		
		// Reduce temperature
		//TODO: Is this a good way to reduce temperature?
		temperature--;
	}
}

int main(int argc, char* argv[])
{
	int i, j;
	
	// Get how many vehicle we should use from arguments
	if (argc > 1)
	{
			vehicleCount = atoi(argv[1]);
	}
	else
		vehicleCount = 1;
	
	if (argc > 2)
	{
			verbosity = atoi(argv[2]);
	}
	else
		verbosity = 0;
	
	if (vehicleCount < 1)
	// there are no vehicles
	{
		// Exit
		if (verbosity >= ERRORVERBOSITY)
			printf("No vehicles. Do not supply a number less than 1\n");
		return 1;
	}
	
	if (verbosity >= INSTRUCTIONVERBOSITY) printf("Enter Total Number of stops:\t");
	scanf("%d", &stopCount);
	
	if (stopCount < MINSTOPS)
	// there are no possible routes
	{
		// Exit
		if (verbosity >= ERRORVERBOSITY)
			printf("No possible useful routes. Do not supply a number less than %d\n", MINSTOPS);
		return 2;
	}
	
	// Allocate data in arrays
	
	// Matrix for how to navigate the city
	routes = (connection**)malloc(sizeof(connection*) * stopCount);
	
	for (i = 0; i < stopCount; i++)
	{
		routes[i] = (connection*)malloc(sizeof(connection) * stopCount);
	}
	
	// Array of information about locations
	stops = (location*)malloc(sizeof(location) * stopCount);
	
	// Route information
	// One vehicleRoute per vehicle
	vehicleRoute = (int**)malloc(sizeof(int*) * vehicleCount);
	bestRoute = (int**)malloc(sizeof(int*) * vehicleCount);
	vehicleRouteLength = (int*)malloc(sizeof(int) * vehicleCount);
	bestRouteLength = (int*)malloc(sizeof(int) * vehicleCount);
	
	// Make vehicleRoute lengths 0 so we know to generate some
	for (i = 0; i < vehicleCount; i++)
	{
		vehicleRouteLength[i] = 0;
		bestRouteLength[i] = 0;
	}
	
	for (i = 0; i < vehicleCount; i++)
	// each vehicle
	{
		// Each vehicle gets space for a vehicleRoute
		// The vehicleRoute probably shouldn't be longer than a hamiltonian path, so equal to the number of stops
		vehicleRoute[i] = (int*)malloc(sizeof(int) * stopCount);
		bestRoute[i] = (int*)malloc(sizeof(int) * stopCount);
	}
	
	if (verbosity >= INSTRUCTIONVERBOSITY) printf("\nEnter customer replenishment vector\n");
	
	for (i = 0; i < stopCount; i++)
	{
		if (verbosity >= INSTRUCTIONVERBOSITY) printf("\nEnter replenishRate, maxCustomers for row [%d]. One number per line\n", i);
		
		scanf("%lf", &stops[i].replenishRate);
		scanf("%d", &stops[i].maxCustomers);
		
		//TODO: Ensure these values are nonzero
	}
	
	if (verbosity >= SANITYVERBOSITY)
	{
		printf("\nEntered replenishment vector\n(replenishRate, maxCustomers)\n");
		
		for(i = 0; i < stopCount; i++)
		{
			printf("(%lf, %d)\n", stops[i].replenishRate, stops[i].maxCustomers);
		}
	}
	
	if (verbosity >= INSTRUCTIONVERBOSITY) printf("\nEnter cost matrix\n");
	
	for(i = 0; i < stopCount; i++)
	{
		if (verbosity >= INSTRUCTIONVERBOSITY) printf("\nEnter %d elements of fare, distance for row [%d]. One number per line\n", stopCount, i);
		
		for(j = 0; j < stopCount; j++)
		{
			scanf("%lf", &routes[i][j].fare);
			scanf("%lf", &routes[i][j].distance);
			
			//TODO: Ensure these values are nonzero
			// NYC taxis charge $2.50 initial charge plus 50 cents per 1/5 mile or 60 seconds
			// 2.5 to start, .5 / 1/5 = 2.5 per mile
			// https://www1.nyc.gov/site/tlc/passengers/taxi-fare.page
		}
	}
	
	if (verbosity >= SANITYVERBOSITY)
	{
		printf("\nEntered cost matrix\n(fare, distance)\n");
		
		for(i = 0; i < stopCount; i++)
		{
			// Output first column
			printf("(%lf, %lf)", routes[i][0].fare, routes[i][0].distance);
			
			// Output other columns
			for(j = 1; j < stopCount; j++)
			{
				printf("\t(%lf, %lf)", routes[i][j].fare, routes[i][j].distance);
			}
			
			printf("\n");
		}
	}
	
	if (verbosity >= INSTRUCTIONVERBOSITY) printf("\n\nCalculating routes...");
	
	findRoutes();
	
	// Print routes
	if (verbosity >= INSTRUCTIONVERBOSITY) printf("\n\nRoutes:\n");
	
	// Print routes (print node IDs in for loop)
	printf("Score: %lf\n", bestScore);
	
	for (i = 0; i < vehicleCount; i++)
	{
		printf("Route %d, length %d:\n", i, bestRouteLength[i]);
		
		for (j = 0; j < bestRouteLength[i]; j++)
		{
			printf("%d\n", bestRoute[i][j]);
		}
	}
	
	printf("\n");
	
	return 0;
}
