
/*
 * contour_format.cpp
 *
 *  Created on: 3 Jan 2014
 *      Author: sashman
 *
 *      This has been extracted from contour_format.cpp to reduce the size and make it more readable.
 */

#include "terrain_generator.hpp"

//terrain_generator.cpp
extern int crop_height;
extern int crop_width;

//contour_format.cpp
extern int sub_map_h;
extern int sub_map_w;
extern int** cmap;

std::set<std::string> padded;

void pad_north(rapidjson::Value& detailArray,
		rapidjson::Document::AllocatorType& a, int t, int x, int y) {

//create unique key for the padding
	std::stringstream ss;
	ss << x << "," << (y - 1) << "-" << y;

//check if it exists
	if (padded.count(ss.str()) != 0) {
		//padding should not be encountered more than twice
		padded.erase(ss.str());
		return;
	}

//if not encountered, add it
	padded.insert(ss.str());

//check the dir of the slope on the padding
	int xoffset = 8;
	int yoffset = -8;
	std::string type;
	switch (t) {
	case CLIFF_NE_NS:
	case CLIFF_NW_SN:
		type = "CLIFF_PADDING_NS_EW";
		break;
	case CLIFF_NE_SN:
	case CLIFF_NW_NS:
		type = "CLIFF_PADDING_NS_WE";
		break;
	default:
		type = "UNKNOWN PADDING";
		break;
	}

//add to the tile array
	rapidjson::Value tileObj(rapidjson::kObjectType);
	rapidjson::Value typeString;
	typeString.SetString(type.c_str(), a);
	tileObj.AddMember("type", typeString, a);
	tileObj.AddMember("x", x, a);
	tileObj.AddMember("y", y, a);
	tileObj.AddMember("yoffset", yoffset, a);
	tileObj.AddMember("xoffset", xoffset, a);

	detailArray.PushBack(tileObj, a);

}

void pad_south(rapidjson::Value& detailArray,
		rapidjson::Document::AllocatorType& a, int t, int x, int y) {

//create unique key for the padding
	std::stringstream ss;
	ss << x << "," << (y) << "-" << y + 1;

//check if it exists
	if (padded.count(ss.str()) != 0) {
		//padding should not be encountered more than twice
		padded.erase(ss.str());
		return;
	}

//if not encountered, add it
	padded.insert(ss.str());

//check the dir of the slope on the padding
	int xoffset = 8;
	int yoffset = 24;
	std::string type;
	switch (t) {
	case CLIFF_SW_NS:
	case CLIFF_SE_SN:
		type = "CLIFF_PADDING_NS_EW";
		break;
	case CLIFF_SW_SN:
	case CLIFF_SE_NS:
		type = "CLIFF_PADDING_NS_WE";
		break;
	default:
		type = "UNKNOWN PADDING";
		break;
	}

//add to the tile array
	rapidjson::Value tileObj(rapidjson::kObjectType);
	rapidjson::Value typeString;
	typeString.SetString(type.c_str(), a);
	tileObj.AddMember("type", typeString, a);
	tileObj.AddMember("x", x, a);
	tileObj.AddMember("y", y, a);
	tileObj.AddMember("yoffset", yoffset, a);
	tileObj.AddMember("xoffset", xoffset, a);

	detailArray.PushBack(tileObj, a);

}

void pad_west(rapidjson::Value& detailArray,
		rapidjson::Document::AllocatorType& a, int t, int x, int y) {

//create unique key for the padding
	std::stringstream ss;
	ss << (x - 1) << "-" << x << "," << y;

//check if it exists
	if (padded.count(ss.str()) != 0) {
		//padding should not be encountered more than twice
		padded.erase(ss.str());
		return;
	}

//if not encountered, add it
	padded.insert(ss.str());

//check the dir of the slope on the padding
	int xoffset = -8;
	int yoffset = 8;
	std::string type;
	switch (t) {
	case CLIFF_NW_SN:
	case CLIFF_SW_SN:
		type = "CLIFF_PADDING_WE_SN";
		break;
	case CLIFF_NW_NS:
	case CLIFF_SW_NS:
		type = "CLIFF_PADDING_WE_NS";
		break;
	default:
		type = "UNKNOWN PADDING";
		break;
	}

//add to the tile array
	rapidjson::Value tileObj(rapidjson::kObjectType);
	rapidjson::Value typeString;
	typeString.SetString(type.c_str(), a);
	tileObj.AddMember("type", typeString, a);
	tileObj.AddMember("x", x, a);
	tileObj.AddMember("y", y, a);
	tileObj.AddMember("yoffset", yoffset, a);
	tileObj.AddMember("xoffset", xoffset, a);

	detailArray.PushBack(tileObj, a);

}

