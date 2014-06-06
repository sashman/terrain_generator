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

//store the random, TODO: allow for the user to provide the seed
int random_seed = 0;
//used to determine the relative size of 1 pixel to the output
//not used
int scale = 1;

//for more explanation on the below values see Midpoint displacement algorithm (http://www.gameprogrammer.com/fractal.html)
//the starting point of the corners of the height map
extern int seed;
//the max value of the next offset
extern int random_offset;
//the "roughness" value, the change in random_offset for the next iteration
extern float offset_dr;

//number of voronoi points to be used
extern int voronoi_size;

//the fraction used as the strength of the voronoi values, when interpolating with the height map
extern float voronoi_alpha;

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
	fprintf(stream,
			"A program to generate terrain and features with variable formats.\n\n");
	fprintf(stream, "Usage:  %s [options]\n", program_name);
	fprintf(stream,
			"  -h  --help                 Display this usage information.\n"
					"  -c  --config <filename>    Use custom config file.\n"
					"  -v  --verbose              Print verbose messages.\n"
					"      --height <value>       Crop the map down to specified positive integer height.\n"
					"      --width <value>        Crop the map down to specified positive integer width.\n"
					"      --rough <value>        Define smoothness of the terrain as a float (0.0 < v < 1.0).\n"
					"                             Lower values produce smoother terrain, smaller difference in adjacent tiles.\n"
					"      --seed <value>         Set the initial positive integer height for the algorithm to be generate values from.\n"
					"      --offset <value>       Set the initial offset positive integer height (seed+offset=max possible height).\n"
					"      --plate <value>        Set the fraction of the tectonic plates appearance.\n"
					"                             Higher values will give a more 'ripped apart' look, values too close to 1 are not\n"
					"                             recommended for realistic terrain. (0.0 < v < 1.0)\n"
					"      --erosion <value>      Number of erosion iterations over the terrain. Must be a positive integer.\n"
					"  -n  --negative             Allow for negative height values.\n"
					"  -s  --standard             Use standard output to be written to a file (used as default output).\n"
					"                             width, height and a set of height values all separated by a space.\n"
					"  -g  --graphical            Display the height map using a 3D OpenGL view.\n"
					"  -x  --xml                  Use the following xml output to be written to a file:\n"
					"                           <map width=int height=int>\n"
					"                           [<tile x=int y=int>\n"
					"                           <height>int</height>\n"
					"							<type>string</type>\n"
					"                           </tile>\n]+"
					"                           </map>\n");
	exit(exit_code);
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
	scale = d["scale"].GetInt();
	seed = d["seed"].GetInt();
	random_offset = d["offset"].GetInt();
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
//TODO:  Use libnoise http://libnoise.sourceforge.net/tutorials/index.html
//TODO:
void generate()
{

	//set crop height and width
	if (crop_height < 1)
		crop_height = tmap_size;
	if (crop_width < 1)
		crop_width = tmap_size;

	//if a crop value is set
	//set tmap_size to fit the cropped values
	int max_size = std::max(crop_height, crop_width);
	int max_size_tmp = max_size - 1;

	if ((max_size_tmp & (max_size_tmp - 1)) == 0)
	{
		//leave set size as highest crop value
		tmap_size = max_size;
	}
	else
	{
		//find smallest value such that (value is power of 2) + 1 and value > max_size
		int t = ceil(log2(max_size)) + 1;
		tmap_size = (1 << t) + 1;
	}

	double finish = 0;
	//display info
	if (verbose)
	{
		std::cout << "Using " << config_file << std::endl;
		std::cout << "Staring square diamond" << std::endl;
		std::cout << "Size: " << crop_width << " x " << crop_height
				<< " original size " << tmap_size << std::endl;
		std::cout << "Starting seed value " << seed << std::endl;
		std::cout << "Starting random offset " << random_offset << std::endl;
		std::cout << "Random offset decrease ratio " << offset_dr << std::endl;

	}

	//init map array
	tmap = new int*[tmap_size];
	for (int i = 0; i < tmap_size; ++i)
	{
		tmap[i] = new int[tmap_size];
		for (int j = 0; j < tmap_size; j++)
			tmap[i][j] = 0;
	}

//	initialize random seed:
//	use for generating a random map every time
//  srand ( time(NULL) );
	//harcoded for now as produces a nice map for testing
	srand(12);

	//fill the array with values
	square_diamond();

	//interpolate voronoi diagram
	//TODO: add noise to voronoi
	if (verbose)
	{
		std::cout << "Voronoi points " << voronoi_size << std::endl;
		/*
		 for (int i = 0; i < voronoi_size; ++i) {
		 std::cout << "\t" << voronoi_points[i][0] << "," << voronoi_points[i][1] << std::endl;
		 }
		 */
	}
	voronoi();

	erosion();

	if (!neg)
		clear_neg();

//		finish = clock() - start;
	if (verbose)
		std::cout << "Finished square diamond " << (finish / 1000000)
				<< std::endl;
	double sqadia = (finish / 1000000);

	if (normalise)
	{
		if (verbose)
			std::cout << "Normalising with value range " << normalise_min << "-"
					<< normalise_max << std::endl;
		normalise_map();
	}

	if (output_format == STANDRARD_HEIGHTS)
	{
		print_map(fopen(output_file.c_str(), "w"));
	}
	else if (output_format == STANDARD_XML)
	{
		print_map_xml(fopen(output_file.c_str(), "w"));
	}

	if (scale > 0 && crop_height > 256 && crop_width > 256)
	{

//			start = clock();
		if (verbose)
			std::cout << "Generating rivers" << std::endl;
		rivers();
//			finish = clock() - start;
		if (verbose)
			std::cout << "Done " << (finish / 1000000) << std::endl;
		double rivers_time = (finish / 1000000);
		print_rivers(0);

//			start = clock();
		if (verbose)
			std::cout << "Generating vegetation" << std::endl;
		vegetation(verbose);
//			finish = clock() - start;
		if (verbose)
			std::cout << "Done " << (finish / 1000000) << std::endl;
		double veg_time = (finish / 1000000);
		print_vegetation(0);

		if (verbose)
			std::cout << "Generating settlements" << std::endl;
		settlements();
//			finish = clock() - start;
		if (verbose)
			std::cout << "Done " << (finish / 1000000) << std::endl;
		double settlement_time = (finish / 1000000);
		print_settlements(0);

		std::cout << crop_height << "\t"
				<< (sqadia + rivers_time + veg_time + settlement_time) << "\t"
				<< sqadia << "\t " << rivers_time << "\t" << veg_time << "\t"
				<< settlement_time << std::endl;
	}

	std::cout << "Drawing contours" << std::endl;
	contour_map(32, 32, verbose);
	print_contour(0);
	print_kf(0);

}

