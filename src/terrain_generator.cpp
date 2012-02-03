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

extern int tmap_size;
extern int crop_height;
extern int crop_width;
extern int** tmap;

extern int seed;
extern int random_offset;
extern float offset_dr;

extern int voronoi_size;
extern int** voronoi_points;

extern bool neg;



void print_usage(FILE* stream, int exit_code, char* program_name) {
	fprintf(stream, "A program to generate simple terrain with variable formats.\n\n");
	fprintf(stream, "Usage:  %s [options]\n", program_name);
	fprintf(
			stream,
					"  -h  --help             Display this usage information.\n"
					"  -v  --verbose          Print verbose messages.\n"
					"      --height <value>   Crop the map down to specified positive integer height.\n"
					"      --width <value>    Crop the map down to specified positive integer width.\n"
					"      --rough <value>    Define smoothness of the terrain as a float (0.0 < v < 1.0).\n"
					"                         Lower values produce smoother terrain, smaller difference in adjacent tiles.\n"
					"      --seed <value>     Set the initial positive integer height for the algorithm to be generate values from.\n"
					"      --offset <value>   Set the initial offset positive integer height (seed+offset=max possible height).\n"
					"  -n  --negative         Allow for negative height values.\n"
					"  -s  --standard         Use standard output (used as default output):\n"
					"                         width, height and a set of height values all separated by a space.\n"
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



//TODO: create visualisation tool
//TODO: create config file

int main(int argc, char** argv) {


	int next_option;

	/* A string listing valid short options letters.  */
	const char* const short_options = "hsxvn";
	/* An array describing valid long options.  */
	const struct option long_options[] = { { "help", 0, NULL, 'h' }, {
			"standard", 0, NULL, 's' }, { "xml", 0, NULL, 'x' }, { "verbose", 0,
			NULL, 'v' },
			{"height", 1, NULL, 'e'},
			{"width", 1, NULL, 'w'},
			{"rough", 1, NULL, 'r'},
			{"seed", 1, NULL, 'd'},
			{"offset", 1, NULL, 'f'},
			{"negative", 0, NULL, 'n'},
			{ NULL, 0, NULL, 0 } /* Required at end of array.  */
	};

	/* The name of the file to receive program output, or NULL for
	 standard output.  */
	const char* output_filename = NULL;

	/* Remember the name of the program, to incorporate in messages.
	 The name is stored in argv[0].  */
	char* program_name = argv[0];

	do {

		next_option = getopt_long(argc, argv, short_options, long_options,NULL);
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

		case 'w': /* --width use next argument as crop width */

			crop_width = atoi(optarg);
			if(crop_width < 1)
				print_usage(stderr, 1, program_name);

			break;

		case 'r': /* --rough roughness ratio */

			offset_dr = atof(optarg);
			if(offset_dr<0 || offset_dr>1){
				print_usage(stderr, 1, program_name);
			}

			break;

		case 'd': /* --seed value */

			seed = atoi(optarg);
			if(seed<0)
				print_usage(stderr, 1, program_name);


			break;

		case 'f':

			random_offset = atoi(optarg);
			if(random_offset<0)
				print_usage(stderr, 1, program_name);

			break;

		case 'n':

			neg = true;
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

	//interpolate voronoi diagram
	//TODO: add noise to voronoi
	voronoi();

	if(verbose){
		std::cout << "Voronoi points" << std::endl;
		for (int i = 0; i < voronoi_size; ++i) {
			std::cout << "\t" << voronoi_points[i][0] << "," << voronoi_points[i][1] << std::endl;
		}
	}

	//TODO: add erosion algorithm
	erosion();


	if(!neg) clear_neg();

	if (verbose) std::cout << "Finished square diamond" << std::endl;

	//TODO: generate tile types


	if (output_format == STANDRARD_HEIGHTS) {
		print_map();
	} else if (output_format == STANDARD_XML) {
		print_map_xml();
	}

	return 0;
}
