#pragma once


namespace inexor {
namespace cube2_map_importer {

	// A class for RGB colors.
	struct RGBColor
	{
		void reset()
		{
			r = 0;
			g = 0;
			b = 0;
		}

		RGBColor()
		{
			reset();
		}

		int r;
		int g;
		int b;
	};

};
};
