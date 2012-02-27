/*
 * contour_format.cpp
 *
 *  Created on: 21 Feb 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"

extern int crop_height;
extern int crop_width;
extern int** tmap;

int max = 0;
int threshhold_increment = 15;
int threshold = 0;

extern int sea_level;

int** cmap;

//TODO: change using tile types
enum TILE_CASE {

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
};

int get_case(int a, int b, int c, int d) {
	int n = 0;
	if (a > threshold)
		n += 1;
	if (b > threshold)
		n += 8;
	if (c > threshold)
		n += 4;
	if (d > threshold)
		n += 2;
	return n;
}

bool check_priority(int c, int v) {

	if (c < 0)
		return true;

	if (c > v)
		return true;

	return false;
}

void set_cmap(int i, int j, TILE_CASE a, TILE_CASE b, TILE_CASE c,
		TILE_CASE d) {

	//if ((cmap[i][j] == GRASS || cmap[i][j] < 0))
	if (check_priority(cmap[i][j], a))
		cmap[i][j] = a;
	if (check_priority(cmap[i + 1][j], b))
		cmap[i + 1][j] = b;
	if (check_priority(cmap[i + 1][j + 1], c))
		cmap[i + 1][j + 1] = c;
	if (check_priority(cmap[i][j + 1], d))
		cmap[i][j + 1] = d;

	/*
	 if (tmap[i][j] < sea_level)
	 cmap[i][j] = WATER;
	 if (tmap[i + 1][j] < sea_level)
	 cmap[i + 1][j] = WATER;
	 if (tmap[i][j + 1] < sea_level)
	 cmap[i][j + 1] = WATER;
	 if (tmap[i + 1][j + 1] < sea_level)
	 cmap[i + 1][j + 1] = WATER;
	 */

}

void set_contour_values() {
	//run through map checking contour cases
	for (int i = 0; i < crop_height - 1; i += 1) {
		for (int j = 0; j < crop_width - 1; j += 1) {

			int cell_case = get_case(tmap[i][j], tmap[i + 1][j],
					tmap[i + 1][j + 1], tmap[i][j + 1]);
			switch (cell_case) {
			case 0:
				set_cmap(i, j, GRASS, GRASS, GRASS, GRASS);

				break;
			case 1:
				set_cmap(i, j, GRASS, CLIFF_WE_SN, CLIFF_NW_SN, CLIFF_NS_EW);
				break;

			case 2:
				set_cmap(i, j, CLIFF_NS_WE, CLIFF_NE_SN, CLIFF_WE_SN, GRASS);
				break;

			case 3:
				set_cmap(i, j, GRASS, CLIFF_WE_SN, CLIFF_WE_SN, GRASS);

				break;

			case 4:
				set_cmap(i, j, CLIFF_SE_NS, CLIFF_WE_SN, CLIFF_WE_SN, GRASS);
				break;

			case 5:
				set_cmap(i, j, GRASS, CLIFF_SW_NS, GRASS, CLIFF_NE_SN);
				break;

			case 6:
				set_cmap(i, j, CLIFF_NS_WE, CLIFF_NS_WE, GRASS, GRASS);

				break;

			case 7:
				set_cmap(i, j, GRASS, CLIFF_SW_SN, GRASS, GRASS);

				break;

			case 8:
				set_cmap(i, j, CLIFF_WE_NS, GRASS, CLIFF_NS_EW, CLIFF_SW_NS);

				break;

			case 9:
				set_cmap(i, j, GRASS, GRASS, CLIFF_NS_EW, CLIFF_SW_NS);

				break;

			case 10:
				set_cmap(i, j, CLIFF_NW_SN, GRASS, CLIFF_SE_NS, GRASS);

				break;

			case 11:
				set_cmap(i, j, GRASS, GRASS, CLIFF_SE_SN, GRASS);

				break;

			case 12:
				set_cmap(i, j, CLIFF_WE_NS, GRASS, GRASS, CLIFF_WE_NS);

				break;

			case 13:
				set_cmap(i, j, GRASS, GRASS, GRASS, CLIFF_NE_NS);

				break;

			case 14:
				set_cmap(i, j, CLIFF_NW_NS, GRASS, GRASS, GRASS);

				break;

			case 15:
				set_cmap(i, j, GRASS, GRASS, GRASS, GRASS);

				break;

			default:
				break;
			}
		}
	}
}

void round_tmap() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			tmap[i][j] = (int) (tmap[i][j] / 10 * 10);

		}
	}

}

void contour_map() {

	//round_tmap();

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

	for (threshold = sea_level; threshold < max; threshold +=
			threshhold_increment) {
		//threshold = 65;
		set_contour_values();
	}
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

				if (t == GRASS)
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
			fprintf(stream, "%02i ", i);
		fprintf(stream, "\n");
		for (int i = 0; i < crop_height; ++i) {
			fprintf(stream, "%02i ", i);
			for (int j = 0; j < crop_width; ++j) {

				int t = tmap[i][j];
				fprintf(stream, "%02i ", t);
			}
			fprintf(stream, "\n");
		}

	}
}

void print_kf(FILE* stream) {

	if (stream == 0) {
		stream = fopen(DEFAULT_CONTOUR_FILE, "w");
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

				//grass
						"<tile tag = \"m\">"
						"<pass>true</pass>"
						"</tile>"
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

