#pragma once

#include "data-structure/OctaHeader.hpp"
#include "data-structure/LegacyOctaHeader.hpp"
#include "data-structure/MapVariables.hpp"
#include "data-structure/VSlots.hpp"
#include "data-structure/EntityAdditionalData.hpp"
#include "data-structure/OctreeCube.hpp"

#include <vector>
#include <string>
#include <memory>


namespace inexor {
namespace cube2_map_importer {


	// A class which stored all the data of a Cube2 map.
	// The data is filled out by the Cube2MapImporter interface.
	class Cube2Map
	{
		public:

			// The header data of the Cube2 map.
			OctaHeader header;
		
			// A legacy version of the header data of the Cube2 map.
			// This is required to load old maps.
			LegacyOctaHeader old_game_version_header;

			// The name of the map file.
			// Without path and without .ogz extension.
			std::string file_name;

			// The title of the map.
			// This contains Cube2 map coloring bytes.
			std::string map_title;
		
			// The title of the map.
			// This does not contain Cube2 map coloring bytes.
			std::string clean_map_title;

			// The size of the world.
			int world_scale;

			// A vector of map variables which store the map settings
			// like skybox, watercolor, maptitle, fog and much more.
			std::vector<MapVariable> variables;

			// The name of the game the map was made for.
			std::string game_name;

			// True if it is a Cube2 map.
			bool map_for_cube2_game;

			// True if the octree loading failed.
			bool loading_octree_failed;

			// True if it is an Eisenstern map.
			// Eisenstern was a rgp game based on the Cube2 engine.
			// The project has been discontinued.
			bool map_for_eisenstern_rpg;

			// The extra game information buffer.
			std::vector<unsigned char> extra_game_info;

			// The size of the additional game info buffer.
			std::size_t extra_game_info_size;

			// The texture cache: MRU means most recently used.
			std::vector<unsigned short> texture_mru;

			// Extra entity info.
			std::vector<unsigned char> extra_entity_info;
		
			// The entities on the map
			std::vector<EntityAdditionalData> entities;

			// The VSlots (vertex slots) of the map geometry.
			std::vector<VSlot> vertex_slots;

			// The octree data structure.
			std::shared_ptr<OctreeCube> octree_world_root;

			// The total number of octree nodes in this map.
			std::size_t number_of_octree_nodes;

			// Resets all data of the class members.
			void reset_data();
	
	};

};
};
