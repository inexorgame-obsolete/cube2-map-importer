#pragma once

#include <memory>


namespace inexor {
namespace cube2_map_importer {

	// A structure for old cube2 map formats.
	// This has been introduced in Cube2 to
	// ensure compatibility with old maps.
	struct LegacyOctaHeader
	{

		// Reset memory in constructor.
		LegacyOctaHeader()
		{
			std::memset(&magic, 0, sizeof(magic));
			version = 0;
			headersize = 0;
			worldsize = 0;
			numents = 0;
			numpvs = 0;
			lightmaps = 0;
			lightprecision = 0;
			lighterror = 0;
			lightlod = 0;
			ambient = 0;
			std::memset(&watercolour, 0, sizeof(watercolour));
			blendmap = 0;
			lerpangle = 0;
			lerpsubdiv = 0;
			lerpsubdivsize = 0;
			bumperror = 0;
			std::memset(&skylight, 0, sizeof(skylight));
			std::memset(&lavacolour, 0, sizeof(lavacolour));
			std::memset(&waterfallcolour, 0, sizeof(waterfallcolour));
			std::memset(&reserved, 0, sizeof(reserved));
			std::memset(&maptitle, 0, sizeof(maptitle));
		}


		char magic[4];              // "OCTA"
		int version;                // any >8bit quantity is little endian
		int headersize;             // sizeof(header)
		int worldsize;
		int numents;
		int numpvs;
		int lightmaps;
		int lightprecision, lighterror, lightlod;
		unsigned char ambient;
		unsigned char watercolour[3];
		unsigned char blendmap;
		unsigned char lerpangle, lerpsubdiv, lerpsubdivsize;
		unsigned char bumperror;
		unsigned char skylight[3];
		unsigned char lavacolour[3];
		unsigned char waterfallcolour[3];
		unsigned char reserved[10];
		char maptitle[128];
	};

};
};