void pad_east(rapidjson::Value& detailArray,
		rapidjson::Document::AllocatorType& a, int t, int x, int y) {

//create unique key for the padding
	std::stringstream ss;
	ss << x << "-" << (x + 1) << "," << y;

//check if it exists
	if (padded.count(ss.str()) != 0) {
		//padding should not be encountered more than twice
		padded.erase(ss.str());
		return;
	}

//if not encountered, add it
	padded.insert(ss.str());

//check the dir of the slope on the padding
	int xoffset = 24;
	int yoffset = 8;
	std::string type;
	switch (t) {
	case CLIFF_NE_SN:
	case CLIFF_SE_SN:
		type = "CLIFF_PADDING_WE_SN";
		break;
	case CLIFF_NE_NS:
	case CLIFF_SE_NS:
		type = "CLIFF_PADDING_WE_NS";
		break;
	default:
		type = "UNKNOWN PADDING";
		break;
	}

//add to the tile array
	rapidjson::Value tileObj(rapidjson::kObjectType);
	rapidjson::Value typeString;
	typeString.SetString(type.c_str(), a);
	tileObj.AddMember("type", typeString, a);
	tileObj.AddMember("x", x, a);
	tileObj.AddMember("y", y, a);
	tileObj.AddMember("yoffset", yoffset, a);
	tileObj.AddMember("xoffset", xoffset, a);

	detailArray.PushBack(tileObj, a);

}

void add_detail_tile(rapidjson::Value& detailArray,
		rapidjson::Document::AllocatorType& a, int t, int x, int y) {

	rapidjson::Value tileObj(rapidjson::kObjectType);
	int xoffset, yoffset;
	std::string type;

	switch (t) {
//not used here
//	case GRASS:
//		int random_grass_type;
//		random_grass_type = rand() % 4;
//		fprintf(stream, "GRASS%d", random_grass_type);
//		break;

//straights
	case CLIFF_NS_EW:
		type = "CLIFF_NS_EW";
		xoffset = 8;
		yoffset = -8;
		break;
	case CLIFF_NS_WE:
		type = "CLIFF_NS_WE";
		xoffset = 8;
		yoffset = -8;
		break;
	case CLIFF_WE_NS:
		type = "CLIFF_WE_NS";
		xoffset = -8;
		yoffset = 8;
		break;
	case CLIFF_WE_SN:
		type = "CLIFF_WE_SN";
		xoffset = -8;
		yoffset = 8;
		break;

//=====corners=====
	case CLIFF_NE_NS:
		type = "CLIFF_NE_NS";
		xoffset = 8;
		yoffset = 8;

		pad_north(detailArray, a, t, x, y);
		pad_east(detailArray, a, t, x, y);

		break;
	case CLIFF_NE_SN:
		type = "CLIFF_NE_SN";
		xoffset = 8;
		yoffset = 8;

		pad_north(detailArray, a, t, x, y);
		pad_east(detailArray, a, t, x, y);

		break;
	case CLIFF_SW_NS:
		type = "CLIFF_SW_NS";
		xoffset = 8;
		yoffset = 8;

		pad_south(detailArray, a, t, x, y);
		pad_west(detailArray, a, t, x, y);

		break;
	case CLIFF_SW_SN:
		type = "CLIFF_SW_SN";
		xoffset = 8;
		yoffset = 8;

		pad_south(detailArray, a, t, x, y);
		pad_west(detailArray, a, t, x, y);

		break;
	case CLIFF_SE_NS:
		type = "CLIFF_SE_NS";
		xoffset = 8;
		yoffset = 8;

		pad_south(detailArray, a, t, x, y);
		pad_east(detailArray, a, t, x, y);

		break;
	case CLIFF_SE_SN:
		type = "CLIFF_SE_SN";
		xoffset = 8;
		yoffset = 8;

		pad_south(detailArray, a, t, x, y);
		pad_east(detailArray, a, t, x, y);

		break;
	case CLIFF_NW_NS:
		type = "CLIFF_NW_NS";
		xoffset = 8;
		yoffset = 8;

		pad_north(detailArray, a, t, x, y);
		pad_west(detailArray, a, t, x, y);

		break;
	case CLIFF_NW_SN:
		type = "CLIFF_NW_SN";
		xoffset = 8;
		yoffset = 8;

		pad_north(detailArray, a, t, x, y);
		pad_west(detailArray, a, t, x, y);

		break;
	default:
		type = "UNKNOWN_TYPE";
		xoffset = 0;
		yoffset = 0;
		break;
	}

	rapidjson::Value typeString;
	typeString.SetString(type.c_str(), a);
	tileObj.AddMember("type", typeString, a);
	tileObj.AddMember("x", x, a);
	tileObj.AddMember("y", y, a);
	tileObj.AddMember("yoffset", yoffset, a);
	tileObj.AddMember("xoffset", xoffset, a);

	detailArray.PushBack(tileObj, a);

}

