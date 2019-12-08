#pragma once

#include "vector_geometry.hpp"
#include "Entity.hpp"


namespace inexor {
namespace cube2_map_importer {

	// 
	#define ushort unsigned short
	#define uchar unsigned char
	
	// 
	struct MaterialSurface
	{
		enum
		{
			F_EDIT = 1<<0
		};

		ivec o;
		ushort csize, rsize;
		ushort material, skip;
		uchar orient, flags;
		union
		{
			short index;
			short depth;
		};
		union
		{
			// TODO: Replace with std::shared_ptr!
			Entity *light;
			ushort envmap;
			uchar ends;
		};
	};

};
};
