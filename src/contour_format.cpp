/*
 * contour_format.cpp
 *
 *  Created on: 8 Jul 2012
 *      Author: sashman
 *
 *      File to translate the heightmap into a grid of tile types found in TILE_CASE (terrian_generator.hpp) enum
 */

#include "terrain_generator.hpp"

extern int crop_height;
extern int crop_width;
extern int** tmap;

int sub_map_h = 0;
int sub_map_w = 0;

int max = 0;
int threshhold_increment = 40;
int threshold = 0;
int nearest_round = 5;

extern int sea_level;

int** cmap;

int convex_case_ids[] =
{ 1 };
int straight_case_ids[] =
{ 2, 3, 6, 7 };
int concave_case_ids[] =
{ 10, 11, 14, 15, 42, 43, 46, 47 };
int bad_case_ids[] =
{ 12, 17, 34, 48, 68, 130, 136 };
std::set<int> convex_set(convex_case_ids, convex_case_ids + 1);
std::set<int> straight_set(straight_case_ids, straight_case_ids + 4);
std::set<int> concave_set(concave_case_ids, concave_case_ids + 8);
std::set<int> bad_set(bad_case_ids, bad_case_ids + 7);

void round_tmap()
{

	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{

			//nearest 10
			//tmap[i][j] = (int) (tmap[i][j] / 10 * 10);

			tmap[i][j] = (int) (tmap[i][j] / nearest_round * nearest_round);

		}
	}

}

bool above_threshold(int height)
{
	return height > threshold;

}

/*
 void print_neighbour_case_id(char case_id)
 {
 //read bits in
 std::vector<char> n_case(8);
 for (int i = n_case.size() - 1; i >= 0; --i)
 {
 char t = (char)((case_id >> i) & 1);
 std::cout<<(int)t<<std::endl;
 n_case.push_back(t);
 }

 //print out
 int c = 0;
 for (std::vector<char>::iterator it = n_case.begin(); it != n_case.end();
 it++)
 {

 //skip middle
 if(c == 4){
 c++;
 std::cout<<"x";
 } else {
 std::cout<< (int)*it ;

 }

 if(c == 2 || c == 6 || c == 8) std::cout<<std::endl;
 c++;
 }

 }
 */

//-1 invalid
char get_neighbour_case(std::vector<int> *n_case, bool verbose)
{
	if (n_case->size() != 8
//			&& n_case->size() != 5 &&
//			n_case->size() != 3
			)
	{
		return -1;
	}

	unsigned char case_id = 0;

	for (int i = n_case->size() - 1; i >= 0; --i)
	{

		case_id |= above_threshold((int) n_case->at(i)) ? 1 << i : 0;

	}

	if (case_id == 255 || case_id == 0)
		return 0;
	if (verbose)
		std::cout << (int) case_id << std::endl;

	if (verbose)
	{
		for (unsigned int i = 0; i < n_case->size(); ++i)
		{

			if (i == 4)
				std::cout << "  ";

			above_threshold((int) n_case->at(i)) ?
					std::cout << ". " : std::cout << "~ ";
			if (i == 2 || i == 4)
				std::cout << std::endl;
		}
	}

	if (verbose)
		std::cout << std::endl;

	return case_id;

}

void reset_grass()
{

	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{

			if (cmap[i][j] == HIGH_GRASS)
				cmap[i][j] = GRASS;

		}
	}

}

/**
 *
 * int t - current threshold
 *
 */
