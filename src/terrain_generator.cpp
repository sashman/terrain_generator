//============================================================================
// Name        : terrain_generator.cpp
// Author      : os63
// Version     : 0.0.0.1
//============================================================================

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 include "boost/program_options/options_description.hpp"
 include "boost/program_options/parsers.hpp"
 include "boost/program_options/variables_map.hpp"
 */

//must be (power of 2) + 1
#define DEFAULT_SIZE 129

//namespace po = boost::program_options;

enum OutFormat {
	STANDRARD_HEIGHTS, STANDARD_XML
};
int output_format = STANDRARD_HEIGHTS;

int tmap_size = DEFAULT_SIZE;
int** tmap;

int seed = 0;
int random_offset = 100;
float offset_dr = .8; //offset decrease ratio

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

void print_map() {
	for (int i = 0; i < tmap_size; ++i) {
		for (int j = 0; j < tmap_size; ++j) {
			printf("%i ", tmap[i][j]);
		}
		//printf("\n");
	}
}

void print_map_xml() {
	printf("<map width='%d' height='%d'>\n", tmap_size, tmap_size);
	for (int i = 0; i < tmap_size; ++i) {
		for (int j = 0; j < tmap_size; ++j) {
			printf("<tile x='%d' y='%d'>\n\t<height>%i</height>\n</tile>\n", i,
					j, tmap[i][j]);
		}
		//printf("\n");
	}
	printf("</map>");
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

void check_out_format(int argc, char** argv) {
	if (argc < 2)
		return;

	if (strcmp(argv[1], "-s") == 0) {
		output_format = STANDRARD_HEIGHTS;
	} else if (strcmp(argv[1], "-x") == 0) {
		output_format = STANDARD_XML;
	}
}

int main(int argc, char** argv) {

	//TODO: remove
	check_out_format(argc, argv);

	/*
	 //argument parser
	 po::options_description desc("Usage");
	 desc.add_options()
	 ("help", "produce help message")
	 ("size", po::value<int>(), "set map size must be a (power of 2) + 1, e.g.: 129, 257")
	 ;

	 po::variables_map vm;
	 po::store(po::parse_command_line(argc, argv, desc), vm);
	 po::notify(vm);

	 if(vm.count("help")){
	 std::cout << desc << "\n";
	 return 1;
	 }

	 if (vm.count("size")){
	 tmap_size = vm["size"].as<int>();
	 }
	 */

	//std::cout << "Started" << std::endl;
	//std::cout << "Size = " << tmap_size << std::endl;
	//init map array
	tmap = new int*[tmap_size];
	for (int i = 0; i < tmap_size; ++i) {
		tmap[i] = new int[tmap_size];
	}
	//fill the array with values
	square_diamond();
	//std::cout << "Finished square diamond" << std::endl;

	if (output_format == STANDRARD_HEIGHTS) {
		std::cout << tmap_size << " " << tmap_size << " ";
		print_map();
	} else if (output_format == STANDARD_XML) {
		print_map_xml();
	}

	return 0;
}
