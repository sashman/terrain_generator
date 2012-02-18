/*
 * rivers.cpp
 *
 *
 *	Used to generate fresh water sources on the map
 *  Created on: 10 Feb 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"

extern int crop_height;
extern int crop_width;
extern int** tmap;

extern int sea_level;
extern int snowtop_level;

int n_rivers = DEFAULT_NO_OF_RIVERS;

int** source_location;

class RiverPoint {
public:
	int x;
	int y;
	int river_id;
	RiverPoint* next;

	RiverPoint(int x_, int y_, int river_id_) {
		x = x_;
		y = y_;
		river_id = river_id_;
		next = 0;
	}
};

RiverPoint** source_river_points;

bool get_river_source(int river_index) {

	//start with random
	int x = rand() % crop_width;
	int y = rand() % crop_height;

	while (point_below_snow_top_level(x, y)) {
		x = rand() % crop_width;
		y = rand() % crop_height;
	}

	source_location[river_index][0] = x;
	source_location[river_index][1] = y;

}

int cutoff_thresh = 100;
int cutoff_count = 0;


int get_tile_river_id(int x, int y){

	for (int i = 0; i < n_rivers; ++i) {
		RiverPoint *rp = source_river_points[i];
		do{
			if(rp!=0 && rp->x == x && rp->y == y) return i;
			rp = rp->next;
		}while(rp!=0);
	}
	return -1;

}

void grow(RiverPoint *rp) {

	//if at sea level
	if (!point_above_sealevel(rp->x, rp->y))
		return;

	//find smallest neighbour
	int true_min = tmap[rp->x][rp->y];
	int true_min_id = -1;

	//used in the case of local minimum#
	//TODO: needs inf value
	int min = 999;
	int min_id = 0;


	//north
	int n_count = 8;
	int** neighbours = new int*[n_count];
	for (int i = 0; i < n_count; i++) {
		neighbours[i] = new int[2];
	}

	//north
	neighbours[0][0] = rp->x - 1;
	neighbours[0][1] = rp->y;

	//NE
	neighbours[1][0] = rp->x - 1;
	neighbours[1][1] = rp->y + 1;

	//east
	neighbours[2][0] = rp->x;
	neighbours[2][1] = rp->y + 1;

	//SE
	neighbours[3][0] = rp->x + 1;
	neighbours[3][1] = rp->y + 1;

	//south
	neighbours[4][0] = rp->x + 1;
	neighbours[4][1] = rp->y;

	//SW
	neighbours[5][0] = rp->x + 1;
	neighbours[5][1] = rp->y - 1;

	//west
	neighbours[6][0] = rp->x;
	neighbours[6][1] = rp->y - 1;

	//NW
	neighbours[7][0] = rp->x - 1;
	neighbours[7][1] = rp->y - 1;

	for (int i = 0; i < n_count; ++i){
		//check bounds
		if(neighbours[i][0]<0 || neighbours[i][0]>=crop_width) continue;
		if(neighbours[i][1]<0 || neighbours[i][1]>=crop_height) continue;
		if(get_tile_river_id(neighbours[i][0], neighbours[i][1]) == rp->river_id) continue;
		if(get_tile_river_id(neighbours[i][0], neighbours[i][1]) > -1) return;


		if(tmap[neighbours[i][0]][neighbours[i][1]] < true_min){
			true_min_id = i;
			true_min = tmap[neighbours[i][0]][neighbours[i][1]];
		}

		if(tmap[neighbours[i][0]][neighbours[i][1]] < min){
			min_id = i;
			min = tmap[neighbours[i][0]][neighbours[i][1]];
		}
	}

	if(true_min_id > -1){ //no local minimum

		//add new RiverPoint
		RiverPoint* new_rp = new RiverPoint(neighbours[true_min_id][0], neighbours[true_min_id][1], rp->river_id);
		rp->next = new_rp;

		grow(new_rp);
	} else {

		RiverPoint* new_rp = new RiverPoint(neighbours[min_id][0], neighbours[min_id][1], rp->river_id);
		rp->next = new_rp;

		grow(new_rp);

	}
	cutoff_count++;
	if(cutoff_count> cutoff_thresh){
		cutoff_count = 0;
		return;
	}


}

void rivers() {

	source_location = new int*[n_rivers];
	source_river_points = new RiverPoint*[n_rivers];

	for (int i = 0; i < n_rivers; ++i) {

		source_location[i] = new int[2];

		//create source
		std::cout << "Finding source for " << i << std::endl;
		while (!get_river_source(i));
		source_river_points[i] = new RiverPoint(source_location[i][0],
				source_location[i][1], i);
		std::cout << "Source "<< i << " done" << std::endl;

	}
	for (int i = 0; i < n_rivers; ++i) {


		//grow river
		grow(source_river_points[i]);
		std::cout << "River " << i << " grown" << std::endl;
	}

}

void print_rivers(FILE* stream) {
	if (stream == 0) {
		stream = fopen(DEAFULT_RIVERS_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {

		fprintf(stream, "<rivers>\n");
		for (int i = 0; i < n_rivers; ++i) {

			fprintf(stream, "<river id = '%d' source_x='%d' source_y='%d'>\n",
					i, source_location[i][0], source_location[i][1]);

			RiverPoint* rp = source_river_points[i];
			do{

				if(rp!=0){
					fprintf(stream, "<river_point x = '%d' y = '%d'></river_point>\n", rp->x, rp->y);
				}

				rp=rp->next;
			}while(rp!=0);

			fprintf(stream, "</river>\n");
		}
		fprintf(stream, "</rivers>\n");

	}
}
