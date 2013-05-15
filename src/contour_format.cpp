/*
 * contour_format2.cpp
 *
 *  Created on: 8 Jul 2012
 *      Author: sashman
 */

#include "terrain_generator.hpp"
#include <vector>;
#include <iostream>;
#include <sstream>;

extern int crop_height;
extern int crop_width;
extern int** tmap;

int sub_map_h = 0;
int sub_map_w = 0;

int max = 0;
int threshhold_increment = 40;
int threshold = 0;
int nearest_round = 5;

extern int sea_level;

int** cmap;

std::vector<int>** domain;

//TODO: change using tile types
enum TILE_CASE {

	HIGH_GRASS,

	//convex cliff corners

	CLIFF_NW_SN, //north -> west turn, south->north increasing incline
	CLIFF_NE_SN, //north -> east turn, south->north increasing incline
	/*
	 * 	lower	| higher
	 * 	lower	\______
	 * 		lower   lower
	 */
	CLIFF_SE_NS, //south -> east turn, north->south incline
	CLIFF_SW_NS,

	//concave cliff corners
	CLIFF_SE_SN, //south -> east corner, south->north incline
	/*
	 *       higher
	 * higher 	 ____
	 * 		/
	 * higher       | lower
	 */
	CLIFF_SW_SN,
	CLIFF_NW_NS,
	CLIFF_NE_NS,

	//cliff straights
	CLIFF_WE_SN,
	CLIFF_NS_WE,
	CLIFF_WE_NS,
	CLIFF_NS_EW, //along north->south, east->west increasing incline

	//other
	GRASS,
	WATER

	//count
	,
	TILE_COUNT
};

void round_tmap() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {

			//nearest 10
			//tmap[i][j] = (int) (tmap[i][j] / 10 * 10);

			tmap[i][j] = (int) (tmap[i][j] / nearest_round * nearest_round);

		}
	}

}

bool above_threshold(int height) {
	return height > threshold;

}

//-1 invalid
char get_neighbour_case(std::vector<int> *n_case, bool verbose) {
	if (n_case->size() != 8
//			&& n_case->size() != 5 &&
//			n_case->size() != 3
			) {
		return -1;
	}

	unsigned char case_id = 0;

	for (int i = n_case->size() - 1; i >= 0; --i) {

		case_id |= above_threshold(n_case->at(i)) ? 1 << i : 0;

	}

	if (case_id == 255 || case_id == 0)
		return 0;
	if (verbose)
		std::cout << (int) case_id << std::endl;

	if (verbose) {
		for (int i = 0; i < n_case->size(); ++i) {

			if (i == 4)
				std::cout << "  ";

			above_threshold(n_case->at(i)) ?
					std::cout << ". " : std::cout << "~ ";
			if (i == 2 || i == 4)
				std::cout << std::endl;
		}
	}

	if (verbose)
		std::cout << std::endl;

	return case_id;

}

void reset_grass() {

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {

			if (cmap[i][j] == HIGH_GRASS)
				cmap[i][j] = GRASS;

		}
	}

}

void fill_one_tile_gaps(int t) {

	//horizontal gaps
	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width - 3; ++j) {

			if (!above_threshold(tmap[i][j + 1]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i][j + 2])) {
				//if (tmap[i][j + 1] <= t && tmap[i][j] > t && tmap[i][j + 2] > t) {
				tmap[i][j + 1] = t + nearest_round;
			}
		}
	}

	//vertical gaps
	for (int i = 0; i < crop_height - 3; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if (!above_threshold(tmap[i + 1][j]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i + 2][j])) {
				tmap[i + 1][j] = t + nearest_round;
			}
		}
	}

	//diagonal gaps
	for (int i = 0; i < crop_height - 3; ++i) {
		for (int j = 0; j < crop_width - 3; ++j) {
			//2 rotational cases
			/*
			 *
			 * ^ v v
			 * v v v
			 * v v ^
			 *
			 * and
			 *
			 * v v ^
			 * v v v
			 * ^ v v
			 *
			 */
			if ((above_threshold(tmap[i][j])
					&& !above_threshold(tmap[i + 1][j + 1])
					&& above_threshold(tmap[i + 2][j + 2]))
					|| (above_threshold(tmap[i + 2][j])
							&& !above_threshold(tmap[i + 1][j + 1])
							&& above_threshold(tmap[i][j + 2]))) {
				tmap[i + 1][j + 1] = t + nearest_round;
			}
		}
	}

}