void detail_terrain_tiles(rapidjson::Value &contentObj,
		rapidjson::Document::AllocatorType& a, int sub_x, int sub_y,
		int w_bounds, int h_bounds) {

	rapidjson::Value detailArray(rapidjson::kArrayType);
	detailArray.Clear();

	int offset_w = (sub_map_w) * sub_x;
	int offset_h = (sub_map_h) * sub_y;

	for (int i = 0; i < h_bounds; ++i) {
		for (int j = 0; j < w_bounds; ++j) {

			//tile coordinates
			int tile_x = j + offset_w;
			int tile_y = i + offset_h;

			if (cmap[tile_x][tile_y] >= GRASS)
				continue;

			add_detail_tile(detailArray, a, cmap[tile_x][tile_y], tile_y,
					tile_x);

//			detailArray.PushBack(tileObj,a);

		}
	}

	contentObj.AddMember("detail", detailArray, a);

}

char* get_str_large_background_tile(int x, int y, int t, char* out) {
	switch (t) {
	case GRASS:
		sprintf(out, "GRASS%d", rand() % 4);
		break;
	case WATER:
		sprintf(out, "WATER");
		break;
	default:
		sprintf(out, "UNKNOWN TYPE %d", t);
		break;
	}
	return out;
}

char* get_str_small_background_tile(int x, int y, int t, char* out) {
	switch (t) {
	case GRASS:
		sprintf(out, "SMALL_GRASS%d", rand() % 4);
		break;
	case WATER:
		sprintf(out, "SMALL_WATER");
		break;
	default:
		sprintf(out, "UNKNOWN TYPE %d", t);
		break;
	}
	return out;
}

int get_terrain_type(int i, int j) {
//TODO: needs to be expanded for every terrain type
//if(!point_above_sealevel(x,y)) return WATER;
//else

	if (point_above_sealevel(i, j))
		return GRASS;
	else
		return WATER;

}

bool diff_tiles(int tile_x, int tile_y, int large_tile_size_x,
		int large_tile_size_y) {

	int tile = get_terrain_type(tile_y, tile_x);
	for (int k = tile_y; k < tile_y + large_tile_size_y; ++k) {
		for (int l = tile_x; l < tile_x + large_tile_size_x; ++l) {
			//only check flat terrain types
			if ((tile ^ get_terrain_type(k, l)) != 0) {
//				printf("[%d,%d] %d different form [%d,%d] %d\n", tile_x, tile_y, tile, l,k, get_terrain_type(k, l));
				return true;
			}
		}
	}
	return false;

}

void background_terrain_tiles(rapidjson::Value &contentObj,
		rapidjson::Document::AllocatorType& a, int sub_x, int sub_y,
		int w_bounds, int h_bounds) {
	char outbuf[20];
	int small_count = 0;
	int large_count = 0;

	rapidjson::Value backgroundArray(rapidjson::kArrayType);
	backgroundArray.Clear();

//for edge cases need to work out if there is LARGE_BACKGROUND_TILE_SIZE tile available

	int large_tile_size_y = LARGE_BACKGROUND_TILE_SIZE;

//localise bounds
	h_bounds *= (sub_y + 1);
	w_bounds *= (sub_x + 1);

//looping over large areas at a time to check if we can use large tiles
	for (int i = 0; i < h_bounds && large_tile_size_y > 0; i +=
			LARGE_BACKGROUND_TILE_SIZE) {

		int large_tile_size_x = LARGE_BACKGROUND_TILE_SIZE;
		for (int j = 0; j < w_bounds && large_tile_size_x > 0; j +=
				LARGE_BACKGROUND_TILE_SIZE) {

			int offset_w = (sub_map_w) * sub_x;
			int offset_h = (sub_map_h) * sub_y;

			//tile coordinates
			int tile_x = j + offset_w;
			int tile_y = i + offset_h;

			//set edges
			if (tile_y + large_tile_size_y > h_bounds) {
//				std::cout << "large_tile_size_y(" << large_tile_size_y
//						<< ")change:: " << "tile_y " << tile_y << " +"
//						<< large_tile_size_y << " " << "h_bounds " << h_bounds
//						<< std::endl;
				large_tile_size_y = h_bounds - tile_y;
			}
			if (tile_x + large_tile_size_x > w_bounds) {
//				std::cout << "large_tile_size_x(" << large_tile_size_x
//						<< ")change:: " << "tile_x " << tile_x << " +"
//						<< large_tile_size_x << " " << "w_bounds " << w_bounds
//						<< std::endl;
				large_tile_size_x = w_bounds - tile_x;
			}
			//skip if step is less than 1 tile
			if (large_tile_size_y >= 1 && large_tile_size_x >= 1) {

				//using XOR calculate if there is at least one different tile
				if (!diff_tiles(tile_x, tile_y, large_tile_size_x,
						large_tile_size_y)) {

					rapidjson::Value tileObject(rapidjson::kObjectType);

					//Adding tile type
					rapidjson::Value typeString;
					typeString.SetString(
							get_str_large_background_tile(tile_x, tile_y,
									get_terrain_type(tile_y, tile_x), outbuf),
							a);
					tileObject.AddMember("type", typeString, a);

					tileObject.AddMember("x", tile_y, a);
					tileObject.AddMember("y", tile_x, a);
					//offset is 0, all background tiles are never offset
					tileObject.AddMember("xoffset", 0, a);
					tileObject.AddMember("yoffset", 0, a);
					backgroundArray.PushBack(tileObject, a);
					large_count++;

				} else {

//					std::cout << "Small tile range " << large_tile_size_y << " "
//							<< large_tile_size_x << " " << tile_y << "-"
//							<< tile_y + large_tile_size_y << " by " << tile_x
//							<< "-" << tile_x + large_tile_size_x << " "
//							<< "y_count "
//							<< (tile_y + large_tile_size_y) - tile_y << " "
//							<< "x_count "
//							<< (tile_x + large_tile_size_x) - tile_x
//							<< std::endl;
//				small_count = 0;
					//different tiles
					for (int k = tile_y; k < tile_y + large_tile_size_y; ++k) {
						for (int l = tile_x; l < tile_x + large_tile_size_x;
								++l) {

							rapidjson::Value tileObject(rapidjson::kObjectType);
							rapidjson::Value typeString;
							typeString.SetString(
									get_str_small_background_tile(k, l,
											get_terrain_type(k, l), outbuf), a);
							tileObject.AddMember("type", typeString, a);
							tileObject.AddMember("x", k, a);
							tileObject.AddMember("y", l, a);
							tileObject.AddMember("xoffset", 0, a);
							tileObject.AddMember("yoffset", 0, a);
							backgroundArray.PushBack(tileObject, a);
							small_count++;

						}
					}
				}
			}
		}
	}

	contentObj.AddMember("background", backgroundArray, a);
}

