//============================================================================
// Name        : terrain_generator.cpp
// Author      : sash
//============================================================================

#include "terrain_generator.hpp"

//Only using STANDARD_HEIGHTS
//this is then used by output format to eventually create json maps
enum OUTPUT_FORMAT
{
	STANDRARD_HEIGHTS, STANDARD_XML, OPENGL_VIEW
};
OUTPUT_FORMAT output_format = STANDRARD_HEIGHTS;

//Whether to display verbose messages.
int verbose = 0;

std::string output_file = DEAFULT_TERRAIN_FILE;
std::string config_file = DEFAULT_CONFIG_FILE;

//the main array that holds the height map data
extern int** tmap;
//properties of the height map
//the height map must be generated as a square of sides equal to (a power of 2) + 1
extern int tmap_size;
//the crop height/width is then used to crop it to the requested size
extern int crop_height;
extern int crop_width;

//perlin noise properties
//see http://libnoise.sourceforge.net/tutorials/tutorial4.html
extern int octaves;
extern double frequency;
//the "roughness" value, the change in random_offset for the next iteration
extern float offset_dr;

//store the random, TODO: allow for the user to provide the seed
int random_seed = 0;
//used to determine the relative size of 1 pixel to the output
//not used
int scale = 1;

//number of times the height map should be eroded
extern int erosion_steps;

//true if allowing for negative values
extern bool neg;

//noramlise the height map between the given max and min value
extern bool normalise;
extern int normalise_min;
extern int normalise_max;

//height levels of different terrain types
extern int sea_level;
extern int sand_level;
extern int snow_level;
extern int cliff_difference;

//max number of rivers
extern int n_rivers;
//number of times a river can produce a branch
extern int max_branches;

//max number of settlements
extern int n_settlements;
//minimum distance between two settlements
extern int min_distance;

//max number of vegetation units
extern int n_vegetation;
//the areas around a unit of vegetation which can influence it, used for water (maybe?) and spawning new vegetation
extern int root_radius;
//number of iteration for which spawn new vegetation units based on the existing units
extern int generations;

//TODO: Update usage string
void print_usage(FILE* stream, int exit_code, char* program_name)
{
	fprintf(stream, "A program to generate terrain and features with variable formats.\n\n");
	fprintf(stream, "Usage:  %s [options]\n", program_name);
	fprintf(stream, "  -?  --help                 Display this usage information.\n"
			"  -c  --config <filename>    Use custom config file.\n"
			"  -v  --verbose              Print verbose messages.\n"
			"  -h  --height <value>       Crop the map down to specified positive integer height.\n"
			"  -w  --width <value>        Crop the map down to specified positive integer width.\n"
			"      --rough <value>        Define smoothness of the terrain as a float (0.0 < v < 1.0).\n"
			"                             Lower values produce smoother terrain, smaller difference in adjacent tiles.\n"
			"  -s  --randomseed <value>   The seed used to generate random numbers.\n"
			"  -n  --negative             Allow for negative height values.\n"
			"");
	exit(exit_code);
}

void log(std::string msg)
{

	if (verbose)
		std::cout << msg << std::endl;
}

//helper method used to return a string of the file contents
std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw(errno);
}

//using a set filename, read the contents of the config file and set the values
void read_json_config()
{

	std::string json = get_file_contents(config_file.c_str());
	rapidjson::Document d;
	d.Parse<0>(json.c_str());

	output_file = d["output_file"].GetString();
	crop_width = d["height"].GetInt();
	crop_height = d["width"].GetInt();
	octaves = d["octaves"].GetInt();
	frequency = d["frequency"].GetDouble();
	scale = d["scale"].GetInt();
	offset_dr = d["rough"].GetDouble();
	normalise = (strcmp(d["normalise"].GetString(), "true") == 0);
	normalise_min = d["normalise_min"].GetInt();
	normalise_max = d["normalise_max"].GetInt();
	sea_level = d["sea_level"].GetInt();
	sand_level = d["sand_level"].GetInt();
	snow_level = d["snow_level"].GetInt();
	cliff_difference = d["cliff_height_difference"].GetInt();
	n_rivers = d["number_of_river_sources"].GetInt();
	max_branches = d["max_branches_per_source"].GetInt();
	n_settlements = d["number_of_settlements"].GetInt();
	min_distance = d["min_distance_between_settlements"].GetInt();
	n_vegetation = d["number_of_vegetation"].GetInt();
	root_radius = d["vegetation_root_radius"].GetInt();
	generations = d["vegetation_generations"].GetInt();

}

//TODO:
//TODO:  Use libnoise http://libnoise.sourceforge.net/tutorials/index.html !!!
//TODO:
void generate()
{

	std::stringstream ss;
	ss << "Random seed: " << random_seed;
	log(ss.str());

	create_height_map();

	/*
	 std::cout << "Drawing contours" << std::endl;
	 contour_map(32, 32, verbose);
	 print_contour(0);
	 print_kf(0);
	 */

}

int main(int argc, char** argv)
{

	int next_option;

	/* A string listing valid short options letters.  */
	const char* const short_options = "h:w:c:vns:";
	/* An array describing valid long options.  */
	const struct option long_options[] =
	{
	{ "help", 0, NULL, '?' },
	{ "config", 1, NULL, 'c' },
	{ "verbose", 0, NULL, 'v' },
	{ "height", 1, NULL, 'h' },
	{ "width", 1, NULL, 'w' },
	{ "rough", 1, NULL, 'r' },
	{ "negative", 0, NULL, 'n' },
	{ "randomseed", 1, NULL, 's' },
	{ NULL, 0, NULL, 0 } /* Required at end of array.  */
	};

	/* Remember the name of the program, to incorporate in messages.
	 The name is stored in argv[0].  */
	char* program_name = argv[0];

	//read config fist so the args can override it
	if (fopen(config_file.c_str(), "r"))
		read_json_config();

	do
	{

		next_option = getopt_long(argc, argv, short_options, long_options,
		NULL);

		switch (next_option)
		{

		case 'c': //config file

			if (strcmp(config_file.c_str(), optarg) != 0)
			{
				config_file = optarg;
				read_json_config();
			}

			break;

		case 'v': /* -v or --verbose */
			verbose = 1;
			break;

		case 'h': /* --height use next argument as crop height */

			crop_height = atoi(optarg);
			if (crop_height < 1)
				print_usage(stderr, 1, program_name);

			break;

		case 'w': /* --width use next argument as crop width */

			crop_width = atoi(optarg);
			if (crop_width < 1)
				print_usage(stderr, 1, program_name);

			break;

		case 'r': /* --rough roughness ratio */

			offset_dr = atof(optarg);
			if (offset_dr < 0 || offset_dr > 1)
			{
				print_usage(stderr, 1, program_name);
			}

			break;

		case 'n': /* allow negative values */

			neg = true;
			break;

		case 's': /* random seed value */

			random_seed = atof(optarg);
			srand(random_seed);
			break;

		case '?':
			/* -h or --help */
			/* User has requested usage information.  Print it to standard
			 output, and exit with exit code zero (normal termination).  */
			print_usage(stdout, 0, program_name);
			break;

		case -1: /* Done with options.  */
			break;

		default: /* Something else: unexpected.  */
			abort();
			break;
		}
	} while (next_option != -1);

	generate();

	return 0;
}

