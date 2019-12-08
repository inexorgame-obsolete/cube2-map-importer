#pragma once

namespace inexor {
namespace cube2_map_importer {

	#define uchar unsigned char

	// 
	enum
	{
		LMID_AMBIENT = 0,
		LMID_AMBIENT1,
		LMID_BRIGHT,
		LMID_BRIGHT1,
		LMID_DARK,
		LMID_DARK1,
		LMID_RESERVED
	};

	// 
	enum
	{
		LAYER_TOP    = (1<<5),
		LAYER_BOTTOM = (1<<6),
		LAYER_DUP    = (1<<7),
		LAYER_BLEND  = LAYER_TOP|LAYER_BOTTOM,
		MAXFACEVERTS = 15
	};

	// 
	struct SurfaceInfo
	{
		uchar lmid[2];
		uchar verts, numverts;

		int totalverts() const { return numverts&LAYER_DUP ? (numverts&MAXFACEVERTS)*2 : numverts&MAXFACEVERTS; }
		bool used() const { return lmid[0] != LMID_AMBIENT || lmid[1] != LMID_AMBIENT || numverts&~LAYER_TOP; }
		void clear() { lmid[0] = LMID_AMBIENT; lmid[1] = LMID_AMBIENT; numverts = (numverts&MAXFACEVERTS) | LAYER_TOP; }
		void brighten() { lmid[0] = LMID_BRIGHT; lmid[1] = LMID_AMBIENT; numverts = (numverts&MAXFACEVERTS) | LAYER_TOP; }
	};


};
};
