#pragma once

#include "data-structure/Entity.hpp"
#include "data-structure/RGBColor.hpp"

#include <string>
#include <algorithm>
#include <iostream>
using namespace std;


namespace inexor {
namespace cube2_map_importer {


	// In Cube2 we can use \f1 up to \f9 to color text.
	// We do not want these bytes in the output console.
	std::string remove_cube2_text_colorisation(std::string colored_map_title)
	{
		// The map title without coloring bytes.
		std::string cleaned_map_title = "";

		for(std::size_t i=0; i<colored_map_title.length(); i++)
		{	
			// Check for backslashes.
			if(colored_map_title[i] != 0x0C)
			{
				// Add to output string.
				cleaned_map_title += colored_map_title[i];
			}
			else
			{
				// Skip the following byte as well!
				// This will ignore the backslash and the coloring code.
				i += 1;
			}
		}

		return cleaned_map_title;
	}


	void print_rgb_color_to_stdcout(RGBColor color)
	{
		cout << "rgb(" << color.r << "," << color.g << "," << color.b << ")" << endl;
	}

	
	void convert_integer_value_to_rgb_code(const int& integer_value, RGBColor& color_value)
	{
		// Convert integer to RGB by shifting bits around.
		color_value.r = (integer_value & 0x00FF0000) >> 16;
		color_value.g = (integer_value & 0x0000FF00) >> 8;
		color_value.b = (integer_value & 0x000000FF) >> 0;
	}

	
	RGBColor convert_3_bytes_to_rgb(const unsigned char* components)
	{
		RGBColor return_value;

		// Convert to RGB by shifting bits.
		return_value.r = static_cast<int>(components[0]) << 16;
		return_value.r = static_cast<int>(components[1]) << 8;
		return_value.r = static_cast<int>(components[2]);

		return return_value;
	}

	
	// TODO: Extend the list?
	// TODO: Read list from a file?
	bool is_integer_map_variable_a_color_value(const std::string& map_variable_name)
	{
		// TODO: Refactor this: make it detect that automatically.
		const std::size_t number_of_entries = 27;

		// The map variables which are rgb color codes.
		const std::string color_map_variables[number_of_entries] = {"skylight", "cloudcolour",
		"fogcolour", "fogdomecolour", "skyboxcolour", "ambient", "shadowmapambient", "sunlight", "cloudboxcolor",
		"watercolour", "water2colour", "water3colour", "water4colour",
		"waterfallcolour", "water2fallcolour", "water2fallcolour", "water2fallcolour",
		"lavacolour", "lava2colour", "lava3colour", "lava4colour",
		"cloudboxcolour", "minimapcolour", "glasscolour"}; 
		
		// Is the name of the map variable equal to one of the list above?
		for(std::size_t i=0; i<number_of_entries; i++)
		{
			if(0 == map_variable_name.compare(color_map_variables[i]))
			{
				// Yes, this is a color variable.
				return true;
			}
		}

		// No, this is not a color variable.
		return false;
	}


	bool is_entity_inside_world(const Entity &e, int worldsize)
	{
		return e.o.x>=0 && e.o.x<worldsize && e.o.y>=0 && e.o.y<worldsize && e.o.z>=0 && e.o.z<worldsize;
	}

};
};
