#pragma once

#include "RGBColor.hpp"

#include <string>


namespace inexor {
namespace cube2_map_importer {

	// The type of map variable.
	enum MAPVARTYPE
	{
		ID_UNDEFINED = -1,
		ID_VAR = 0,
		ID_FVAR = 1,
		ID_SVAR = 2,
		ID_COLORVAR
	};


	// Structure for map variables.
	struct MapVariable
	{
		// Constructor.
		MapVariable()
		{
			type = ID_UNDEFINED;
			name = "";
			value_int = 0;
			value_float = 0.0f;
			value_string = "";
			value_color.reset();
		}

		// The type of map variable.
		MAPVARTYPE type;

		// The name of the variable.
		std::string name;

		// Note: We do not use unions here to keep it simple.
		// This means we allocate this memory for every variable
		// ignoring that only one of these types will be used.
		// This is negligible!
		int value_int;
		float value_float;
		std::string value_string;
		RGBColor value_color;
	};

};
};
