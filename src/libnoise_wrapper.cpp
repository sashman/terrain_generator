/*
 * libnoise_wrapper.cpp
 *
 *  Created on: 7 Jul 2014
 *      Author: sashman
 */

#include "terrain_generator.hpp"

using namespace noise;

int tmap_size = DEFAULT_SIZE;
int crop_height = 0;
int crop_width = 0;
int** tmap;

int seed = 10;
int random_offset = 40;
float offset_dr = .8; //offset decrease ratio

//set of voronoi points
int voronoi_size = 100; //(int)(DEFAULT_VORONOI_SIZE*((float)tmap_size/(float)512));
int** voronoi_points;

float voronoi_alpha = 0.33;

//erosion
int thermal_talus = random_offset / 40; //4/tmap_size;
float thermal_shift = 0.5;

int erosion_steps = 5;

bool neg = false;

bool normalise = false;
int normalise_min = 0;
int normalise_max = 150;

int sea_level = DEFAULT_SEA_LEVEL;
int sand_level = DEAFULT_SAND_LEVEL;
int snow_level = DEFAULT_SNOW_TOP_LEVEL;
int cliff_difference = 100;

//constraint helper methods

bool point_above_sealevel(int x, int y)
{
	return tmap[y][x] > sea_level;
}

bool point_above_sandlevel(int x, int y)
{
	return tmap[y][x] > sand_level;
}

bool point_below_snow_top_level(int x, int y)
{
	return tmap[y][x] < snow_level;
}

void create_height_map()
{

	module::Perlin perlinModule;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule(perlinModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);

	heightMapBuilder.SetDestSize(crop_width, crop_height);

	heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build();

	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("out.bmp");
	writer.WriteDestFile();

	char msg[50];
	sprintf(msg, "Created heightmap: %d %s %d", heightMap.GetWidth(),"x",heightMap.GetHeight());
	log(std::string(msg));

}
