#include "Cube2Map.hpp"


namespace inexor {
namespace cube2_map_importer {


	// 
	void Cube2Map::reset_data()
	{
		map_title = "";
		world_scale = 0;
		variables.clear();
		game_name = "";
		map_for_cube2_game = false;
		map_for_eisenstern_rpg = false;
		extra_game_info.clear();
		texture_mru.clear();
		number_of_octree_nodes = 0;
		loading_octree_failed = false;
		octree_world_root = NULL;
	}

};
};
