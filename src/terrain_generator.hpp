/*
 * terrain_generator.hpp
 *
 *  Created on: 21 Jan 2012
 *      Author: sash
 */

#ifndef TERRAIN_GENERATOR_HPP_
#define TERRAIN_GENERATOR_HPP_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <cmath>
#include <getopt.h>
#include <stdlib.h>



#define DEFAULT_CONFIG_FILE "config.yaml"

#define DEAFULT_TERRAIN_FILE "terrain.txt"

#define DEFAULT_SIZE 512

#define DEFAULT_VORONOI_SIZE 100

#define DEFAULT_SEA_LEVEL 47//0.588 * 126

#define DEAFULT_SAND_LEVEL 0.7 * 126

#define DEFAULT_SNOW_TOP_LEVEL 0.8 * 126

#define DEAFULT_RIVERS_FILE "rivers.txt"

#define DEFAULT_NO_OF_RIVERS 70

#define DEFAULT_MAX_RIVER_BRANCHES 30

#define DEAFULT_SETTLEMENTS_FILE "settlements.txt"

#define DEFAULT_NO_OF_SETTLEMENTS 30

#define DEFAULT_MIN_DISTANCE_BETWEEN_SETTLEMENTS 400

#define DEAFULT_VEGETATION_FILE "vegetation.txt"

#define DEFAULT_NO_OF_VEGETATION 300

#define DEFAULT_CONTOUR_FILE "contour.txt"

#define DEFAULT_CONTOUR_KF_FILE "contour_kf.txt"


int get_val(int x, int y);

int get_sqr_avg(int x, int y, int l);

int get_dia_avg(int x, int y, int l);

int square_diamond();

void voronoi();

void erosion();

void clear_neg();

void normalise_map();

//standard print
void print_map(FILE* stream);

//xml print
void print_map_xml(FILE* stream);

#ifndef _COMPACT
	void run_view();
#endif

bool point_above_sealevel(int x, int y);

bool point_above_sandlevel(int x, int y);

bool point_below_snow_top_level(int x, int y);

void settlements();
void print_settlements(FILE* stream);

void rivers();

void print_rivers(FILE* stream);

void contour_map();
void print_contour(FILE* stream);
void print_kf(FILE* stream);

class RiverPoint {
public:
	int x;
	int y;
	int river_id;
	RiverPoint* next;
	RiverPoint* branch;

	RiverPoint(int x_, int y_, int river_id_) {
		x = x_;
		y = y_;
		river_id = river_id_;
		next = 0;
		branch = 0;
	}
	~RiverPoint() {
		//delete next;
	}
};

void vegetation();
void print_vegetation(FILE* stream);
#endif /* TERRAIN_GENERATOR_HPP_ */
