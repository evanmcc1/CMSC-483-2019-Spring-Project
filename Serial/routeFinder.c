//https://sahebg.github.io/computersceince/travelling-salesman-problem-c-program/
/*Travelling Salesman Problem is defined as “Given a list of cities and the distances between each pair of cities, 
what is the shortest possible route that visits each city exactly once and returns to the origin city?” 
It is an NP-hard problem.

Bellman–Held–Karp algorithm: Compute the solutions of all subproblems starting with the smallest. 
Whenever computing a solution requires solutions for smaller problems using the above recursive equations, look up these solutions which are already computed. 
To compute a minimum distance tour, use the final equation to generate the 1st node, and repeat for the other nodes. 
For this problem, we cannot know which subproblems we need to solve, so we solve them all.
*/
#include <stdio.h>
#include <stdlib.h>

#define defaultStops 8

typedef struct {
	double fare;
	double distance;
} connection;

typedef struct {
	double replenishRate;
	int maxCustomers;
} location;

// Total number of stops in the city
int stopCount = defaultStops;
// How many vehicles are in our company
int vehicleCount = 1;
// Matrix for how to navigate the city
connection **routes;
// Array of information about locations
location *stops;
// Route information
int **route, **bestRoute;
int *routeLength, *bestRouteLength;
// I don't understand this variable
int cost = 0;

/*
int tsp(int c){
	int count, nearest_city = 999;
	int minimum = 999, temp;
	for(count = 0; count < stopCount; count++){
		if((matrix[c][count] != 0) && (visited_cities[count] == 0)){
			if(matrix[c][count] < minimum){
				minimum = matrix[count][0] + matrix[c][count];
			}
			temp = matrix[c][count];
			nearest_city = count;
		}
	}
	if(minimum != 999){
		cost = cost + temp;
	}
	return nearest_city;
}
*/

/*
void minimum_cost(int city){
	int nearest_city;
	visited_cities[city] = 1;
	printf("%d ", city + 1);
	nearest_city = tsp(city);
	if(nearest_city == 999){
		nearest_city = 0;
		printf("%d", nearest_city + 1);
		cost = cost + matrix[city][nearest_city];
		return;
	}
 minimum_cost(nearest_city);
}
*/

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
	
	printf("Enter Total Number of stops:\t");
	scanf("%d", &stopCount);
	
	// Allocate data in arrays
	
	// Matrix for how to navigate the city
	routes = malloc(sizeof(connection*) * stopCount);
	
	for (i = 0; i < stopCount; i++)
	{
		routes[i] = malloc(sizeof(connection) * stopCount);
	}
	
	// Array of information about locations
	stops = malloc(sizeof(location) * stopCount);
	
	// Route information
	// One route per vehicle
	route = malloc(sizeof(int*) * vehicleCount);
	bestRoute = malloc(sizeof(int*) * vehicleCount);
	routeLength = malloc(sizeof(int) * vehicleCount);
	bestRouteLength = malloc(sizeof(int) * vehicleCount);
	
	for (i = 0; i < vehicleCount; i++)
	// each vehicle
	{
		// Each vehicle gets space for a route
		// The route probably shouldn't be longer than a hamiltonian path, so equal to the number of stops
		route[i] = malloc(sizeof(int) * stopCount);
		bestRoute[i] = malloc(sizeof(int) * stopCount);
	}
	
	printf("\nEnter customer replenishment vector\n");
	
	for (i = 0; i < stopCount; i++)
	{
		printf("\nEnter replenishRate, maxCustomers for row [%d]. One number per line\n", i);
		
		scanf("%lf", &stops[i].replenishRate);
		scanf("%d", &stops[i].maxCustomers);
	}
	
	printf("\nEntered replenishment vector\n(replenishRate, maxCustomers)\n");
	
	for(i = 0; i < stopCount; i++)
	{
		printf("(%lf, %d)\n", stops[i].replenishRate, stops[i].maxCustomers);
	}
	
	printf("\nEnter cost matrix\n");
	
	for(i = 0; i < stopCount; i++)
	{
		printf("\nEnter %d elements of fare, distance for row [%d]. One number per line\n", stopCount, i);
		
		for(j = 0; j < stopCount; j++)
		{
			scanf("%lf", &routes[i][j].fare);
			scanf("%lf", &routes[i][j].distance);
		}
	}
	
	printf("\nEntered cost matrix\n(fare, distance)\n");
	
	for(i = 0; i < stopCount; i++)
	{
		// Output first column
		printf("(%lf, %lf)", routes[i][0].fare, routes[i][0].distance);
		
		// Output other columns
		for(j = 1; j < stopCount; j++)
		{
			printf(" (%lf, %lf)", routes[i][j].fare, routes[i][j].distance);
		}
		
		printf("\n");
	}
	
	printf("\n\nPath:\t");
	
	//minimum_cost(0);
	
	printf("\n\nMinimum Cost: \t");
	//printf("%d\n", cost);
	
	printf("\n");
	
	return 0;
}