void verify_one_tile_gaps(int t, bool verbose) {
	//horizontal gaps
	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width - 3; ++j) {

			if (!above_threshold(tmap[i][j + 1]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i][j + 2])) {

				if (verbose) {
					std::cout << "===" << j << ", " << i
							<< " horizontal gap (threshold = " << t << ")"
							<< std::endl;
					std::cout << "===" << "\t-----" << std::endl;
					for (int k = (i) - 2; k < (i) + 3; k++) {
						std::cout << "===";
						for (int l = (j + 1) - 2; l < (j + 1) + 3; l++) {
							if ((k >= 0 && k < crop_height)
									&& (l >= 0 && l < crop_width)) {
								above_threshold(tmap[k][l]) ?
										std::cout << " ." : std::cout << " ~";
							}
						}
						std::cout << std::endl;
					}
					std::cout << "===" << "\t-----" << std::endl;
				}
			}

		}
	}
	//vertical gaps
	for (int i = 0; i < crop_height - 3; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if (!above_threshold(tmap[i + 1][j]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i + 2][j])) {
				if (verbose) {
					std::cout << "===" << j << ", " << i
							<< " vertical gap (threshold = " << t << ")"
							<< std::endl;
					std::cout << "===" << "\t-----" << std::endl;
					for (int k = (i + 1) - 2; k < (i + 1) + 3; k++) {
						std::cout << "===";
						for (int l = (j) - 2; l < (j) + 3; l++) {
							if ((k >= 0 && k < crop_height)
									&& (l >= 0 && l < crop_width)) {
								above_threshold(tmap[k][l]) ?
										std::cout << " ." : std::cout << " ~";
							}
						}
						std::cout << std::endl;
					}
					std::cout << "===" << "\t-----" << std::endl;
				}
			}
		}
	}
}

unsigned char rotate_case(std::vector<int> *n_case, bool verbose) {

	int t5 = (*n_case)[5];
	int t3 = (*n_case)[3];
	(*n_case)[5] = (*n_case)[0];
	(*n_case)[3] = (*n_case)[1];
	(*n_case)[0] = (*n_case)[2];
	(*n_case)[1] = (*n_case)[4];
	(*n_case)[2] = (*n_case)[7];
	(*n_case)[4] = (*n_case)[6];
	(*n_case)[7] = t5;			//(*n_case)[5];
	(*n_case)[6] = t3;

	return get_neighbour_case(n_case, verbose);
}

int undo_rotation(int id, int r, bool verbose) {
	if (r > 3) {
		if (verbose)
			std::cout << "***BAD ROTATE r = " << r << std::endl;
		return -1;
	}

	if (id == 1)
		return CLIFF_NW_SN + r;
	else if (id == 2 || id == 3 || id == 6 || id == 7)
		return CLIFF_WE_SN + r;
	else if (id == 10 || id == 11 || id == 15 || id == 43 || id == 47)
		return CLIFF_SE_SN + r;
	else {
		if (verbose)
			std::cout << "***BAD ROTATE id = " << id << std::endl;
		return -1;
	}
}

