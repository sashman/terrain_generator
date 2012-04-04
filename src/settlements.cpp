/*
 * settlements.cpp
 *
 *  Created on: 10 Feb 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"
#include <string>
#include <vector>
#include <locale>

int n_settlements = DEFAULT_NO_OF_SETTLEMENTS;
int min_distance = DEFAULT_MIN_DISTANCE_BETWEEN_SETTLEMENTS;

extern int crop_height;
extern int crop_width;
extern int** tmap;

extern int sea_level;

bool* settlement_found;
int** locations;
std::vector<int*> settlement_location_candidates;
std::string* names;


/*
 * attempts to find a location above sea level
 * if so sets as the location and returns true
 * else false
 */
bool get_above_sea_level(int location_index) {

	//create a settlement above sea level at a random location
	int x = rand() % crop_width;
	int y = rand() % crop_height;


	while (tmap[y][x] <= sea_level) {
		x = rand() % crop_width;
		y = rand() % crop_height;
	}

	locations[location_index][0] = x;
	locations[location_index][1] = y;

	return true;
}



bool point_at_min_distance(int x, int y, int index) {

	for (int i = 0; i < index; ++i) {
		int d = sqrt(
				abs((pow(x, 2) - pow(locations[i][0], 2)))
						+ abs((pow(y, 2) - pow(locations[i][1], 2))));

		if (d < min_distance)
			return false;
	}
	return true;
}

/*
 * create settlement wrt given constraints
 *
 */
bool get_non_candidate_settelement(int location_index) {

	//start with random
	int x = rand() % crop_width;
	int y = rand() % crop_height;

	while (!point_above_sandlevel(x, y) || !point_below_snow_top_level(x, y)
			|| !point_at_min_distance(x, y, location_index)) {
		x = rand() % crop_width;
		y = rand() % crop_height;
	}

	locations[location_index][0] = x;
	locations[location_index][1] = y;

	return true;

}

bool get_settelement(int location_index) {

	if (settlement_location_candidates.empty())
		return false;

	int i = rand() % settlement_location_candidates.size();

	int* loc = new int[2];
	loc[0] = settlement_location_candidates[i][0];
	loc[1] = settlement_location_candidates[i][1];
	settlement_location_candidates.erase(
			settlement_location_candidates.begin() + i);

	while (!point_at_min_distance(loc[0], loc[1], location_index)) {
		if (settlement_location_candidates.empty())
			return false;

		i = rand() % settlement_location_candidates.size();
		loc[0] = settlement_location_candidates[i][0];
		loc[1] = settlement_location_candidates[i][1];
		settlement_location_candidates.erase(
				settlement_location_candidates.begin() + i);
	}

	locations[location_index][0] = loc[0];
	locations[location_index][1] = loc[1];
	return true;

}

void get_name(std::string *name) {

	FILE* fprefix = fopen("location_prefix", "r");
	FILE* fsuffix = fopen("location_suffix", "r");

	if (fprefix == NULL || fsuffix == NULL)
		perror("Error opening file");
	else {

		int fprefix_count = 0;
		fscanf(fprefix, "%d\n", &fprefix_count);

		int fsuffix_count = 0;
		fscanf(fsuffix, "%d\n", &fsuffix_count);

		int prefix_i = rand() % fprefix_count + 1;
		int suffix_i = rand() % fsuffix_count + 1;

		char prefix[128];
		char suffix[128];

		for (int i = 0; i < prefix_i; ++i)
			fscanf(fprefix, "%s\n", &prefix);
		for (int i = 0; i < suffix_i; ++i)
			fscanf(fsuffix, "%s\n", &suffix);

		*name += prefix;
		*name += suffix;

		//*name-> = toupper(*name->at(0));

	}
	fclose(fprefix);
	fclose(fsuffix);

}

bool settlement_candidate_constraint(int x, int y){

	return (point_above_sealevel(x,y) && point_below_snow_top_level(x, y));

}

void calculate_settlement_candidates() {
	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {

			if (settlement_candidate_constraint(j, i)) {

				int *point = new int[2];
				point[0] = j;
				point[1] = i;
				settlement_location_candidates.push_back(point);

			}

		}
	}
}

void settlements() {

	settlement_found = new bool[n_settlements];
	locations = new int*[n_settlements];
	names = new std::string[n_settlements];
	calculate_settlement_candidates();

	for (int i = 0; i < n_settlements; ++i) {
		locations[i] = new int[2];
		names[i] = "";
		settlement_found[i] = false;

		//use a simple rule to find a suitable place for a settlement
		if(get_settelement(i)){
			get_name(&names[i]);
			settlement_found[i] = true;
		}
	}
}



void print_settlements(FILE* stream) {
	if (stream == 0) {
		stream = fopen(DEAFULT_SETTLEMENTS_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {

		fprintf(stream, "<settlements>\n");
		for (int i = 0; i < n_settlements; ++i) {
			if (settlement_found[i]) {

				fprintf(stream, "<settlement x='%d' y='%d'>\n", locations[i][0],
						locations[i][1]);

				char* name = (char*) names[i].c_str();
				name[0] = toupper(name[0]);

				fprintf(stream, "<name>%s</name>\n", name);
				fprintf(stream, "</settlement>\n");
			}
		}
		fprintf(stream, "</settlements>\n");

	}
}

