/*
 * midpoint_displacement.cpp
 *
 *  Created on: 29 Jan 2012
 *      Author: sash
 */

#include "terrain_generator.hpp"

int tmap_size = DEFAULT_SIZE;
int crop_height = 0;
int crop_width = 0;
int** tmap;

int seed = 0;
int random_offset = 100;
float offset_dr = .8; //offset decrease ratio


//set of voronoi points
int voronoi_size = DEFAULT_VORONOI_SIZE;
int** voronoi_points;

bool neg = false;

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

void voronoi(){


}




//replace all negative values with 0
void clear_neg(){

	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			if(tmap[i][j]<0)tmap[i][j] = 0;
		}
	}

}

//standard print
void print_map() {
	printf("%i %i ", crop_width, crop_height);
	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			printf("%i ", tmap[i][j]);
		}
	}
}

//xml print
void print_map_xml() {
	//TODO: add tile type value
	printf("<map width='%d' height='%d'>\n", crop_width, crop_height);
	for (int i = 0; i < crop_height; ++i) {
		for (int j = 0; j < crop_width; ++j) {
			printf("<tile x='%d' y='%d'>\n\t<height>%i</height>\n</tile>\n", i,
					j, tmap[i][j]);
		}
		//printf("\n");
	}
	printf("</map>");
}




