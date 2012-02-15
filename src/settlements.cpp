/*
 * settlements.cpp
 *
 *  Created on: 10 Feb 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"
#include <string>
#include <locale>

int n_settlements = DEFAULT_NO_OF_SETTLEMENTS;

extern int crop_height;
extern int crop_width;
extern int** tmap;

int** locations;
std::string* names;

/*
 * attempts to find a location above sea level
 * if so sets as the location and returns true
 * else false
 */
bool above_sea_level(int location_index) {

	//create a settlement above sea level at a random location
	int x = rand() % crop_width;
	int y = rand() % crop_height;

	//TODO: change to a set sea level!
	int sea_level = 0.588 * 126;


	while (tmap[x][y] <= sea_level) {
		x = rand() % crop_width;
		y = rand() % crop_height;
	}

	locations[location_index][0] = x;
	locations[location_index][1] = y;

	return true;

}

void get_name(std::string *name){

	FILE* fprefix = fopen("location_prefix", "r");
	FILE* fsuffix = fopen("location_suffix", "r");

	if (fprefix == NULL || fsuffix == NULL)
		perror("Error opening file");
	else {

		int fprefix_count = 0;
		fscanf(fprefix,"%d\n", &fprefix_count);

		int fsuffix_count = 0;
		fscanf(fsuffix,"%d\n", &fsuffix_count);

		int prefix_i = rand()%fprefix_count+1;
		int suffix_i = rand()%fsuffix_count+1;

		char prefix[128];
		char suffix[128];

		for (int i = 0; i < prefix_i; ++i) fscanf(fprefix, "%s\n", &prefix);
		for (int i = 0; i < suffix_i; ++i) fscanf(fsuffix, "%s\n", &suffix);

		*name += prefix;
		*name += suffix;

		//*name-> = toupper(*name->at(0));

	}
	fclose(fprefix);
	fclose(fsuffix);


}

void settlements() {

	locations = new int*[n_settlements];
	names = new std::string[n_settlements];

	for (int i = 0; i < n_settlements; ++i) {
		locations[i] = new int[2];
		names[i] = "";
		//names[i] = new std::string;

		//use a simple rule to find a suitable place for a settlement
		bool found = above_sea_level(i);

		//TODO: change to loop if not found
		if (found)
			get_name(&names[i]);

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
			fprintf(stream, "<settlement x='%d' y='%d'>\n", locations[i][0],
					locations[i][1]);

			char* name = (char*)names[i].c_str();
			name[0] = toupper(name[0]);

			fprintf(stream, "<name>%s</name>\n", name);
			fprintf(stream, "</settlement>\n");
		}
		fprintf(stream, "<settlements>\n");


	}
}

