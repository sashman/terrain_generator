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

//must be (power of 2) + 1
#define DEFAULT_SIZE 512

#define DEFAULT_VORONOI_SIZE 100

#define DEFAULT_SEA_LEVEL 150

#define DEFAULT_NO_OF_RIVERS 10

#define DEFAULT_NO_OF_SETTLEMENTS 10

#define DEAFULT_SETTLEMENTS_FILE "settlements.txt"



int get_val(int x, int y);

int get_sqr_avg(int x, int y, int l);

int get_dia_avg(int x, int y, int l);

int square_diamond();

void voronoi();

void erosion();

void clear_neg();

//standard print
void print_map();

//xml print
void print_map_xml();

#ifndef _COMPACT
	void run_view();
#endif


void settlements();
void print_settlements(FILE* stream);

#endif /* TERRAIN_GENERATOR_HPP_ */