void set_contour_values(bool verbose) {

	for (int i = 0; i < crop_height; i += 1) {
		for (int j = 0; j < crop_width; j += 1) {

			if (above_threshold(tmap[i][j])) {

				cmap[i][j] = GRASS;
			} else {

				std::vector<int> *n_case = new std::vector<int>;
				for (int k = i - 1; k <= i + 1; ++k) {
					for (int l = j - 1; l <= j + 1; ++l) {
						if ((k >= 0 && k < crop_height)
								&& (l >= 0 && l < crop_width)
								&& (k != i || l != j)) {
							//std::cout << i << ","<< j << " pushing back " << tmap[k][l] <<  " " << k << ","<< l <<  std::endl;
							//std::cout <<  l << "," << k << ":::";
							n_case->push_back(tmap[k][l]);

						}
					}
				}

				unsigned char id = get_neighbour_case(n_case, verbose);
				if (id != 0 && id != 255 && id != -1) {
					if (verbose)
						std::cout << j << "," << i << std::endl;

					if (verbose)
						std::cout << "---------" << std::endl;
					int r = 0;
					while (id != 1 && id != 2 && id != 3 && id != 6 && id != 7
							&& id != 10 && id != 11 && id != 15 && id != 43
							&& id != 47) {

						if (verbose)
							std::cout << (int) id << " shifting " << std::endl;
						id = rotate_case(n_case, verbose);
						r++;
						if (r > 3) {
							if (verbose)
								std::cout << "***BAD CASE! id = " << (int) id
										<< " on threshold = " << threshold
										<< " EXITING!" << std::endl;
							//exit(0);
							break;
						}
					}
					if (verbose)
						std::cout << "-> " << (int) id << std::endl;

					cmap[i][j] = undo_rotation(id, r, verbose);

					//add random gaps in straights
					if (cmap[i][j] >= CLIFF_WE_SN && cmap[i][j] <= CLIFF_NS_EW)
						if (rand() % 10 == 0)
							cmap[i][j] = GRASS;

//					cmap[i][j] = WATER;
					if (cmap[i][j] == -1)
						cmap[i][j] = GRASS;

				} else {

					if (cmap[i][j] == -1)
						cmap[i][j] = GRASS;
				}
			}

		}

	}

}

