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
int threshhold_increment = 50;
int threshold = 0;

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
	 * 			/
	 * higher 	| lower
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

			tmap[i][j] = (int) (tmap[i][j] / 5 * 5);

		}
	}

}

bool above_threshold(int height) {
	return height > threshold;

}

char get_neighbour_case(std::vector<int> *n_case) {
	//-1 invalid
	if (n_case->size() != 8){

		return -1;
	}


	unsigned char case_id = 0;

	for (int i = n_case->size()-1; i >=0; --i) {

		case_id |= above_threshold(n_case->at(i)) ? 1 << i : 0;

	}


	if (case_id == 255 || case_id == 0)
		return 0;
	std::cout << (int)case_id << std::endl;


	for (int i = 0; i < n_case->size(); ++i) {

		if(i==4) std::cout<<"  ";
		above_threshold(n_case->at(i)) ? std::cout <<  ". " :std::cout<<  "~ ";
		if(i == 2 || i == 4) std::cout<<std::endl;
	}

	std::cout<<std::endl;

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

			if (tmap[i][j + 1] <= t && tmap[i][j] > t && tmap[i][j + 2] > t) {
				tmap[i][j + 1] = t + 1;
			}
		}
	}

	//vertical gaps
	for (int i = 0; i < crop_height - 3; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if (tmap[i + 1][j] <= t && tmap[i][j] > t && tmap[i + 2][j] > t) {
				tmap[i + 1][j] = t + 1;
			}
		}
	}

	//diagonal gaps
	for (int i = 0; i < crop_height - 3; ++i) {
		for (int j = 0; j < crop_width-3; ++j){
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
			if((tmap[i][j] > t && tmap[i+1][j+1] <= t && tmap[i+2][j+2] > t) ||
					(tmap[i+2][j] > t && tmap[i+1][j+1] <= t && tmap[i][j+2] > t)){
				tmap[i+1][j+1] = t + 1;
			}
		}
	}

}


unsigned char rotate_case(std::vector<int> *n_case){

//	int shift = 2;
//	//return (case_id >> shift) | (case_id << (sizeof(case_id)*8 - shift));
//	int t;
//
//	for (int i = 0; i < shift; ++i) {
//		t = n_case->at(0);
//		int j = 0;
//		for (; j < n_case->size()-1; ++j) {
//			(*n_case)[j] = (*n_case)[j+1];
//		}
//		(*n_case)[j] = t;
//	}
	int t5 = (*n_case)[5];
	int t3 = (*n_case)[3];
	(*n_case)[5] = (*n_case)[0];
	(*n_case)[3] = (*n_case)[1];
	(*n_case)[0] = (*n_case)[2];
	(*n_case)[1] = (*n_case)[4];
	(*n_case)[2] = (*n_case)[7];
	(*n_case)[4] = (*n_case)[6];
	(*n_case)[7] = t5;//(*n_case)[5];
	(*n_case)[6] = t3;




	return get_neighbour_case(n_case);
}

int undo_rotation(int id, int r){
	if(r>3) return -1;

	if(id == 1) return CLIFF_NW_SN+r;
	else if(id == 2 ||id == 3 ||id == 6 ||id == 7 )return CLIFF_WE_SN+r;
	else if(id == 11 ||id == 15 ||id == 43 ||id == 47 )return CLIFF_SE_SN+r;
	else return -1;
}

void set_contour_values() {

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


				unsigned char id = get_neighbour_case(n_case);
				if (id != 0 && id != 255 && id != -1) {
//					std::cout << j << "," << i << std::endl;

					int r = 0;
					while(id != 1 &&
							id != 2 &&
							id != 3 &&
							id != 6 &&
							id != 7 &&
							id != 11 &&
							id != 15 &&
							id != 43 &&
							id != 47 ){
						if(r>=3){
							std::cout<<"***BAD CASE!"<<std::endl;
							break;
						}
						std::cout<< (int)id << " shifting "<<std::endl;
						id = rotate_case(n_case);
						r++;
					}
					std::cout<<"-> " << (int)id <<std::endl;

					cmap[i][j] = undo_rotation(id,r);
//					cmap[i][j] = WATER;

				}else{

					if(cmap[i][j] == -1) cmap[i][j] = GRASS;
				}
			}

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

	threshold = sea_level;
	std::cout << "\n" << std::endl;
	for (threshold = sea_level; threshold < max; threshold +=
			threshhold_increment) {
		std::cout << "T= " << threshold << std::endl;

		//needs to be ran twice to get rid of some cases
		fill_one_tile_gaps(threshold);
		fill_one_tile_gaps(threshold);
		//threshold = 65;
		set_contour_values();
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

