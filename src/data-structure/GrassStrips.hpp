#pragma once

namespace inexor {
namespace cube2_map_importer {

	// 
	#define ushort unsigned short

	// 
	struct GrassStrips
	{
		vec v[4];
		int numv;
		vec4 tcu, tcv;
		plane surface;
		vec center;
		float radius;
		float minz, maxz;
		ushort texture, lmid;
	};

};
};
