/*
 * midpoint_displacement.cpp
 *
 * This module creates a height map matrix using a midpoint displacement (diamond square) algorithm.
 * It also supplies functionality to make alteration to give the generated terrain a more realistic appearance.
 *
 *  Created on: 29 Jan 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"
#include <limits.h>

int tmap_size = DEFAULT_SIZE;
int crop_height = 0;
int crop_width = 0;
int** tmap;

int seed = 10;
int random_offset = 40;
float offset_dr = .8; //offset decrease ratio

//set of voronoi points
int voronoi_size = (int)(DEFAULT_VORONOI_SIZE*(tmap_size/512));
int** voronoi_points;

float voronoi_alpha = 0.33;

//erosion
int thermal_talus = random_offset / 40; //4/tmap_size;
float thermal_shift = 0.5;

int erosion_steps = 5;

bool neg = false;


bool normalise = false;
int normalise_min = 0;
int normalise_max = 150;

int sea_level = DEFAULT_SEA_LEVEL;
int sand_level = DEAFULT_SAND_LEVEL;
int snowtop_level = DEFAULT_SNOW_TOP_LEVEL;
int cliff_difference = 100;

//constraint helper methods
bool point_above_sealevel(int x, int y) {
	return tmap[y][x] > sea_level;
}

bool point_above_sandlevel(int x, int y) {
	return tmap[y][x] > sand_level;
}

bool point_below_snow_top_level(int x, int y) {
	return tmap[y][x] < snowtop_level;
}

int get_val(int x, int y) {
	return ((int) rand() % random_offset * 2) - (int) rand() % random_offset;
}

int get_sqr_avg(int x, int y, int l) {
	return (int) (tmap[x - l][y - l] + tmap[x + l][y - l] + tmap[x - l][y + l]
			+ tmap[x + l][y + l]) / 4 + get_val(x, y);
}

int get_dia_avg(int x, int y, int l) {
	int sum = 0;
	int n = 0;
	if (x - l >= 0) {
		sum += tmap[x - l][y];
		n++;
	}
	if (y - l >= 0) {
		sum += tmap[x][y - l];
		n++;
	}
	if (y + l <= tmap_size - 1) {
		sum += tmap[x][y + l];
		n++;
	}
	if (x + l <= tmap_size - 1) {
		sum += tmap[x + l][y];
		n++;
	}
	return (int) (sum / n + get_val(x, y));
}

int square_diamond() {
	int l = 0; //low
	int h = tmap_size - 1; //high
	int m = (int) (h / 2); //mid

	//seed values
	tmap[l][l] += seed;
	tmap[h][l] += seed;
	tmap[l][h] += seed;
	tmap[h][h] += seed;

	int i, j;
	while (h > 1) {
		m = int(h / 2);
		i = m;

		//square values
		while (i < tmap_size - 1) {
			j = m;
			while (j < tmap_size - 1) {
				tmap[i][j] = get_sqr_avg(i, j, m);
				j += (m * 2);
			}
			i += (m * 2);
		}

		//diamond values
		bool odd = false;
		i = 0;
		while (i <= tmap_size - 1) {
			if (!odd)
				j = m;
			else
				j = 0;
			while (j <= tmap_size - 1) {
				tmap[i][j] = get_dia_avg(i, j, m);
				j += (m * 2);
			}
			i += m;
			if (odd)
				odd = false;
			else
				odd = true;
		}
		random_offset *= offset_dr;
		if (random_offset < 1)
			random_offset = 1;

		if (h == 2) {
			h = 0;
		} else
			h /= 2;

	}

	return 0;

}

void setup_voronoi_points() {

	voronoi_points = new int*[voronoi_size];
	for (int i = 0; i < voronoi_size; i++) {
		voronoi_points[i] = new int[2];
		voronoi_points[i][0] = (int) rand() % tmap_size; //random x coord
		voronoi_points[i][1] = (int) rand() % tmap_size; //random y coord
	}

}

void interpolate_voronoi() {

	// using h = -d1 + d2 for voronoi value
	// where d1 is the nearest point to the current coordinate
	// d2 is the second nearest

	int min_d1 = tmap_size + 1;
	int min_d2 = tmap_size + 1;
	for (int i = 0; i < tmap_size; ++i) {
		for (int j = 0; j < tmap_size; ++j) {

			for (int k = 0; k < voronoi_size; ++k) {

				int d = sqrt(
						pow((voronoi_points[k][0] - j), 2)
								+ pow((voronoi_points[k][1] - i), 2));

				if (d < min_d2)
					min_d2 = d;
				if (d < min_d1) {
					min_d2 = min_d1;
					min_d1 = d;
				}
			}

			//line defines the shape of the voronoi diagram
			int val = min_d2 - min_d1;

			tmap[i][j] = (int) ((1.0 - voronoi_alpha) * (float) tmap[i][j])
					+ (int) (voronoi_alpha * (float) (val));

			min_d1 = tmap_size + 1;
			min_d2 = tmap_size + 1;

		}
	}

}

void voronoi() {

	//create points in a voronoi set
	setup_voronoi_points();

	//interpolate values w.t.r to the points in the set
	interpolate_voronoi();

}

void thermal() {

	int n_count = 4;
	int** neighbours = new int*[n_count];
	for (int i = 0; i < n_count; i++) {
		neighbours[i] = new int[2];
	}

	//looping ensures to always have 4 neighbours
	for (int i = 1; i < tmap_size - 1; ++i) {
		for (int j = 1; j < tmap_size - 1; ++j) {

			//north
			neighbours[0][0] = i - 1;
			neighbours[0][1] = j;

			//east
			neighbours[1][0] = i;
			neighbours[1][1] = j + 1;

			//south
			neighbours[2][0] = i + 1;
			neighbours[2][1] = j;

			//west
			neighbours[3][0] = i;
			neighbours[3][1] = j - 1;

			int d_totoal = 0;
			int d_max = tmap[i][j] - tmap[neighbours[0][0]][neighbours[0][1]];

			int min_n = 0;

			bool move = true;
			//find the shortest neighbour
			for (int k = 0; k < n_count; ++k) {

				int d = tmap[i][j] - tmap[neighbours[k][0]][neighbours[k][1]];
				if (tmap[neighbours[min_n][0]][neighbours[min_n][1]]
						> tmap[neighbours[k][0]][neighbours[k][1]])
					min_n = k;

				if (d > thermal_talus) {
					move = true;
					d_totoal += d;
					if (d > d_max)
						d_max = d;
				}
			}

			if (move) {
				//add a fraction of the height to the shortest neighbour
				int min_h = tmap[neighbours[min_n][0]][neighbours[min_n][1]];
				int change = (int) (thermal_shift
						* ((tmap[i][j] - min_h) - thermal_talus)); //* ((tmap[i][j] - min_h)/d_totoal));
				int new_h = min_h + change;
				tmap[neighbours[min_n][0]][neighbours[min_n][1]] = new_h;

				//remove from centre
				tmap[i][j] -= change;
			}

		}
	}

}

void erosion() {

	for (int i = 0; i < erosion_steps; ++i)
		thermal();

}

//replace all negative values with 0
void clear_neg() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if (tmap[i][j] < 0)
				tmap[i][j] = 0;
		}
	}

}

//recalculate heights to scale the between specified min/max values
void normalise_map(){

	int max = -INT_MAX;
	int min = INT_MAX;
	for (int i = 0; i < crop_height; ++i) {
				for (int j = 0; j < crop_width; ++j) {
					if(max<tmap[i][j]) max = tmap[i][j];
					if(min>tmap[i][j]) min = tmap[i][j];
				}
	}

	for (int i = 0; i < crop_height; ++i) {
			for (int j = 0; j < crop_width; ++j) {
					int diff = normalise_max - normalise_min;
					tmap[i][j] = (int)((float)tmap[i][j]/(float)(max-min) * (float)diff);
			}
	}

}

//standard print
void print_map(FILE* stream) {
	if (stream == 0) {
		stream = fopen(DEAFULT_RIVERS_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {

		fprintf(stream, "%i %i ", crop_width, crop_height);
		for (int i = 0; i < crop_height; ++i) {
			for (int j = 0; j < crop_width; ++j) {
				fprintf(stream, "%i ", tmap[i][j]);
			}
		}
		fprintf(stream, "\n");

	}
}



//xml print
void print_map_xml(FILE* stream) {
	if (stream == 0) {
		stream = fopen(DEAFULT_RIVERS_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {
		//TODO: add tile type value
		fprintf(stream,"<map width='%d' height='%d'>\n", crop_width, crop_height);
		for (int i = 0; i < crop_height; ++i) {
			for (int j = 0; j < crop_width; ++j) {

				std::string type;

				//resolve tile type
				//plain type
				if(tmap[i][j]<=sea_level){
					type = "water";
				}else if(tmap[i][j]<=sand_level){
					type = "sand";
				}else if(tmap[i][j]<=sand_level){
					type = "sand";
				}else if(tmap[i][j]<=snowtop_level){
					type = "grass";
				}else
					type = "snow";

				//height difference type for cliffs
				//North-South difference
				bool is_cliff = false;
				if(i-1>=0 && i+1<crop_height)
					if(abs(tmap[i-1][j]-tmap[i+1][j]) >= cliff_difference){
						type="cliff";
						is_cliff = true;
					}
				//East-West difference
				if(!is_cliff){
					if(j-1>=0 && j+1<crop_width)
						if(abs(tmap[i][j-1]-tmap[i][j+1]) >= cliff_difference){
							type="cliff";
						}
				}




				fprintf(stream,"<tile x='%d' y='%d'>\n\t<height>%i</height>\n"
						"<type>%s</type>"
						"</tile>\n", i, j, tmap[i][j], type.c_str());
			}
			fprintf(stream,"\n");
		}
		fprintf(stream,"</map>\n");
	}
}