void fill_one_tile_gaps(int t)
{

	//horizontal gaps
	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width - 3; ++j)
		{

			if (!above_threshold(tmap[i][j + 1]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i][j + 2]))
			{
				//if (tmap[i][j + 1] <= t && tmap[i][j] > t && tmap[i][j + 2] > t) {
				tmap[i][j + 1] = t + nearest_round;
			}
		}
	}

	//vertical gaps
	for (int i = 0; i < crop_height - 3; ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{
			if (!above_threshold(tmap[i + 1][j]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i + 2][j]))
			{
				tmap[i + 1][j] = t + nearest_round;
			}
		}
	}

	//diagonal gaps
	for (int i = 0; i < crop_height - 3; ++i)
	{
		for (int j = 0; j < crop_width - 3; ++j)
		{
			//2 rotational cases
			/*
			 *
			 * ^ v v
			 * v v v
			 * v v ^
			 *
			 * and
			 *
			 * v v ^
			 * v v v
			 * ^ v v
			 *
			 */
			if ((above_threshold(tmap[i][j])
					&& !above_threshold(tmap[i + 1][j + 1])
					&& above_threshold(tmap[i + 2][j + 2]))
					|| (above_threshold(tmap[i + 2][j])
							&& !above_threshold(tmap[i + 1][j + 1])
							&& above_threshold(tmap[i][j + 2])))
			{
				tmap[i + 1][j + 1] = t + nearest_round;
			}
		}
	}

}

/**
 *
 * int t - current threshold
 * int n - size of gap to fill in
 *
 */
void fill_n_tile_gaps(int t, int n)
{

	//horizontal gaps
	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width - (n + 2); ++j)
		{

			//if both sides are above threshold
			if (above_threshold(tmap[i][j])
					&& above_threshold(tmap[i][j + n + 1]))
			{
				//check the in between tiles
				bool gap = false;
				for (int k = j + 1; k < j + n + 1; ++k)
				{
					//detect gap
					if (!gap && !above_threshold(tmap[i][k]))
						gap = true;
					//if gap detected, fill in tiles
					if (gap)
					{
						tmap[i][k] = t + nearest_round;
					}
				}
			}
		}
	}

	//vertical gaps
	for (int i = 0; i < crop_height - (n + 2); ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{

			//if both sides are above threshold
			if (above_threshold(tmap[i][j])
					&& above_threshold(tmap[i + n + 1][j]))
			{
				//check the in between tiles
				bool gap = false;
				for (int k = i + 1; k < i + n + 1; ++k)
				{
					//detect gap
					if (!gap && !above_threshold(tmap[k][j]))
						gap = true;
					//if gap detected, fill in tiles
					if (gap)
					{
						tmap[k][j] = t + nearest_round;
					}
				}
			}
		}
	}

	/*
	 * Not filling diagonals
	 *
	 //diagonal gaps
	 for (int i = 0; i < crop_height - (n + 2); ++i) {
	 for (int j = 0; j < crop_width - (n + 2); ++j) {
	 //n=2 examples
	 //			 *
	 //			 *
	 //			 * ^ v v v
	 //			 * v v v v
	 //			 * v v v v
	 //			 * v v v ^
	 //			 *
	 //			 * and
	 //			 *
	 //			 * v v v ^
	 //			 * v v v v
	 //			 * v v v v
	 //			 * ^ v v v
	 //			 *
	 //			 *
	 //check edges

	 if (above_threshold(tmap[i][j])	&& above_threshold(tmap[i + n + 1][j + n + 1])) {
	 bool gap = false;
	 std::cout<<"\t"<< "x " << j << " y "<< i <<
	 " by "<< "x " << j+n+1 << " y "<< i+n+1 <<std::endl;
	 for (int k = j + 1; k < j + n + 1; ++k) {
	 std::cout<<"\t"<< "k=" << k <<std::endl;
	 //detect gap
	 if (!gap && !above_threshold(tmap[k][k]))
	 gap = true;
	 //if gap detected, fill in tiles
	 if (gap) {
	 tmap[k][k] = t + nearest_round;
	 }

	 }
	 }

	 if (above_threshold(tmap[i][j + n + 1])
	 && above_threshold(tmap[i + n + 1][j])) {
	 bool gap = false;
	 for (int k = j + 1; k < j + n; ++k) {
	 //detect gap
	 if (!gap && !above_threshold(tmap[n - k + 1][k]))
	 gap = true;
	 //if gap detected, fill in tiles
	 if (gap) {
	 tmap[n - k + 1][k] = t + nearest_round;
	 }
	 }
	 }

	 }
	 }

	 */
}

