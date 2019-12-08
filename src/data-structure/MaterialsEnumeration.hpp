#pragma once

namespace inexor {
namespace cube2_map_importer {


	// Hardcoded texture numbers.
	enum
	{
		DEFAULT_SKY = 0,
		DEFAULT_GEOM
	};


	// 
	enum
	{
		MATF_INDEX_SHIFT  = 0,
		MATF_VOLUME_SHIFT = 2,
		MATF_CLIP_SHIFT   = 5,
		MATF_FLAG_SHIFT   = 8,
		MATF_INDEX  = 3 << MATF_INDEX_SHIFT,
		MATF_VOLUME = 7 << MATF_VOLUME_SHIFT,
		MATF_CLIP   = 7 << MATF_CLIP_SHIFT,
		MATF_FLAGS  = 0xFF << MATF_FLAG_SHIFT
	};


	// Cube empty-space materials.
	enum
	{
		MAT_AIR      = 0,                      // the default, fill the empty space with air
		MAT_WATER    = 1 << MATF_VOLUME_SHIFT, // fill with water, showing waves at the surface
		MAT_LAVA     = 2 << MATF_VOLUME_SHIFT, // fill with lava
		MAT_GLASS    = 3 << MATF_VOLUME_SHIFT, // behaves like clip but is blended blueish
		MAT_NOCLIP   = 1 << MATF_CLIP_SHIFT,   // collisions always treat cube as empty
		MAT_CLIP     = 2 << MATF_CLIP_SHIFT,   // collisions always treat cube as solid
		MAT_GAMECLIP = 3 << MATF_CLIP_SHIFT,   // game specific clip material
		MAT_DEATH    = 1 << MATF_FLAG_SHIFT,   // force player suicide
		MAT_ALPHA    = 4 << MATF_FLAG_SHIFT    // alpha blended
	};

};
};