void contour_map(int _sub_map_h, int _sub_map_w, bool verbose) {
	sub_map_h = _sub_map_h;
	sub_map_w = _sub_map_w;

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

		//needs to be ran twice to get rid of some cases
		fill_one_tile_gaps(threshold);
		fill_one_tile_gaps(threshold);
		fill_one_tile_gaps(threshold);

		verify_one_tile_gaps(threshold, verbose);
		//fill_one_tile_gaps(threshold);
		//threshold = 65;
		set_contour_values(verbose);
		reset_grass();
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

const std::string get_kf_map_name(int x, int y, std::string grass,
		std::string colour) {

	std::stringstream ss;
	ss << "\"total_x\":" << sub_map_w << "," << std::endl;
	ss << "\"total_y\":" << sub_map_h << "," << std::endl;
	ss << "\"x\":" << x << "," << std::endl;
	ss << "\"y\":" << y << "," << std::endl;
	//ss << "\"colour\":" << colour << "," << std::endl;

	std::string out = ss.str();
	return out;
}

std::string kf_tile_header = "";

void add_detail_tile(FILE* stream, int t, int x, int y) {

	int xoffset, yoffset;
	fprintf(stream, "\t\t{ \"type\": \"");
	switch (t) {
	//not used here
//	case GRASS:
//		int random_grass_type;
//		random_grass_type = rand() % 4;
//		fprintf(stream, "GRASS%d", random_grass_type);
//		break;

	//straights
	case CLIFF_NS_EW:
		fprintf(stream, "CLIFF_NS_EW");
		xoffset = 8;
		yoffset = -8;
		break;
	case CLIFF_NS_WE:
		fprintf(stream, "CLIFF_NS_WE");
		xoffset = 8;
		yoffset = -8;
		break;
	case CLIFF_WE_NS:
		fprintf(stream, "CLIFF_WE_NS");
		xoffset = -8;
		yoffset = 8;
		break;
	case CLIFF_WE_SN:
		fprintf(stream, "CLIFF_WE_SN");
		xoffset = -8;
		yoffset = 8;
		break;
		//corners
	case CLIFF_NE_NS:
		fprintf(stream, "CLIFF_NE_NS");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_NE_SN:
		fprintf(stream, "CLIFF_NE_SN");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_SW_NS:
		fprintf(stream, "CLIFF_SW_NS");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_SW_SN:
		fprintf(stream, "CLIFF_SW_SN");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_SE_NS:
		fprintf(stream, "CLIFF_SE_NS");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_SE_SN:
		fprintf(stream, "CLIFF_SE_SN");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_NW_NS:
		fprintf(stream, "CLIFF_NW_NS");
		xoffset = 8;
		yoffset = 8;
		break;
	case CLIFF_NW_SN:
		fprintf(stream, "CLIFF_NW_SN");
		xoffset = 8;
		yoffset = 8;
		break;
	default:
		break;
	}
	fprintf(stream, "\", \"x\": %d, \"y\": %d, ", x, y);
	fprintf(stream, "\"xoffset\": %d, \"yoffset\": %d }", xoffset, yoffset);

}

void detail_terrain_tiles(FILE* stream, int sub_x, int sub_y, int w_bounds,
		int h_bounds) {
	fprintf(stream, "\t\"background\": \n\t[");

	for (int i = 0; i < h_bounds; ++i) {
		for (int j = 0; j < w_bounds; ++j) {
			int offset_w = (sub_map_w - 1) * sub_x;
			int offset_h = (sub_map_h - 1) * sub_y;

			//tile coordinates
			int tile_x = j + offset_w;
			int tile_y = i + offset_h;

			add_detail_tile(stream, cmap[tile_y][tile_x], tile_x, tile_y);
			if (i == h_bounds - 1 && j == w_bounds - 1)
				fprintf(stream, "\n");
			else
				fprintf(stream, ",\n");

		}
	}

	fprintf(stream, "\t],\n");
}

void add_large_background_tile(FILE* stream, int x, int y, int t) {
	switch (t) {
	case GRASS:
		fprintf(stream, "GRASS%d", rand() % 4);
		break;
	default:
		break;
	}
}

void add_small_background_tile(FILE* stream, int x, int y, int t) {
	switch (t) {
	case GRASS:
		fprintf(stream, "SMALL_GRASS%d", rand() % 4);
		break;
	default:
		break;
	}
}

void background_terrain_tiles(FILE* stream, int sub_x, int sub_y, int w_bounds,
		int h_bounds) {
	//for edge cases need to work out if there is LARGE_BACKGROUND_TILE_SIZE tile available
	int large_tile_size_x;
	int large_tile_size_y;

	fprintf(stream, "\t\"background\": \n\t[");
	//looping over large areas at a time to check if we can use large tiles
	for (int i = 0; i < h_bounds; i + LARGE_BACKGROUND_TILE_SIZE) {
		for (int j = 0; j < w_bounds; j + LARGE_BACKGROUND_TILE_SIZE) {

			large_tile_size_x = LARGE_BACKGROUND_TILE_SIZE;
			large_tile_size_y = LARGE_BACKGROUND_TILE_SIZE;

			int offset_w = (sub_map_w - 1) * sub_x;
			int offset_h = (sub_map_h - 1) * sub_y;

			//tile coordinates
			int tile_x = j + offset_w;
			int tile_y = i + offset_h;

			//set edges
			if (tile_y + LARGE_BACKGROUND_TILE_SIZE >= h_bounds)
				large_tile_size_y = tile_y + LARGE_BACKGROUND_TILE_SIZE
						- h_bounds;
			if (tile_x + LARGE_BACKGROUND_TILE_SIZE >= w_bounds)
				large_tile_size_x = tile_x + LARGE_BACKGROUND_TILE_SIZE
						- w_bounds;

			//using XOR calculate if there is at least one different tile
			int diff = cmap[tile_y][tile_x];
			for (int k = tile_y; k < tile_y + large_tile_size_y; ++k) {
				for (int l = tile_x; l < tile_x + large_tile_size_x; ++l) {
					diff ^= cmap[k][l];
				}
			}

			if (diff == 0) {
				//no difference
				//add type
				fprintf(stream, "\t\t{ \"type\": \"");
				add_large_background_tile(stream, tile_x, tile_y,
						cmap[tile_y][tile_x]);
				//add coords and offset (offset is hardcoded for now)
				fprintf(stream, "\", \"x\": %d, \"y\": %d, ", tile_x, tile_y);
				fprintf(stream, "\"xoffset\": %d, \"yoffset\": %d }", 0, 0);

			} else {
				//different tiles
				for (int k = tile_y; k < tile_y + large_tile_size_y; ++k) {
					for (int l = tile_x; l < tile_x + large_tile_size_x; ++l) {
						//add type
						fprintf(stream, "\t\t{ \"type\": \"");
						add_small_background_tile(stream, l, k, cmap[k][l]);
						//add coords and offset (offset is hardcoded for now)
						fprintf(stream, "\", \"x\": %d, \"y\": %d, ", l, k);
						fprintf(stream, "\"xoffset\": %d, \"yoffset\": %d }", 0,
								0);

						//sort commas
						if (k == tile_y + large_tile_size_y - 1
								&& l == tile_x + large_tile_size_x - 1)
							fprintf(stream, "\n");
						else
							fprintf(stream, ",\n");

					}
				}
			}
			//sort commas
			if (i == h_bounds - 1 && j == w_bounds - 1)
				fprintf(stream, "\n");
			else
				fprintf(stream, ",\n");
		}
	}
	fprintf(stream, "\t],\n");
}

void print_kf_file(FILE* stream, int sub_x, int sub_y) {
	//print wrapper
	fprintf(stream, "{\"map\": {\n");

	//headers etc
	fprintf(stream, "%s", get_kf_map_name(sub_x, sub_x, "grass", "").c_str());
	fprintf(stream, "%s", kf_tile_header.c_str());

	int w_bounds = sub_map_w ? sub_map_w : crop_height;
	int h_bounds = sub_map_h ? sub_map_h : crop_width;

	//conternt wrapper
	fprintf(stream, "\"content\": \n{");
	//background tiles, eg: GRASS, WATER
	background_terrain_tiles(stream, sub_x, sub_y, w_bounds, h_bounds);
	fprintf(stream, ",\n\n");
	//detail tiles, eg: CLIFF
	detail_terrain_tiles(stream, sub_x, sub_y, w_bounds, h_bounds);

	//close content and map
	fprintf(stream, "}\n}\n");

}

void print_kf(FILE* stream) {
	//no submaps
	if (sub_map_h == 0 || sub_map_w == 0) {
		if (stream == 0) {
			stream = fopen(DEFAULT_CONTOUR_KF_FILE, "w");
		}
		if (stream == NULL)
			perror("Error opening file");
		else {
			print_kf_file(stream, 0, 0);
		}
	} else {

		int sub_map_count_h = crop_height / sub_map_h;
		int sub_map_count_w = crop_width / sub_map_w;

		for (int i = 0; i < sub_map_count_h; i++) {
			for (int j = 0; j < sub_map_count_w; ++j) {

				std::ostringstream map_file;
				map_file << KF_MAP_DIRECTORY << j << "_" << i << ".map";
				std::cout << map_file.str() << std::endl;

				FILE* stream = 0;
				if (stream == 0) {
					stream = fopen(map_file.str().c_str(), "w");
				}
				if (stream == NULL)
					perror("Error opening file");
				else {
					print_kf_file(stream, j, i);
					fclose(stream);
				}

			}
		}
	}
}

