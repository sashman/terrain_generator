/*
 * contour_format2.cpp
 *
 *  Created on: 8 Jul 2012
 *      Author: sashman
 */


#include "terrain_generator.hpp"
#include <vector>;

extern int crop_height;
extern int crop_width;
extern int** tmap;

int max = 0;
int threshhold_increment = 20;
int threshold = 0;

extern int sea_level;

int** cmap;

std::vector<int>** domain;

//TODO: change using tile types
enum TILE_CASE {

	HIGH_GRASS,

	//convex cliff corners
	CLIFF_NE_SN, //north -> east turn, south->north increasing incline
	/*
	 * 	lower	| higher
	 * 	lower	\______
	 * 		lower   lower
	 */
	CLIFF_NW_SN, //north -> west turn, south->north increasing incline
	CLIFF_SE_NS, //south -> east turn, north->south incline
	CLIFF_SW_NS,
	//concave cliff corners
	CLIFF_SE_SN, //south -> east corner, south->north incline
	/*
	 *       higher
	 * higher 	 ____
	 * 			/
	 * higher 	| lower
	 */
	CLIFF_SW_SN,
	CLIFF_NE_NS,
	CLIFF_NW_NS,

	//cliff straights
	CLIFF_NS_EW, //along north->south, east->west increasing incline
	CLIFF_NS_WE,
	CLIFF_WE_NS,
	CLIFF_WE_SN,

	//other
	GRASS,
	WATER

	//count
	,TILE_COUNT
};




void round_tmap() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {

			//nearest 10
			//tmap[i][j] = (int) (tmap[i][j] / 10 * 10);

			tmap[i][j] = (int) (tmap[i][j] / 5 * 5);

		}
	}

}

void reset_grass() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {

			if(cmap[i][j] == HIGH_GRASS) cmap[i][j] = GRASS;

		}
	}

}





void contour_map() {

	round_tmap();

	max = tmap[0][0];
	//set up contour map array
	cmap = new int*[crop_height];
	for (int i = 0; i < crop_height; ++i) {
		cmap[i] = new int[crop_width];
		for (int j = 0; j < crop_width; ++j) {
			cmap[i][j] = -1;
		}
	}

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if (tmap[i][j] > max)
				max = tmap[i][j];
		}
	}

	std::cout << "\n" << std::endl;
	for (threshold = sea_level; threshold < max; threshold +=
			threshhold_increment) {
		std::cout << "T= " << threshold << std::endl;
		//threshold = 65;
		set_contour_values();
		reset_grass();
	}

	correct_contours();
}

void print_contour(FILE* stream) {

	if (stream == 0) {
		stream = fopen(DEFAULT_CONTOUR_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {

		fprintf(stream, "%i %i %i\n\n", crop_width, crop_height, max);

		fprintf(stream, "  ");
		for (int i = 0; i < crop_height; ++i)
			fprintf(stream, "%.2i", i);
		fprintf(stream, "\n");
		for (int i = 0; i < crop_height; ++i) {
			fprintf(stream, "%02i ", i);
			for (int j = 0; j < crop_width; ++j) {

				int t = cmap[i][j];

				if (t == GRASS || t == HIGH_GRASS)
					fprintf(stream, ". ");
				else if (t == CLIFF_NS_EW || t == CLIFF_NS_WE)
					fprintf(stream, "| ");
				else if (t == CLIFF_WE_NS || t == CLIFF_WE_SN)
					fprintf(stream, "- ");
				else if (t == WATER)
					fprintf(stream, "~ ");
				else if (t == CLIFF_NE_NS || t == CLIFF_NE_SN
						|| t == CLIFF_SW_NS || t == CLIFF_SW_SN)
					fprintf(stream, "\\ ");
				else if (t == CLIFF_SE_NS || t == CLIFF_SE_SN
						|| t == CLIFF_NW_NS || t == CLIFF_NW_SN)
					fprintf(stream, "/ ");
				else
					fprintf(stream, "%i ", t);
			}
			fprintf(stream, "\n");
		}

		fprintf(stream, "\n");
		fprintf(stream, "   ");
		for (int i = 0; i < crop_height; ++i)
			fprintf(stream, "%03i ", i);
		fprintf(stream, "\n");
		for (int i = 0; i < crop_height; ++i) {
			fprintf(stream, "%02i ", i);
			for (int j = 0; j < crop_width; ++j) {

				int t = tmap[i][j];
				fprintf(stream, "%03i ", t);
			}
			fprintf(stream, "\n");
		}

	}
}

void print_kf(FILE* stream) {

	if (stream == 0) {
		stream = fopen(DEFAULT_CONTOUR_KF_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else {
		std::string header =
				"<map x=\"0\" y=\"0\" type = \"grass\" colour = \"#008800\" >"
						"<tile tag = \"a\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"b\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"c\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"d\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"e\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"f\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"g\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"h\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"i\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"j\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"k\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"l\">"
						"<pass>false</pass>"
						"</tile>"

						"<tile tag = \"m\">"
						"<pass>false</pass>"
						"</tile>"

				//grass

						"<tile tag = \"n\">"
						"<pass>true</pass>"
						"</tile>"
						"<tile tag = \"o\">"
						"<pass>true</pass>"
						"</tile>"
						"<tile tag = \"p\">"
						"<pass>true</pass>"
						"</tile>"
						"\n\n";

		fprintf(stream, "%s", header.c_str());
		fprintf(stream, "<content>\n");
		for (int i = 0; i < crop_height; ++i) {
			fprintf(stream, "<r>");
			for (int j = 0; j < crop_width; ++j) {

				int t = cmap[i][j];

				if (t == GRASS) {
					char t = 110 + rand() % 3;

					fprintf(stream, "%c", t);
				}

				//straights
				else if (t == CLIFF_NS_EW)
					fprintf(stream, "e");
				else if (t == CLIFF_NS_WE)
					fprintf(stream, "f");
				else if (t == CLIFF_WE_NS)
					fprintf(stream, "j");
				else if (t == CLIFF_WE_SN)
					fprintf(stream, "k");
				else if (t == WATER)
					fprintf(stream, "p");

				//corners
				else if (t == CLIFF_NE_NS)
					fprintf(stream, "a");
				else if (t == CLIFF_NE_SN)
					fprintf(stream, "b");
				else if (t == CLIFF_SW_NS)
					fprintf(stream, "i");
				else if (t == CLIFF_SW_SN)
					fprintf(stream, "m");
				else if (t == CLIFF_SE_NS)
					fprintf(stream, "d");
				else if (t == CLIFF_SE_SN)
					fprintf(stream, "h");
				else if (t == CLIFF_NW_NS)
					fprintf(stream, "c");
				else if (t == CLIFF_NW_SN)
					fprintf(stream, "g");
				//else
				//fprintf(stream, " ");

			}
			fprintf(stream, "</r>\n");
		}
		fprintf(stream, "</content></map>");
	}
}

