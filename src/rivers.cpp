/*
 * rivers.cpp
 *
 *
 *	Used to generate fresh water sources on the map
 *  Created on: 10 Feb 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"
#include <vector>

extern int crop_height;
extern int crop_width;
extern int** tmap;

extern int sea_level;
extern int snowtop_level;

int n_rivers = DEFAULT_NO_OF_RIVERS;

std::vector<int*> candidate_souce_location;
int** source_location;
int* n_river_branches;
int max_branches = DEFAULT_MAX_RIVER_BRANCHES;

RiverPoint** source_river_points;

bool get_river_source(int river_index) {

	//start with random
	int x = rand() % crop_width;
	int y = rand() % crop_height;

	while (point_below_snow_top_level(x, y)

	) {
		x = rand() % crop_width;
		y = rand() % crop_height;
	}

	source_location[river_index][0] = x;
	source_location[river_index][1] = y;

}

bool get_river_source_from_candidates(int river_index) {

	if (candidate_souce_location.empty())
		return false;

	int i = rand() % candidate_souce_location.size();

	source_location[river_index][0] = candidate_souce_location[i][0];
	source_location[river_index][1] = candidate_souce_location[i][1];

	candidate_souce_location.erase(candidate_souce_location.begin() + i);

	//std::cout<< "Source point " << source_location[river_index][0] << "," << source_location[river_index][1] <<std::endl;
	return true;
}

int get_branch_id(RiverPoint* rp, int x, int y) {

	do {
//		std::cout << "Branch " << rp->x << "," << rp->y << " ~ " << x << ","
//				<< y << std::endl;
		if (rp != 0 && rp->x == x && rp->y == y)
			return rp->river_id;

		rp = rp->next;
	} while (rp != 0);

	return -1;
}

int get_tile_river_id(int x, int y) {

	for (int i = 0; i < n_rivers; ++i) {
		RiverPoint *rp = source_river_points[i];
		do {
			if (rp != 0 && rp->x == x && rp->y == y)
				return i;
			//check branches
			if (rp->branch != 0) {
				//std::cout << "Checking branch on river " << rp->river_id << " " << rp->x << "," << rp->y << std::endl;
				int r = get_branch_id(rp->branch, x, y);
				if (r > -1) {

					return r;
				}
			}

			rp = rp->next;
		} while (rp != 0);
	}
	return -1;
}


int branch_chance = 2; //1 in branch_chance chance to branch on available equal heights
void grow(RiverPoint *rp, bool branch) {

	if(rp==NULL) return;

	//if at sea level stop
	if (!point_above_sealevel(rp->x, rp->y))
		return;

	//find smallest neighbour
	int true_min = tmap[rp->x][rp->y];
	int true_min_id = -1;

	//used in the case of local minimum#
	//TODO: needs inf value
	int min = 999;
	int min_id = 0;

	int branch_id = -1;

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

	//check if ALL neighbours are part of the same river
	int surrounded_count = 0;

	for (int i = 0; i < n_count; ++i) {
		//check bounds
		if (neighbours[i][0] < 0 || neighbours[i][0] >= crop_width)
			continue;
		if (neighbours[i][1] < 0 || neighbours[i][1] >= crop_height)
			continue;

		//check if joining river

		int tile_id = get_tile_river_id(neighbours[i][0], neighbours[i][1]);
//		std::cout << "Tile id " << tile_id << std::endl;

		if (tile_id == rp->river_id) {
			if(branch) return;
			surrounded_count++;
			continue;
		}
		if (tile_id > -1)
			return;

		if (tmap[neighbours[i][0]][neighbours[i][1]] < true_min) {
			true_min_id = i;
			true_min = tmap[neighbours[i][0]][neighbours[i][1]];
		}

		if (tmap[neighbours[i][0]][neighbours[i][1]] < min) {
			min_id = i;
			min = tmap[neighbours[i][0]][neighbours[i][1]];
		}
	}

	if (branch && surrounded_count == n_count - 1) {
		//the river/branch has locked itself in
		//std::cout << "Surrounded!" << std::endl;
		return;
	}

	//check for equal height to branch to
	if (true_min_id > -1) {
		for (int i = 0; i < n_count; ++i) {
			int y = neighbours[i][0];
			int x = neighbours[i][1];
			if (tmap[y][x] == true_min
					&& true_min != i) {
				branch_id = i;
			}
		}
	}

	if (true_min_id > -1) { //no local minimum

		if (neighbours[true_min_id][0] == 0
				|| neighbours[true_min_id][0] == crop_width)
			return;
		if (neighbours[true_min_id][1] == 0
				|| neighbours[true_min_id][1] == crop_height)
			return;

		//add new RiverPoint
		RiverPoint* new_rp = new RiverPoint(neighbours[true_min_id][0],
				neighbours[true_min_id][1], rp->river_id);
		rp->next = new_rp;

		grow(new_rp, false);

		int height_above_sealevel = (int)(tmap[rp->y][rp->x]- sea_level);


		if (rand() % (branch_chance + abs(height_above_sealevel - 10)) == 0 &&
				branch_id > -1 &&
				n_river_branches[rp->river_id] > 0) {

//			std::cout << "Branching from " << rp->x << "," << rp->y
//					<< std::endl;
			RiverPoint* branch_source = new RiverPoint(neighbours[branch_id][0],
					neighbours[branch_id][1], rp->river_id);

			rp->branch = branch_source;
//			std::cout << "Growing branch on river " << rp->river_id
//					<< std::endl;
			grow(branch_source, true);
//			std::cout << "Branch done" << std::endl;
			n_river_branches[rp->river_id]--;

		}

	} else {

		if (neighbours[min_id][0] == 0 || neighbours[min_id][0] == crop_width)
			return;
		if (neighbours[min_id][1] == 0 || neighbours[min_id][1] == crop_height)
			return;

		RiverPoint* new_rp = new RiverPoint(neighbours[min_id][0],
				neighbours[min_id][1], rp->river_id);
		rp->next = new_rp;

		grow(new_rp, false);

	}

}

bool river_source_candidate_constraint(int x, int y) {

	return !point_below_snow_top_level(x, y);
}

void calculate_source_candiates() {

	for (int i = 0; i < crop_height; ++i) {

		for (int j = 0; j < crop_width; ++j) {

			if (river_source_candidate_constraint(j, i)) {

				int *point = new int[2];
				point[0] = j;
				point[1] = i;
				candidate_souce_location.push_back(point);

			}

		}
	}

}

int source_cutoff = 100;
int source_cutoff_count = 0;
void rivers() {

	calculate_source_candiates();

	source_location = new int*[n_rivers];
	n_river_branches = new int[n_rivers];
	source_river_points = new RiverPoint*[n_rivers];


	for (int i = 0; i < n_rivers; ++i) {

		source_location[i] = new int[2];
		source_location[i][0] = -1;
		source_location[i][1] = -1;
		source_river_points[i] = NULL;

		//create source

		if (get_river_source_from_candidates(i)){
			source_river_points[i] = new RiverPoint(source_location[i][0],source_location[i][1], i);
			n_river_branches[i] = max_branches;
		}

	}

	for (int j = 0; j < 1; ++j) {

		for (int i = 0; i < n_rivers; ++i)
			//grow river
			grow(source_river_points[i], false);

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
			int x = source_location[i][0];
			int y = source_location[i][1];

			if(x==-1 || y==-1) continue;

			fprintf(
					stream,
					"<river id = '%d' source_x='%d' source_y='%d' alt = '%d'>\n",
					i, x, y,
					tmap[y][x]);

			RiverPoint* rp = source_river_points[i];
			do {

				if (rp != 0) {
					fprintf(stream,
							"<river_point x = '%d' y = '%d' is_branch = 'false'></river_point>\n",
							rp->x, rp->y);

					if (rp->branch != 0) {
						RiverPoint* rpb = rp->branch;
						do {
							if (rpb != 0) {
								fprintf(
										stream,
										"<river_point x = '%d' y = '%d' is_branch = 'true'></river_point>\n",
										rpb->x, rpb->y);

								rpb = rpb->next;
							}
						} while (rpb != 0);

					}
					rp = rp->next;
				}


			} while (rp != 0);

			fprintf(stream, "</river>\n");
		}
		fprintf(stream, "</rivers>\n");

	}
}