void print_kf_file(FILE* stream, int sub_x, int sub_y) {

	rapidjson::Document mapDoc;

	mapDoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = mapDoc.GetAllocator();

	rapidjson::Value mapObject(rapidjson::kObjectType);

	int sub_map_count_h = crop_height / sub_map_h;
	int sub_map_count_w = crop_width / sub_map_w;

//adding map meta data

	mapObject.AddMember("total_x", sub_map_count_w, allocator);
	mapObject.AddMember("total_y", sub_map_count_h, allocator);
	mapObject.AddMember("x", sub_x, allocator);
	mapObject.AddMember("y", sub_y, allocator);

	rapidjson::Value contentObject(rapidjson::kObjectType);

	int w_bounds = sub_map_w ? sub_map_w : crop_width;
	int h_bounds = sub_map_h ? sub_map_h : crop_height;

//background tiles, eg: GRASS, WATER
	background_terrain_tiles(contentObject, allocator, sub_x, sub_y, w_bounds,
			h_bounds);

//detail tiles, eg: CLIFF
	detail_terrain_tiles(contentObject, allocator, sub_x, sub_y, w_bounds,
			h_bounds);

	mapObject.AddMember("content", contentObject, allocator);
	mapDoc.AddMember("map", mapObject, allocator);

//Print JSON to file
	rapidjson::StringBuffer strbuf;

	rapidjson::FileStream out_file(stream);
	rapidjson::PrettyWriter<rapidjson::FileStream> writer(out_file);

	mapDoc.Accept(writer);

}

void print_kf(FILE* stream) {
//no submaps
	if (sub_map_h == 0 || sub_map_w == 0) {
		if (stream == 0) {
			stream = fopen(DEFAULT_CONTOUR_KF_FILE, "w");
		}
		if (stream == NULL)
			perror("Error opening file");
		else {
			print_kf_file(stream, 0, 0);
		}
	} else {

		int sub_map_count_h = crop_height / sub_map_h;
		int sub_map_count_w = crop_width / sub_map_w;

		int map_total = 0;
		for (int i = 0; i < sub_map_count_h; i++) {
			for (int j = 0; j < sub_map_count_w; ++j) {

				std::ostringstream map_file;
				map_file << KF_MAP_DIRECTORY << j << "_" << i << ".map";
				std::cout << map_file.str() << std::endl;

				FILE* stream = 0;
				if (stream == 0) {
					stream = fopen(map_file.str().c_str(), "w");
				}
				if (stream == NULL || stream == 0)
					perror("Error opening file");
				else {
					print_kf_file(stream, j, i);
					fclose(stream);
				}

				map_total++;
			}
		}
		std::cout << map_total << " maps created in " << KF_MAP_DIRECTORY
				<< std::endl;
	}
}
