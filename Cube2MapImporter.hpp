#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

#include <zlib.h>


#include "OctaHeader.hpp"
#include "CompatHeader.hpp"
#include "MapVariables.hpp"
#include "ExtendedEntity.hpp"
#include "Helpers.hpp"
#include "VSlots.hpp"
#include "Octree.hpp"


namespace inexor {
namespace cube2_map_importer {


	// The highest supported map format is version 33.
	#define CUBE2_MAP_IMPORTER_HIGHEST_SUPPORTED_MAPVERSION 33
	
	// The maximum supported number of variables in the map.
	#define MAX_NUMVARS 1000
	
	// The maximum supported number of entities.
	#define MAXENTS 10*1000

	// Maximum string length in Sauerbraten.
	#define MAXSTRLEN 260


	// 
	enum CUBE2_MAP_IMPORTER_STATUS
	{
		IMPORTER_READY = 0,
		IMPORTER_WORKING
	};
	

	// A structure which contains 8 std::shared_ptr<cube>.
	struct CubePointArray
	{
		std::shared_ptr<cube> entry[8];
	};


	// 
	class Cube2MapImporter
	{
		private:
			
			// Resets all data of the class members.
			void reset_data();

			// Loads an entire file into memory.
			bool load_file_into_memory(const std::string& map_file_name);
			
			// Decompress the loaded Cube2 map file using zlib.
			bool decompress_data();

			// Load the header information.
			bool parse_map_header();
			
			// Load the map variables.
			bool parse_map_variables();

			// Load the name of the game for which this map was specified for.
			bool parse_game_name();

			// Load additional game information.
			bool parse_extra_game_info();

			// Load texture information cache.
			bool parse_texture_cache();

			// Load the map entities.
			bool parse_entites();

			// Load the vslots of the map.
			bool parse_vslots();

			// Load octree geometry.
			bool parse_octree();

			// Load lightmaps.
			bool parse_lightmaps();

			// Load PVS visibility tree.
			bool parse_pvs_tree();

			// Load blend maps.
			bool parse_blendmaps();

			// Generate crc32 checksum from the decompressed data.
			bool generate_crc32_checksum();
			
			// Parse the decompressed data.
			bool parse_decompressed_data();
			
			// TODO: Implement!
			void abort_loading_process();
			
			// In Cube2 we can use \f1 up to \f9 to color text.
			// We do not want these bytes in the output console.
			std::string remove_cube2_texture_coloring_bytes(std::string colored_map_title)
			{
				// The map title without coloring bytes.
				std::string cleaned_map_title = "";

				for(std::size_t i=0; i<colored_map_title.length(); i++)
				{	
					// Check for backslashes.
					if(colored_map_title[i] != 0x0C)
					{
						// Add to output string.
						cleaned_map_title += colored_map_title[i];
					}
					else
					{
						// Skip the following byte as well!
						// This will ignore the backslash and the coloring code.
						i += 1;
					}
				}

				return cleaned_map_title;
			}

			// TODO: Move this into helper section.
			bool is_entity_inside_world(const Entity &e) const;
			
			// 
			CubePointArray generate_new_cubes(uint face = F_EMPTY, int mat = MAT_AIR);

			// 
			std::shared_ptr<cube> load_octree_children(const ivec &co, int size, bool &failed);

		protected:

			// Read memory from buffer and use it to fill out a structure.
			void read_memory_into_structure(void* target_memory, const std::size_t number_of_bytes_to_read, const std::size_t target_memory_size);

			// Reads a slice from the buffer.
			std::vector<unsigned char> read_slice_from_buffer(const std::size_t how_manumber_of_bytes_to_readny_bytes);

			// Reads a slice from the buffer as std::string.
			std::string read_slice_from_buffer_as_string(const std::size_t number_of_bytes_to_read);

			// Reads one byte from the buffer.
			unsigned char read_one_byte_from_buffer();

			// Reads a float value from the buffer.
			float read_float_from_buffer();

			// Reads an integer value from the buffer.
			int read_int_from_buffer();

			// Reads an unsigned int from the buffer.
			unsigned int read_unsigned_int_from_buffer();

			// Reads a short value from the buffer.
			short read_short_from_buffer();

			// Reads an unsigned short from the buffer.
			unsigned short read_unsigned_short_from_buffer();
			
			// Skips a certain amount of bytes before reading again.
			bool skip_reading_buffer_bytes(const std::size_t how_many_bytes_to_skip);

			// 
			void fix_entity_data(Entity &e, const int version);

			// 
			bool is_integer_map_variable_a_color_value(const std::string& map_variable_name);
			
			// 
			void convert_integer_value_to_rgb_code(const int& integer_value, RGBColor& output_color);
			
			// 
			RGBColor convert_3_bytes_to_rgb(const unsigned char* color_components);
			
			// 
			void print_rgb_color_to_stdcout(RGBColor output_color);

		protected:
			

			// 
			CUBE2_MAP_IMPORTER_STATUS importer_status;
		
			// 
			std::string map_file_path_name;

			// 
			std::size_t compressed_file_size;

			// 
			std::vector<char> compressed_data_buffer;

			// 
			std::vector<unsigned char> decompressed_data_buffer;

			// 
			std::size_t compressed_data_size;

			// The read position of the decompressed data buffer.
			std::size_t buffer_read_position;

			// The header of the Cube2 map.
			OctaHeader map_header;

			// 
			CompatibilityMapHeader compatible_map_header;

			// The title of the map
			std::string map_title;

			// 
			std::string clean_map_title;

			// 
			int world_scale;

			// 
			std::vector<MapVariable> map_variables;

			// 
			std::string game_name;

			// 
			bool map_for_cube2_game;

			// 
			bool map_for_eisenstern_rpg;

			// 
			std::vector<unsigned char> extra_game_information;

			// Texture cache.
			std::vector<unsigned short> texture_mru;

			// 
			std::size_t extra_game_info_size;

			// Extra entity info.
			// std::optional because it might not exist for this map!
			std::vector<unsigned char> extra_entity_info;
		
			// The entities on the map
			std::vector<ExtendedEntity> map_entities;

			// 
			std::vector<VSlot> map_vertex_slots;

			// 
			std::shared_ptr<cube> octree_world_root;

			// 
			std::size_t all_octree_nodes;

			// 
			bool loading_octree_failed;

			// 
			void setcubeext(cube &c, cubeext *ext);
			
			// 
			cubeext *growcubeext(cubeext *old, int maxverts);

			// 
			void loadc(const std::shared_ptr<cube>& c, const ivec &co, int size, bool &failed);
			
			// 
			cubeext *newcubeext(cube &c, int maxverts, bool init);

			// 
			void edgespan2vectorcube(cube &c);

		public:
			
			// 
			Cube2MapImporter()
			{
				reset_data();
			}

			// 
			~Cube2MapImporter()
			{
			}


			// Returns the status of the map importer.
			const CUBE2_MAP_IMPORTER_STATUS get_status() const
			{
				return importer_status;
			}

			// TODO: Generate CRC32 checksum!

			// Load a map file.
			bool load_map_file(const std::string& map_file_name);



	};

};
};
