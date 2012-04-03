/*
 * vegetation.cpp
 *
 *
 * Places vegetation objects on the terrain
 *  Created on: 30 Mar 2012
 *      Author: sash
 */


#include "terrain_generator.hpp"
#include <vector>


extern int crop_height;
extern int crop_width;
extern int** tmap;

extern int snowtop_level;
extern int sea_level;

extern int n_rivers;
extern RiverPoint** source_river_points;

int n_vegetation = DEFAULT_NO_OF_VEGETATION;

std::vector<int*> candidate_veg_location;
std::vector<int*> veg_location;
std::vector<int*> water_tile_location;


bool point_at_river_tile(int x, int y){
	for (int i = 0; i < water_tile_location.size(); ++i) {
		if(water_tile_location[i][0] == x && water_tile_location[i][1] == y) return true;
	}
	return false;
}


bool veg_candidate_constraint(int x, int y) {

	bool result = true;
	result = result && point_below_snow_top_level(x, y) && point_above_sealevel(x,y) && !point_at_river_tile(x,y);
	//std::cout << "Snow level " << snowtop_level << "Sea level" << sea_level << " : " << tmap[y][x] << " " << result << std::endl;
	return result;
}

void calculate_veg_candiates() {

	for (int i = 0; i < crop_height; ++i) {

		for (int j = 0; j < crop_width; ++j) {

			if (veg_candidate_constraint(j, i)) {

				int *point = new int[2];
				point[0] = j;
				point[1] = i;
				candidate_veg_location.push_back(point);

			}

		}
	}

}


int get_branch_id(RiverPoint* rp) {

	do {
//		std::cout << "Branch " << rp->x << "," << rp->y << " ~ " << x << ","
//				<< y << std::endl;
		if (rp != 0)
			return rp->river_id;

		rp = rp->next;
	} while (rp != 0);

	return -1;
}

void calculate_water_tiles() {

	for (int i = 0; i < n_rivers; ++i) {
		RiverPoint *rp = source_river_points[i];
		do {
			if (rp != 0) {
				int* loc = new int[2];
				loc[0] = rp->x;
				loc[1] = rp->y;
				water_tile_location.push_back(loc);

				//check branches
				if (rp->branch != 0) {
					int r = get_branch_id(rp->branch);
					if (r > -1) {
						int* loc = new int[2];
						loc[0] = rp->x;
						loc[1] = rp->y;
						water_tile_location.push_back(loc);
					}
				}
				rp = rp->next;
			}

		} while (rp != 0);
	}

}

void populate_veg() {

	for (int j = 0; j < n_vegetation; ++j) {

		if (candidate_veg_location.empty())
			return;

		int i = rand() % candidate_veg_location.size();

		int* loc = new int[2];
		loc[0] = candidate_veg_location[i][0];
		loc[1] = candidate_veg_location[i][1];

		veg_location.push_back(loc);

		candidate_veg_location.erase(candidate_veg_location.begin() + i);
	}
}

void refine_veg(){

}

void vegetation() {

	//create a list of river tiles
	calculate_water_tiles();

	calculate_veg_candiates();
	populate_veg();



}

void print_vegetation(FILE* stream) {
	if (stream == 0) {
		stream = fopen(DEAFULT_VEGETATION_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {

		fprintf(stream, "<vegetation>\n");
		for (int i = 0; i < veg_location.size(); ++i) {
			int* loc = veg_location[i];
			fprintf(stream, "<veg x = '%d' y = '%d'>"
					"</veg>\n", loc[0], loc[1]);
		}
		fprintf(stream, "</vegetation>\n");
	}
}