void verify_one_tile_gaps(int t, bool verbose)
{
//horizontal gaps
	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width - 3; ++j)
		{

			if (!above_threshold(tmap[i][j + 1]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i][j + 2]))
			{

				if (verbose)
				{
					std::cout << "===" << j << ", " << i
							<< " horizontal gap (threshold = " << t << ")"
							<< std::endl;
					std::cout << "===" << "\t-----" << std::endl;
					for (int k = (i) - 2; k < (i) + 3; k++)
					{
						std::cout << "===";
						for (int l = (j + 1) - 2; l < (j + 1) + 3; l++)
						{
							if ((k >= 0 && k < crop_height)
									&& (l >= 0 && l < crop_width))
							{
								above_threshold(tmap[k][l]) ?
										std::cout << " ." : std::cout << " ~";
							}
						}
						std::cout << std::endl;
					}
					std::cout << "===" << "\t-----" << std::endl;
				}
			}

		}
	}
//vertical gaps
	for (int i = 0; i < crop_height - 3; ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{
			if (!above_threshold(tmap[i + 1][j]) && above_threshold(tmap[i][j])
					&& above_threshold(tmap[i + 2][j]))
			{
				if (verbose)
				{
					std::cout << "===" << j << ", " << i
							<< " vertical gap (threshold = " << t << ")"
							<< std::endl;
					std::cout << "===" << "\t-----" << std::endl;
					for (int k = (i + 1) - 2; k < (i + 1) + 3; k++)
					{
						std::cout << "===";
						for (int l = (j) - 2; l < (j) + 3; l++)
						{
							if ((k >= 0 && k < crop_height)
									&& (l >= 0 && l < crop_width))
							{
								above_threshold(tmap[k][l]) ?
										std::cout << " ." : std::cout << " ~";
							}
						}
						std::cout << std::endl;
					}
					std::cout << "===" << "\t-----" << std::endl;
				}
			}
		}
	}
}

unsigned char rotate_case(std::vector<int> *n_case, bool verbose)
{

	int t5 = (*n_case)[5];
	int t3 = (*n_case)[3];
	(*n_case)[5] = (*n_case)[0];
	(*n_case)[3] = (*n_case)[1];
	(*n_case)[0] = (*n_case)[2];
	(*n_case)[1] = (*n_case)[4];
	(*n_case)[2] = (*n_case)[7];
	(*n_case)[4] = (*n_case)[6];
	(*n_case)[7] = t5;			//(*n_case)[5];
	(*n_case)[6] = t3;

	return get_neighbour_case(n_case, verbose);
}

int undo_rotation(int id, int r, bool verbose)
{

	TILE_CASE convex_cliff_start = CLIFF_NW_SN;
	TILE_CASE straight_cliff_start = CLIFF_WE_SN;
	TILE_CASE concave_cliff_start = CLIFF_SE_SN;

	if (r > 3)
	{
		if (verbose)
			std::cout << "***BAD ROTATE r = " << r << std::endl;
		return -1;
	}

	if (convex_set.count(id) > 0)
		return convex_cliff_start + r;
	else if (straight_set.count(id) > 0)
		return straight_cliff_start + r;
	else if (concave_set.count(id) > 0)
		return concave_cliff_start + r;
	else
	{
		if (verbose)
			std::cout << "***BAD ROTATE id = " << id << std::endl;
		return -1;
	}
}