int main(int argc, char** argv)
{

	int next_option;

	/* A string listing valid short options letters.  */
	const char* const short_options = "hc:sxgvna:";
	/* An array describing valid long options.  */
	const struct option long_options[] =
	{
	{ "help", 0, NULL, 'h' },
	{ "config", 1, NULL, 'c' },
	{ "standard", 0, NULL, 's' },
	{ "xml", 0, NULL, 'x' },
	{ "graphic", 0, NULL, 'g' },
	{ "verbose", 0, NULL, 'v' },
	{ "height", 1, NULL, 'e' },
	{ "width", 1, NULL, 'w' },
	{ "rough", 1, NULL, 'r' },
	{ "seed", 1, NULL, 'd' },
	{ "offset", 1, NULL, 'f' },
	{ "plate", 1, NULL, 'p' },
	{ "erosion", 1, NULL, 'o' },
	{ "negative", 0, NULL, 'n' },
	{ "randomseed", 1, NULL, 'a' },
	{ NULL, 0, NULL, 0 } /* Required at end of array.  */
	};

	/* Remember the name of the program, to incorporate in messages.
	 The name is stored in argv[0].  */
	char* program_name = argv[0];

	if (fopen(config_file.c_str(), "r"))
		read_json_config();

	do
	{

		next_option = getopt_long(argc, argv, short_options, long_options,
				NULL);
		switch (next_option)
		{
		case 'h': /* -h or --help */
			/* User has requested usage information.  Print it to standard
			 output, and exit with exit code zero (normal termination).  */
			print_usage(stdout, 0, program_name);
			break;

			case 'c': //config file

					if(strcmp(config_file.c_str(),optarg) != 0)
					{
						config_file = optarg;
						read_json_config();
					}

					break;
					case 's': /* -s --standard */

					//Use default output format
					//do nothing
					break;

					case 'x': /* -x --xml*/

					//Use xml output format
					output_format = STANDARD_XML;
					break;

					case 'g': /* -g --graphical*/

					//Display the map as 3d opengl representation
					output_format = OPENGL_VIEW;
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
					if(offset_dr<0 || offset_dr>1)
					{
						print_usage(stderr, 1, program_name);
					}

					break;

					case 'd': /* --seed value */

					seed = atoi(optarg);
					if(seed<0)
					print_usage(stderr, 1, program_name);

					break;

					case 'f': /* --offset value */

					random_offset = atoi(optarg);
					if(random_offset<0)
					print_usage(stderr, 1, program_name);

					break;

					case 'p': /* --plate  vonornoi interpolation value */

					voronoi_alpha = atof(optarg);
					if(voronoi_alpha<0 || voronoi_alpha>1)
					{
						print_usage(stderr, 1, program_name);
					}

					break;

					case 'o': /* --erosion number of erosion iterations */

					erosion_steps = atoi(optarg);
					if(erosion_steps<0)
					print_usage(stderr, 1, program_name);

					break;

					case 'n': /* allow negative values */

					neg = true;
					break;

					case 'a': /* random seed value */

//			srand ( time(NULL) );
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

	generate();

	return 0;
}

