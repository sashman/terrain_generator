//============================================================================
// Name        : terrain_generator.cpp
// Author      : os63
// Version     : 0.0.1
//============================================================================

#include <iostream>
using namespace std;

#define DEFAULT_SIZE 257

int map_size = DEFAULT_SIZE;
int **map;

int main(int argc, char** argv) {

	//init map array
	map = new int*[map_size];
	for (int i = 0; i < map_size; ++i) {
		map[i] = new int[map_size];
	}


	return 0;
}
