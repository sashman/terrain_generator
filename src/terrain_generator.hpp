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
#define DEFAULT_SIZE 5

#define DEFAULT_VORONOI_SIZE 10


int get_val(int x, int y);

int get_sqr_avg(int x, int y, int l);

int get_dia_avg(int x, int y, int l);

int square_diamond();

void clear_neg();

//standard print
void print_map();

//xml print
void print_map_xml();



#endif /* TERRAIN_GENERATOR_HPP_ */
