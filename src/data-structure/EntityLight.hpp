#pragma once

#include "vector_geometry.hpp"


namespace inexor {
namespace cube2_map_importer {

	// 
	struct EntityLight
	{
		vec color;
		
		vec dir;

		int millis;

		EntityLight()
		{
			color.r = 1;
			color.g = 1;
			color.b = 1;
			color.x = 0;
			color.y = 0;
			color.z = 1;
			millis = -1;
		}
	};

};
};
