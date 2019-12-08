#pragma once

#include "CompressedFileReader.hpp"
#include "Cube2MapOldVersionSupport.hpp"
#include "Cube2Map.hpp"

#include "data-structure/vector_geometry.hpp"


namespace inexor {
namespace cube2_map_importer {


	// The highest supported map format is version 33.
	#define CUBE2_MAP_IMPORTER_HIGHEST_SUPPORTED_MAPVERSION 33
	
	// The maximum supported number of variables in the map.
	// This constant is defined by Cube2.
	#define MAX_NUMVARS 1000
	
	// The maximum supported number of entities.
	// This constant is defined by Cube2.
	#define MAXENTS 10*1000

	// Maximum string length in Sauerbraten.
	// This constant is defined by Cube2.
	#define MAXSTRLEN 260


	// Octree cube flags.
	enum
	{
		OCTSAV_CHILDREN = 0,
		OCTSAV_EMPTY,
		OCTSAV_SOLID,
		OCTSAV_NORMAL,
		OCTSAV_LODCUBE
	};


	// A class for loading a compressed .ogz file, decompressing it and parsing the data into a Cube2Map class instance.
	class Cube2MapDataParser : public CompressedFileReader, public Cube2MapOldVersionSupport
	{
		private:

			// TODO: Implement!
			// Abort the loading/reading process.
			void abort_loading();

			// A pointer to the Cube2 map data class.
			// The data will be written to this address.
			std::shared_ptr<Cube2Map> cube2map;

			// Fills the 8 cubes in an octree node with data.
			void fill_octree_node_with_data(std::shared_ptr<OctreeCube>& c, const ivec &co, int size, bool &failed);
			
			// Loads 8 octree cubes in an octree node.
			std::array<std::shared_ptr<OctreeCube>, 8> load_octree_node(const ivec &co, int size, bool &failed);


		protected:			

			// Sets the value of cube2map.
			void set_map_data_pointer(std::shared_ptr<Cube2Map> cube2_map_ptr);

			// Load the header information.
			bool read_map_header();
			
			// Load the map variables.
			bool read_map_variables();

			// Load the name of the game for which this map was specified for.
			bool read_game_name();

			// Load additional game information.
			bool read_extra_game_info();

			// Load texture information cache.
			bool read_texture_cache();

			// Load the map entities.
			bool read_entites();

			// Load the vslots of the map.
			bool read_vslots();

			// Load octree geometry.
			bool read_octree();

			// Parse the decompressed data.
			bool parse_decompressed_data();
			
			// Load lightmaps.
			// TODO: Implement!
			//bool read_lightmaps();

			// Load PVS visibility tree.
			// TODO: Implement!
			//bool read_pvs_tree();

			// Load blend maps.
			// TODO: Implement!
			//bool read_blendmaps();
			
			// Generate crc32 checksum from the decompressed data.
			//bool generate_crc32_checksum();

		public:

			Cube2MapDataParser();
			
			~Cube2MapDataParser();

	};

};
};
