#pragma once

namespace inexor {
namespace cube2_map_importer {

	// 
	#define uchar unsigned char
	#define ushort unsigned short
	
	// 
	struct SurfaceCompat
	{
		uchar texcoords[8];
		uchar w, h;
		ushort x, y;
		uchar lmid, layer;
	};

};
};