//return coords to go back to
std::pair<int, int> fix_tile_case(int i, int j, std::vector<int> *n_case,
		int id)
{
	bool found = false;
	std::pair<int, int> backtrack(i, j);


	//Hardcoded gap fixes
	if (bad_set.count(id) > 0)
	{
		found = true;
		tmap[i][j + 1] = threshold + nearest_round;
		backtrack.first = i - 1;
		backtrack.second = j - 1;
	}

	//find gap between tiles
	if (above_threshold(tmap[i - 1][j + 1]) && !above_threshold(tmap[i][j + 1])
			&& above_threshold(tmap[i + 1][j + 1]))
	{
		found = true;
		tmap[i][j + 1] = threshold + nearest_round;
	}

	if (above_threshold(tmap[i + 1][j - 1]) && !above_threshold(tmap[i + 1][j])
			&& above_threshold(tmap[i + 1][j + 1]))
	{
		found = true;
		tmap[i + 1][j] = threshold + nearest_round;
	}

	if (above_threshold(tmap[i - 1][j]) && !above_threshold(tmap[i][j])
			&& above_threshold(tmap[i + 1][j]))
	{
		found = true;
		tmap[i][j] = threshold + nearest_round;
		backtrack.first = i - 1;
		backtrack.second = j - 1;
	}

	if (above_threshold(tmap[i][j - 1]) && !above_threshold(tmap[i][j])
			&& above_threshold(tmap[i][j + 1]))
	{
		found = true;
		tmap[i][j] = threshold + nearest_round;
		backtrack.first = i - 1;
		backtrack.second = j - 1;
	}

	if (above_threshold(tmap[i - 1][j - 1]) && !above_threshold(tmap[i][j - 1])
			&& above_threshold(tmap[i + 1][j - 1]))
	{
		found = true;
		tmap[i][j - 1] = threshold + nearest_round;
		backtrack.first = i - 1;
		backtrack.second = j - 2;
	}

	if (above_threshold(tmap[i - 1][j - 1]) && !above_threshold(tmap[i - 1][j])
			&& above_threshold(tmap[i - 1][j + 1]))
	{
		found = true;
		tmap[i - 1][j] = threshold + nearest_round;
		backtrack.first = i - 2;
		backtrack.second = j - 1;
	}

	if (!found)
	{
		for (int k = i - 1; k <= i + 1; ++k)
		{
			for (int l = j - 1; l <= j + 1; ++l)
			{
				if (k == i && j == l)
					std::cout << "  ";
				else
					above_threshold(tmap[k][l]) ?
							std::cout << ". " : std::cout << "~ ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		exit(0);
	}
//	std::cout << "Going back to " << backtrack.second << "," << backtrack.first
//			<< std::endl;
	return backtrack;
}

void set_contour_values(bool verbose)
{

	for (int i = 0; i < crop_height; i += 1)
	{
		for (int j = 0; j < crop_width; j += 1)
		{

			if (above_threshold(tmap[i][j]))
			{

				cmap[i][j] = GRASS;
			}
			else
			{

				std::vector<int> *n_case = new std::vector<int>;
				for (int k = i - 1; k <= i + 1; ++k)
				{
					for (int l = j - 1; l <= j + 1; ++l)
					{
						if ((k >= 0 && k < crop_height)
								&& (l >= 0 && l < crop_width)
								&& (k != i || l != j))
						{
							//std::cout << i << ","<< j << " pushing back " << tmap[k][l] <<  " " << k << ","<< l <<  std::endl;
							//std::cout <<  l << "," << k << ":::";
							n_case->push_back(tmap[k][l]);

						}
					}
				}

				unsigned char id = get_neighbour_case(n_case, verbose);
				if (id != 0 && id != 255 && id != -1)
				{
					if (verbose)
						std::cout << "Looking at " << j << "," << i
								<< std::endl;

					if (verbose)
						std::cout << "---------" << std::endl;
					int r = 0;

					/*
					 while (id != 1 //  convex cliff
					 && id != 2 && id != 3 && id != 6 && id != 7 // straight cliff
					 && id != 10 && id != 11 && id != 15 && id != 43
					 && id != 47)
					 */
					while (convex_set.count(id) == 0 &&		// convex cliffs
							straight_set.count(id) == 0 &&  // straight cliffs
							concave_set.count(id) == 0)		// concave cliffs
					{

						if (verbose)
							std::cout << (int) id << " shifting " << std::endl;
						id = rotate_case(n_case, verbose);
						r++;
						if (r > 3)
						{
							if (verbose)
							{
								std::cout << "***BAD CASE! id = " << (int) id
										<< " on threshold = " << threshold
										<< std::endl;

								std::cout << "Fixing case " << (int) id
										<< std::endl;

//#define EXIT_ON_BAD_CASE
#ifdef EXIT_ON_BAD_CASE
								std::cout << "EXITING!" << std::endl;
								exit(0);
#endif
							}

							std::pair<int, int> backtrack = fix_tile_case(i, j,
									n_case, id);
							i = backtrack.first;
							j = backtrack.second;

							break;
						}
					}

					if (verbose)
					{
						std::cout << "-> " << (int) id << std::endl;
						if (convex_set.count(id) != 0)
						{
							std::cout << "Part of convex set" << std::endl;
							for (std::set<int>::iterator iter =
									convex_set.begin();
									iter != convex_set.end(); ++iter)
								std::cout << *iter << " ";
							std::cout << std::endl;
						}
						if (straight_set.count(id) != 0)
						{
							std::cout << "Part of straight set " << std::endl;
							for (std::set<int>::iterator iter =
									straight_set.begin();
									iter != straight_set.end(); ++iter)
								std::cout << *iter << " ";
							std::cout << std::endl;
						}
						if (concave_set.count(id) != 0)
						{
							std::cout << "Part of concave set " << std::endl;
							for (std::set<int>::iterator iter =
									concave_set.begin();
									iter != concave_set.end(); ++iter)
								std::cout << *iter << " ";
							std::cout << std::endl;
						}
					}

					cmap[i][j] = undo_rotation(id, r, verbose);

					//add random gaps in straights
					//old way
					//	if (rand() % 10 == 0) cmap[i][j] = GRASS;

					//for a series of 3 straights
					//1 in x chance to create a gap
					int chance = 10;
					//3 in a row indicies must be measured backwards
					if (cmap[i][j] >= CLIFF_WE_SN && cmap[i][j] <= CLIFF_NS_EW)
					{

						switch (cmap[i][j])
						{
						case CLIFF_WE_SN:
							if (cmap[i][j - 1] == CLIFF_WE_SN
									&& cmap[i][j - 2] == CLIFF_WE_SN
									&& (rand() % chance == 0))
							{

								cmap[i][j - 2] = CLIFF_NW_SN;
								cmap[i][j - 1] = GRASS;
								cmap[i][j] = CLIFF_NE_SN;
							}
							break;
						case CLIFF_WE_NS:
							if (cmap[i][j - 1] == CLIFF_WE_NS
									&& cmap[i][j - 2] == CLIFF_WE_NS
									&& (rand() % chance == 0))
							{
								cmap[i][j - 2] = CLIFF_SW_NS;
								cmap[i][j - 1] = GRASS;
								cmap[i][j] = CLIFF_SE_NS;
							}
							break;
						case CLIFF_NS_WE:
							if (cmap[i-1][j] == CLIFF_NS_WE
									&& cmap[i-2][j] == CLIFF_NS_WE
									&& (rand() % chance == 0))
							{
								cmap[i-2][j] = CLIFF_NE_SN;
								cmap[i-1][j] = GRASS;
								cmap[i][j] = CLIFF_SE_NS;
							}
							break;
						case CLIFF_NS_EW:
							if (cmap[i-1][j] == CLIFF_NS_EW
									&& cmap[i-2][j] == CLIFF_NS_EW
									&& (rand() % chance == 0))
							{
								cmap[i-2][j] = CLIFF_NW_SN;
								cmap[i-1][j] = GRASS;
								cmap[i][j] = CLIFF_SW_NS;
							}
							break;
						default:
							break;
						}

					}

//					cmap[i][j] = WATER;
					if (cmap[i][j] == -1)
					{
						if (point_above_sealevel(j, i))
							cmap[i][j] = GRASS;
						else
							cmap[i][j] = WATER;
					}

				}
				else
				{

					if (cmap[i][j] == -1)
					{
						if (point_above_sealevel(j, i))
							cmap[i][j] = GRASS;
						else
							cmap[i][j] = WATER;
					}
				}
			}

		}

	}

}

void contour_map(int _sub_map_h, int _sub_map_w, bool verbose)
{
	sub_map_h = _sub_map_h;
	sub_map_w = _sub_map_w;

	round_tmap();

	max = tmap[0][0];
//set up contour map array
	cmap = new int*[crop_height];
	for (int i = 0; i < crop_height; ++i)
	{
		cmap[i] = new int[crop_width];
		for (int j = 0; j < crop_width; ++j)
		{
			cmap[i][j] = -1;
		}
	}

	for (int i = 0; i < crop_height; ++i)
	{
		for (int j = 0; j < crop_width; ++j)
		{
			if (tmap[i][j] > max)
				max = tmap[i][j];
		}
	}

	std::cout << "\n" << std::endl;
	for (threshold = sea_level; threshold < max; threshold +=
			threshhold_increment)
	{
		std::cout << "T= " << threshold << std::endl;

		//needs to be ran twice to get rid of some cases
		fill_one_tile_gaps(threshold);
		fill_one_tile_gaps(threshold);
		fill_one_tile_gaps(threshold);

		//fill in gaps of 2
		fill_n_tile_gaps(threshold, 4);
		fill_n_tile_gaps(threshold, 4);

		verify_one_tile_gaps(threshold, verbose);
		//fill_one_tile_gaps(threshold);
		//threshold = 65;
		set_contour_values(verbose);
		reset_grass();
	}

}

void print_contour(FILE* stream)
{

	if (stream == 0)
	{
		stream = fopen(DEFAULT_CONTOUR_FILE, "w");
	}
	if (stream == NULL)
		perror("Error opening file");
	else
	{

		fprintf(stream, "%i %i %i\n\n", crop_width, crop_height, max);

		fprintf(stream, "  ");
		for (int i = 0; i < crop_height; ++i)
			fprintf(stream, "%.2i", i);
		fprintf(stream, "\n");
		for (int i = 0; i < crop_height; ++i)
		{
			fprintf(stream, "%02i ", i);
			for (int j = 0; j < crop_width; ++j)
			{

				int t = cmap[i][j];

				if (t == GRASS || t == HIGH_GRASS)
					fprintf(stream, ". ");
				else if (t == CLIFF_NS_EW || t == CLIFF_NS_WE)
					fprintf(stream, "| ");
				else if (t == CLIFF_WE_NS || t == CLIFF_WE_SN)
					fprintf(stream, "- ");
				else if (t == WATER)
					fprintf(stream, "~ ");
				else if (t == CLIFF_NE_NS || t == CLIFF_NE_SN
						|| t == CLIFF_SW_NS || t == CLIFF_SW_SN)
					fprintf(stream, "\\ ");
				else if (t == CLIFF_SE_NS || t == CLIFF_SE_SN
						|| t == CLIFF_NW_NS || t == CLIFF_NW_SN)
					fprintf(stream, "/ ");
				else
					fprintf(stream, "%i ", t);
			}
			fprintf(stream, "\n");
		}

		fprintf(stream, "\n");
		fprintf(stream, "   ");
		for (int i = 0; i < crop_height; ++i)
			fprintf(stream, "%03i ", i);
		fprintf(stream, "\n");
		for (int i = 0; i < crop_height; ++i)
		{
			fprintf(stream, "%02i ", i);
			for (int j = 0; j < crop_width; ++j)
			{

				int t = tmap[i][j];
				fprintf(stream, "%03i ", t);
			}
			fprintf(stream, "\n");
		}

	}
}

