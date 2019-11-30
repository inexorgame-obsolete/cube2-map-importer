#pragma once

namespace inexor {
namespace cube2_map_importer {

	// Structure for description of a Cube2 map file header.
	struct OctaHeader
	{
		OctaHeader()
		{
			magic[0] = 0;
			magic[1] = 0;
			magic[2] = 0;
			magic[3] = 0;
			version = 0;
			headersize = 0;
			worldsize = 0;
			number_of_entities = 0;
			number_of_pvs_trees = 0;
			lightmaps = 0;
			blendmap = 0;
			number_of_map_variables = 0;
			number_of_vslots = 0;
		}

		// The magic number "OCTA".
		char magic[4];
    
		// The version of the map file.
		// any >8bit quantity is little endian.
		// The current version of the map file which
		// is used in the game is defined by MAPVERSION.
		int version;
    
		// The size of the header section.
		int headersize;
    
		// The size of the world section.
		int worldsize;
    
		// The number of entities in the map.
		// The maximum number of entities
		// is defined by MAXENTS.
		int number_of_entities;
    
		// The number of PVS trees.
		// A PVS tree is a data structure which
		// describes if a part of the map is visible.
		int number_of_pvs_trees;
    
		// The number of light maps.
		int lightmaps;
    
		// The number of blend maps.
		int blendmap;
    
		// The number of variables in the map.
		int number_of_map_variables;
    
		// The number of v slots.
		int number_of_vslots;
	};

};
};
