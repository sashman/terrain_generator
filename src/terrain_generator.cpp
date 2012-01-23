//============================================================================
// Name        : terrain_generator.cpp
// Author      : os63
// Version     : 0.0.0.1
//============================================================================

#include "terrain_generator.hpp"



enum OutFormat {
	STANDRARD_HEIGHTS, STANDARD_XML
};
int output_format = STANDRARD_HEIGHTS;

/* Whether to display verbose messages.  */
int verbose = 0;

int tmap_size = DEFAULT_SIZE;
int crop_height = 0;
int crop_width = 0;
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

void print_usage(FILE* stream, int exit_code, char* program_name) {
	fprintf(stream, "A program to generate simple terrain with variable formats.\n\n");
	fprintf(stream, "Usage:  %s options\n", program_name);
	fprintf(
			stream,
					"  -h  --help             Display this usage information.\n"
					"  -v  --verbose          Print verbose messages.\n"
					"      --height <value>   Crop the map down to specified positive integer height.\n"
					"      --width <value>    Crop the map down to specified positive integer width.\n"
					"  -s  --standard         Use standard output: width, height and a set of height values all separated by a space.\n"
					"  -x  --xml              Use the following xml output:\n"
					"                           <map width=int height=int>\n"
					"                           [\n"
					"                           <tile x=int y=int>\n"
					"                           <height>int</height>\n"
					"                           </tile>\n"
					"                           ]+\n"
					"                           </map>\n");
	exit(exit_code);
}

//TODO: specify offset reduction ratio
//TODO: specify seed value
//TODO: specify allowance of negative height values
//TODO: create visualisation tool
//TODO: create config file

int main(int argc, char** argv) {


	int next_option;

	/* A string listing valid short options letters.  */
	const char* const short_options = "hsxv";
	/* An array describing valid long options.  */
	const struct option long_options[] = { { "help", 0, NULL, 'h' }, {
			"standard", 0, NULL, 's' }, { "xml", 0, NULL, 'x' }, { "verbose", 0,
			NULL, 'v' },
			{"height", 1, NULL, 'e'},
			{"width", 1, NULL, 'w'},
			{ NULL, 0, NULL, 0 } /* Required at end of array.  */
	};

	/* The name of the file to receive program output, or NULL for
	 standard output.  */
	const char* output_filename = NULL;

	/* Remember the name of the program, to incorporate in messages.
	 The name is stored in argv[0].  */
	char* program_name = argv[0];

	do {

		next_option = getopt_long(argc, argv, short_options, long_options,
				NULL);
		switch (next_option) {
		case 'h': /* -h or --help */
			/* User has requested usage information.  Print it to standard
			 output, and exit with exit code zero (normal termination).  */
			print_usage(stdout, 0, program_name);
			break;
		case 's': /* -s --standard */

			//Use default output format
			//do nothing
			break;

		case 'x': /* -x --xml*/

			//Use xml output format
			output_format = STANDARD_XML;
			break;

		case 'v': /* -v or --verbose */
			verbose = 1;
			break;

		case 'e': /* --height use next argument as crop height */

			crop_height = atoi(optarg);
			if(crop_height < 1)
				print_usage(stderr, 1, program_name);

			break;

		case 'w': /* --height use next argument as crop width */

			crop_width = atoi(optarg);
			if(crop_width < 1)
				print_usage(stderr, 1, program_name);

			break;
		case '?': /* The user specified an invalid option.  */
			/* Print usage information to standard error, and exit with exit
			 code one (indicating abnormal termination).  */
			print_usage(stderr, 1, program_name);
			break;
		case -1: /* Done with options.  */
			break;

		default: /* Something else: unexpected.  */
			abort();
			break;
		}
	} while (next_option != -1);




	//set crop height and width
	if(crop_height<1) crop_height = tmap_size;
	if(crop_width<1) crop_width = tmap_size;

	//if a crop value is set
	//set tmap_size to fit the cropped values
	int max_size = std::max(crop_height,crop_width);
	int max_size_tmp = max_size-1;

	if((max_size_tmp & (max_size_tmp - 1)) == 0){
		//leave set size as highest crop value
		tmap_size = max_size;
	} else {
		//find smallest value such that (value is power of 2) + 1 and value > max_size
		int t = ceil(log2(max_size))+1;
		tmap_size = (1<<t)+1;
	}

	//display info
	if (verbose) {
		std::cout << "Staring square diamond" << std::endl;
		std::cout << "Size: " << crop_width << " x " << crop_height << " original size " << tmap_size << std::endl;
		std::cout << "Starting seed value " << seed << std::endl;
		std::cout << "Starting random offset " << random_offset << std::endl;
		std::cout << "Random offset decrease ratio " << offset_dr << std::endl;
	}

	//init map array
	tmap = new int*[tmap_size];
	for (int i = 0; i < tmap_size; ++i) {
		tmap[i] = new int[tmap_size];
	}

	//fill the array with values
	square_diamond();

	if (verbose)
		std::cout << "Finished square diamond" << std::endl;

	//TODO: generate tile types
	//TODO: generate voronoi diagram
	//TODO: interpolate voronoi diagram


	if (output_format == STANDRARD_HEIGHTS) {
		print_map();
	} else if (output_format == STANDARD_XML) {
		print_map_xml();
	}

	return 0;
}
