#pragma once

namespace inexor {
namespace cube2_map_importer {
	

	// 
	#define uchar unsigned char
	#define ushort unsigned short


	// 
	struct ElementSet
	{
		ushort texture, lmid, envmap;
		uchar dim, layer;
		ushort length[2], minvert[2], maxvert[2];
	};

};
};
