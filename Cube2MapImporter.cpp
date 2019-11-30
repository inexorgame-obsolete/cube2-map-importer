#include "Cube2MapImporter.hpp"


namespace inexor {
namespace cube2_map_importer {

	
	void Cube2MapImporter::reset_data()
	{
		// Resetting members.
		importer_status = IMPORTER_READY;
		map_file_path_name = "";
		compressed_file_size = 0;
		compressed_data_buffer.clear();
		decompressed_data_buffer.clear();
		compressed_data_size = 0;
		buffer_read_position = 0;
		map_title = "";
		world_scale = 0;
		map_variables.clear();
		game_name = "";
		map_for_cube2_game = false;
		map_for_eisenstern_rpg = false;
		extra_game_information.clear();
		texture_mru.clear();
		all_octree_nodes = 0;
		octree_world_root = NULL;
	}


	void Cube2MapImporter::abort_loading_process()
	{
		// TODO: Implement
	}

	
	std::vector<unsigned char> Cube2MapImporter::read_slice_from_buffer(const std::size_t number_of_bytes_to_read)
	{
		// Do not exceed the buffer's size when reading!
		if(buffer_read_position + number_of_bytes_to_read > decompressed_data_buffer.size())
		{
			cout << "Do not read out of bounds!" << endl;
			return std::vector<unsigned char>();
		}

		// Where do we begin?
		std::vector<unsigned char>::const_iterator start = decompressed_data_buffer.begin() + buffer_read_position;
			
		// Where do we end?
		std::vector<unsigned char>::const_iterator end = decompressed_data_buffer.begin() + buffer_read_position + number_of_bytes_to_read;

		// Construct a new vector.
		std::vector<unsigned char> buffer_slice(start, end);

		// Move position forward.
		buffer_read_position += number_of_bytes_to_read;
	
		// Return the data.
		return buffer_slice;
	}


	std::string Cube2MapImporter::read_slice_from_buffer_as_string(const std::size_t number_of_bytes_to_read)
	{
		// Do not exceed the buffer's size when reading!
		if(buffer_read_position + number_of_bytes_to_read > decompressed_data_buffer.size())
		{
			cout << "Do not read out of bounds!" << endl;
			return std::string("");
		}

		// Read a slice of the buffer as vector of char.
		std::vector<unsigned char> slice_value = read_slice_from_buffer(number_of_bytes_to_read);
		
		std::string return_value = "";

		// Create a new string as return value.
		return_value = std::string(slice_value.begin(), slice_value.end());


		// Convert the vector of char to std::string.
		// This std::string will be null-terminated automatically.

		// 
		return return_value;
	}


	bool Cube2MapImporter::load_file_into_memory(const std::string& file_name)
	{
		cout << "Attempting to load file " << file_name.c_str() << "." << endl;

		// Store the map's file name with path.
		map_file_path_name = file_name;

		// Open a file stream at the end to ask for the file size later.
		// TODO: Do we need std::ios::in here?
		std::ifstream cube2_map_file(file_name.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

		// 
		if(cube2_map_file.is_open())
		{
			cout << "Opened file successfully." << endl;

			// Check the size of the file in bytes.
			compressed_file_size = static_cast<std::size_t>(cube2_map_file.tellg());

			cout << "File size: " << compressed_file_size << " bytes." << endl;

			cout << "Allocating memory." << endl;

			// Preallocate the memory for the file buffer
			// to prevent allocation during the read operation.
			compressed_data_buffer.resize(compressed_file_size);

			// Set the reading pointer to the start
			// of the file before starting to read data.
			cube2_map_file.seekg(0, std::ios::beg);

			cout << "Reading file data." << endl;

			// Read the entire file into memory.
			cube2_map_file.read(&compressed_data_buffer.data()[0], compressed_file_size);

			// Close the file stream.
			cube2_map_file.close();

			cout << "Read " << compressed_data_buffer.size() << " bytes." << endl;
		}
		else
		{
			cout << "Error: could not open file!" << endl;
			return false;
		}

		// Loaded file successfully into memory.
		return true;
	}


	unsigned char Cube2MapImporter::read_one_byte_from_buffer()
	{
		// Do not exceed the buffer's size when reading!
		if(1 + buffer_read_position > decompressed_data_buffer.size())
		{
			cout << "Do not read out of bounds!" << endl;
			return 0x0;
		}

		// Increase read position by one.
		buffer_read_position++;

		// Return the required byte.
		return decompressed_data_buffer[buffer_read_position -1];
	}


	float Cube2MapImporter::read_float_from_buffer()
	{
		float return_value = 0.0f;

		// Convert 4 bytes (unsigned char) to float.
		unsigned char float_buffer[] = {0x0, 0x0, 0x0, 0x0};
		float_buffer[0] = read_one_byte_from_buffer();
		float_buffer[1] = read_one_byte_from_buffer();
		float_buffer[2] = read_one_byte_from_buffer();
		float_buffer[3] = read_one_byte_from_buffer();

		// Copy memory.
		std::memcpy(&return_value, &float_buffer, sizeof(return_value));
		return return_value;
	}


	int Cube2MapImporter::read_int_from_buffer()
	{
		int return_value = 0;

		// Convert 4 bytes (unsigned char) to float.
		unsigned char int_buffer[] = {0x0, 0x0, 0x0, 0x0};
		int_buffer[0] = read_one_byte_from_buffer();
		int_buffer[1] = read_one_byte_from_buffer();
		int_buffer[2] = read_one_byte_from_buffer();
		int_buffer[3] = read_one_byte_from_buffer();

		// Copy memory.
		std::memcpy(&return_value, &int_buffer, sizeof(return_value));
		return return_value;
	}


	unsigned int Cube2MapImporter::read_unsigned_int_from_buffer()
	{
		unsigned int return_value = 0;

		// Convert 4 bytes (unsigned char) to float.
		unsigned char uint_buffer[] = {0x0, 0x0, 0x0, 0x0};
		uint_buffer[0] = read_one_byte_from_buffer();
		uint_buffer[1] = read_one_byte_from_buffer();
		uint_buffer[2] = read_one_byte_from_buffer();
		uint_buffer[3] = read_one_byte_from_buffer();

		// Copy memory.
		std::memcpy(&return_value, &uint_buffer, sizeof(return_value));
		return return_value;
	}


	short Cube2MapImporter::read_short_from_buffer()
	{
		short return_value = 0;

		// Convert 4 bytes (unsigned char) to float.
		unsigned char short_buffer[] = {0x0, 0x0};
		short_buffer[0] = read_one_byte_from_buffer();
		short_buffer[1] = read_one_byte_from_buffer();

		// Copy memory.
		std::memcpy(&return_value, &short_buffer, sizeof(return_value));
		return return_value;		
	}


	unsigned short Cube2MapImporter::read_unsigned_short_from_buffer()
	{
		unsigned short return_value = 0;

		// Convert 4 bytes (unsigned char) to float.
		unsigned char ushort_buffer[] = {0x0, 0x0};
		ushort_buffer[0] = read_one_byte_from_buffer();
		ushort_buffer[1] = read_one_byte_from_buffer();

		// Copy memory.
		std::memcpy(&return_value, &ushort_buffer, sizeof(return_value));
		return return_value;	
	}


	void Cube2MapImporter::read_memory_into_structure(void* target_memory, const std::size_t number_of_bytes_to_read, const std::size_t target_memory_size)
	{
		// Read memory from buffer.
		std::vector<unsigned char> memory_source = read_slice_from_buffer(number_of_bytes_to_read);

		// 
		std::memcpy(target_memory, &memory_source[0], target_memory_size);
	}


	bool Cube2MapImporter::skip_reading_buffer_bytes(const std::size_t number_of_bytes_to_skip)
	{
		// Do not exceed the buffer's size when reading!
		if(number_of_bytes_to_skip + buffer_read_position > decompressed_data_buffer.size())
		{
			cout << "Do not read out of bounds!" << endl;
			return false;
		}

		// Move the read position.
		buffer_read_position += number_of_bytes_to_skip;
		return true;
	}


	CubePointArray Cube2MapImporter::generate_new_cubes(uint face, int mat)
	{
		// A structure which contains 8 std::shared_ptr<cube>.
		CubePointArray new_cubes;

		for(std::size_t i=0; i<8; i++)
		{
			// Create a new shared pointer.
			new_cubes.entry[i] = std::make_shared<cube>();

			new_cubes.entry[i]->children = NULL;

			new_cubes.entry[i]->ext = NULL;

			new_cubes.entry[i]->visible = NULL;
			
			new_cubes.entry[i]->merged = NULL;

			for(std::size_t k=0; k<3; k++)
			{
				new_cubes.entry[i]->faces[k] = face;
			}
		}
		
		all_octree_nodes++;

		// Return a structure which contains 8 new std::shared_ptr<cube>.
		return new_cubes;
	}


	bool Cube2MapImporter::decompress_data()
	{
		// Check if the input file is empty.
		if(0 == compressed_data_buffer.size())
		{
			cout << "Info: The file is empty." << endl;
			return false;
		}

		cout << "Clearing decompressed data buffer." << endl;

		// Clear buffer for decompressed data.
		decompressed_data_buffer.clear();

		// Calculate the half of the data size.
		std::size_t decompressed_data_half_size = compressed_data_buffer.size() / 2;
		
		cout << "Half size: " << decompressed_data_half_size << "." << endl;

		// Approximate the size of the decompressed data.
		// Let's assume that the decompressed file is 1.5x as large as the compressed file.
		std::size_t approximate_decompressed_size = compressed_data_buffer.size() + decompressed_data_half_size;

		cout << "Allocating memory for decompressed data buffer." << endl;

		// Preallocate memory before writing to buffer.
		// We don't know the size of the decompressed data
		// before decompression. We surely need to allocate
		// more memory later anyways.
		decompressed_data_buffer.resize(approximate_decompressed_size);


		// The zlib decompression stream.
		z_stream decompression_stream;

		// TODO: Use static_cast instead of (Bytef *) ?
		decompression_stream.next_in = (Bytef *)(compressed_data_buffer.data());

		// 
		decompression_stream.next_out = (Bytef *)(decompressed_data_buffer.data());

		// 
		decompression_stream.avail_in = compressed_data_buffer.size();

		// Setup for the stream.
		decompression_stream.total_out = 0;

		// 
		decompression_stream.avail_out = approximate_decompressed_size;

		// No custom allocator and no free method.
		decompression_stream.zalloc = Z_NULL;
		decompression_stream.zfree = Z_NULL;

		// Are we done decompressing?
		bool decompression_finished = false;
		
		cout << "----------------------------------------------------------------------------" << endl;

		cout << "Starting data decompression." << endl;
		
		// Start decompression (inflating data)
		if(inflateInit2(&decompression_stream, (16 + MAX_WBITS)) != Z_OK)
		{
			// Something went wrong.
			return false;
		}

		// How much percent of the data has been decompressed?
		float decompression_progress = 0.0f;

		// 
		while(!decompression_finished)
		{
			// Check if buffer for decompressed data is too small.
			if(decompression_stream.total_out >= approximate_decompressed_size)
			{
				// Calculate the new size of the memory.
				std::size_t new_memory_size = approximate_decompressed_size + decompressed_data_half_size;

				cout << "Allocating memory. Old: " << approximate_decompressed_size << " New: " << new_memory_size << endl;

				// Allocate more memory.
				decompressed_data_buffer.resize(new_memory_size);

				// Increase the size of the memory.
				approximate_decompressed_size += decompressed_data_half_size;
			}

			// Calculate the next memory address.
			Bytef* address_of_next_output = &decompressed_data_buffer[0] + decompression_stream.total_out;

			// Submit new memory address.
			decompression_stream.next_out = address_of_next_output;

			// How much space is left to write to?
			decompression_stream.avail_out = approximate_decompressed_size - decompression_stream.total_out;

			// Inflate another chunk.
			int error_code = inflate(&decompression_stream, Z_SYNC_FLUSH);
			
			if(Z_STREAM_END == error_code)
			{
				decompression_finished = true;
			}

			if(Z_OK != error_code)
			{
				break;
			}
		}

		// 
		if(Z_OK != inflateEnd(&decompression_stream))
		{
			return false;
		}

		// 
		approximate_decompressed_size = decompression_stream.total_out;

		cout << "Finished decompressing." << endl;

		return true;
	}


	void Cube2MapImporter::print_rgb_color_to_stdcout(RGBColor color)
	{
		cout << "rgb(" << color.r << "," << color.g << "," << color.b << ")" << endl;
	}


	bool Cube2MapImporter::parse_map_header()
	{
		cout << "----------------------------------------------------------------------------" << endl;

		cout << "Parsing map header." << endl;

		// TODO: Refactor this!

		// Define the number of bytes from the structure which are filled out.
		#define MAP_OCTA_HEADER_READ_SIZE 7 * sizeof(int)

		// Read most of the map header, but not everything.
		// blendmap, numvars and numvslots will be read later.
		read_memory_into_structure(&map_header, MAP_OCTA_HEADER_READ_SIZE, MAP_OCTA_HEADER_READ_SIZE);

		cout << "Validating map header." << endl;
		
		// Check if magic number "OCTA" is valid.
		if(0 != std::memcmp(map_header.magic, "OCTA", 4))
		{
			// Something is wrong with the header section of this map!
			cout << "Error: Invalid magic number in map header!" << endl;
			return false;
		}
		else
		{
			cout << "Magic number: OCTA" << endl;
		}
		
		cout << "Worldsize: " << map_header.worldsize << endl;
		
		// Check if world size is valid.
		if(map_header.worldsize <= 0)
		{
			cout << "Error: Invalid world size in map header!" << endl;
			return false;
		}
		
		cout << "Entities: " << map_header.number_of_entities << endl;

		// Check if number of entites is valid.
		// Please note that a map can have 0 entities though!
		if(map_header.number_of_entities < 0)
		{
			cout << "Error: Invalid amount of entites!" << endl;
			return false;
		}

		cout << "Mapversion: " << map_header.version << endl;

		// Check if the mapversion is valid.
		if(map_header.version > CUBE2_MAP_IMPORTER_HIGHEST_SUPPORTED_MAPVERSION)
		{
			cout << "This map requires a newer version of the map loader!" << endl;
			return false;
		}

		// In older versions of Cube2 the map byte layout was different.
		// Support older map formats as well!
		if(map_header.version <= 28)
		{
			// Read every byte manually.
			compatible_map_header.lightprecision = read_int_from_buffer();
			compatible_map_header.lighterror = read_int_from_buffer();
			compatible_map_header.lightlod = read_int_from_buffer();
			compatible_map_header.ambient = read_one_byte_from_buffer();
			compatible_map_header.watercolour[0] = read_one_byte_from_buffer();
			compatible_map_header.watercolour[1] = read_one_byte_from_buffer();
			compatible_map_header.watercolour[2] = read_one_byte_from_buffer();
			compatible_map_header.blendmap = read_one_byte_from_buffer();
			compatible_map_header.lerpangle = read_one_byte_from_buffer();
			compatible_map_header.lerpsubdiv = read_one_byte_from_buffer();
			compatible_map_header.lerpsubdivsize = read_one_byte_from_buffer();
			compatible_map_header.bumperror = read_one_byte_from_buffer();
			compatible_map_header.skylight[0] = read_one_byte_from_buffer();
			compatible_map_header.skylight[1] = read_one_byte_from_buffer();
			compatible_map_header.skylight[2] = read_one_byte_from_buffer();
			compatible_map_header.lavacolour[0] = read_one_byte_from_buffer();
			compatible_map_header.lavacolour[1] = read_one_byte_from_buffer();
			compatible_map_header.lavacolour[2] = read_one_byte_from_buffer();
			compatible_map_header.waterfallcolour[0] = read_one_byte_from_buffer();
			compatible_map_header.waterfallcolour[1] = read_one_byte_from_buffer();
			compatible_map_header.waterfallcolour[2] = read_one_byte_from_buffer();

			for(std::size_t i=0; i<10; i++)
			{
				compatible_map_header.reserved[i] = read_one_byte_from_buffer();
			}

			for(std::size_t i=0; i<128; i++)
			{
				compatible_map_header.maptitle[i] = read_one_byte_from_buffer();
			}
		}
		else
		{
			// Looks like the change the arrangement of bytes in map version 30.
			int extra = 0;
			if(map_header.version <= 29) extra++;

			cout << "Extra bytes: " << extra << endl;
				
			// Size of the rest of the bytes to read.
			#define BLENDMAP_BYTE_SIZE sizeof(map_header) - (7+extra)*sizeof(int)

			// Read additional bytes for blend maps.
			// Important note: We read the memory into map_header, not compatible_map_header!
			read_memory_into_structure(&map_header.blendmap, BLENDMAP_BYTE_SIZE, BLENDMAP_BYTE_SIZE);
		}


		if(map_header.version <= 28)
		{
			if(compatible_map_header.lightprecision)
			{
				cout << "Light precision: " << static_cast<int>(compatible_map_header.lightprecision) << endl;
			}
				
			if(compatible_map_header.lighterror)
			{
				cout << "Light error: " << static_cast<int>(compatible_map_header.lighterror) << endl;
			}
        
			if(compatible_map_header.bumperror)
			{
				cout << "Bump error: " << static_cast<int>(compatible_map_header.bumperror) << endl;
			}
        
			if(compatible_map_header.lightlod)
			{
				cout << "Light lod: " << static_cast<int>(compatible_map_header.lightlod) << endl;
			}
        
			if(compatible_map_header.ambient)
			{
				cout << "Ambient: " << static_cast<int>(compatible_map_header.ambient) << endl;
			}
        
			if(compatible_map_header.skylight)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(compatible_map_header.skylight);

				// Print color as RGB value.
				cout << "Skylight: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(compatible_map_header.watercolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(compatible_map_header.watercolour);

				// Print color as RGB value.
				cout << "Watercolor: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(compatible_map_header.waterfallcolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(compatible_map_header.waterfallcolour);
				
				// Print color as RGB value.
				cout << "Waterfallcolour: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(compatible_map_header.lavacolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(compatible_map_header.lavacolour);

				// Print color as RGB value.
				cout << "Lavacolour: ";
				print_rgb_color_to_stdcout(output_color);
			}
				
			if(compatible_map_header.lerpangle)
			{
				cout << "Lerpangle: " << static_cast<int>(compatible_map_header.lerpangle) << endl;
			}
        
			if(compatible_map_header.lerpsubdiv)
			{
				cout << "Lerpsubdiv: " << static_cast<int>(compatible_map_header.lerpsubdiv) << endl;
			}

			if(compatible_map_header.lerpsubdivsize)
			{
				cout << "Lerpsubdivsize: " << static_cast<int>(compatible_map_header.lerpsubdivsize) << endl;
			}
			
			
			if(std::strlen(compatible_map_header.maptitle) > 0)
			{
				// Store the name of the map.
				map_title = compatible_map_header.maptitle;

				// Remove Cube2 coloring bytes from map title.
				clean_map_title = remove_cube2_texture_coloring_bytes(map_title);

				// Print info message about maptitle
				cout << "Maptitle: " << map_title.c_str() << endl;

				if(clean_map_title.find("Misan") != string::npos)
				{
					cout << "Misan found!" << endl;
				}
			}

			if(map_header.blendmap)
			{
				cout << "Blendmap: " << map_header.blendmap << endl;

				map_header.blendmap = compatible_map_header.blendmap;
			}

			map_header.number_of_map_variables = 0;

			map_header.number_of_vslots = 0;
		}
		else if(map_header.version <= 29)
		{
			map_header.number_of_vslots = 0;
		}

		cout << "World size: " << map_header.worldsize << endl;

		// Calculate the world scale.
		while(1 << world_scale < map_header.worldsize)
		{
			world_scale++;
		}
		
		cout << "World scale: " << world_scale << endl;

		return true;
	}


	bool Cube2MapImporter::is_integer_map_variable_a_color_value(const std::string& map_variable_name)
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
				// Yes.
				return true;
			}
		}

		return false;
	}


	void Cube2MapImporter::convert_integer_value_to_rgb_code(const int& integer_value, RGBColor& color_value)
	{
		// Convert integer to RGB by shifting bits around.
		color_value.r = (integer_value & 0x00FF0000) >> 16;
		color_value.g = (integer_value & 0x0000FF00) >> 8;
		color_value.b = (integer_value & 0x000000FF) >> 0;
	}

	
	RGBColor Cube2MapImporter::convert_3_bytes_to_rgb(const unsigned char* components)
	{
		RGBColor return_value;

		// Convert to RGB by shifting bits.
		return_value.r = static_cast<int>(components[0]) << 16;
		return_value.r = static_cast<int>(components[1]) << 8;
		return_value.r = static_cast<int>(components[2]);

		return return_value;
	}


	bool Cube2MapImporter::parse_map_variables()
	{
		cout << "Map variables: " << map_header.number_of_map_variables << endl;

		// Check if we read something wrong!
		if(map_header.number_of_map_variables > 0)
		{
			cout << "----------------------------------------------------------------------------" << endl;
			
			// Loop through all variables and load them.
			for(int i=0; i<map_header.number_of_map_variables; i++)
			{
				// The new variable which is being read.
				MapVariable new_map_variable;
				
				// Which type of variable are we expecting?
				int var_type = read_one_byte_from_buffer();

				// The length of the string.
				int var_length = read_short_from_buffer();

				// Read the name out of the buffer.
				new_map_variable.name = read_slice_from_buffer_as_string(var_length);

				// Print out which map variable is being read.
				cout << new_map_variable.name << ": ";

				// Parse the map variable data depending on the memory type.
				switch(var_type)
				{
					case ID_VAR:
					{
						// Set this as an integer variable.
						new_map_variable.type = ID_VAR;

						// Read integer from buffer.
						new_map_variable.value_int = read_int_from_buffer();

						// Check if this is the name of the variable is
						// one of the list mentioned above. If so, calculate
						// the rgb values from the integer values.
						// Example: ? = rgb(244,2,234);
						if(is_integer_map_variable_a_color_value(new_map_variable.name))
						{
							// This is a color!
							new_map_variable.type = ID_COLORVAR;

							// TODO: Is there a better way to do this?
							convert_integer_value_to_rgb_code(new_map_variable.value_int, new_map_variable.value_color);
							
							// Print color as RGB value.
							print_rgb_color_to_stdcout(new_map_variable.value_color);
						}
						else
						{
							cout << new_map_variable.value_int << endl;
						}

						break;
					}
					case ID_FVAR:
					{
						// Set this as a float variable.
						new_map_variable.type = ID_FVAR;

						// Read float from buffer.
						new_map_variable.value_float = read_float_from_buffer();
								
						// Print the value of the data.
						cout << new_map_variable.value_float << endl;
						break;
					}
					case ID_SVAR:
					{
						// Check the length of the string.
						int var_str_length = read_short_from_buffer();

						// Read the length of the buffer.
						std::string var_str_value = read_slice_from_buffer_as_string(var_str_length);
								
						// Copy string value.
						new_map_variable.value_string = var_str_value;
							
						// This is the maptitle!
						// Since the map title can be formatted with Cube2
						// console coloring bytes, we have to remove them.
						if(0 == new_map_variable.name.compare("maptitle"))
						{
							// Store the name of the map.
							map_title = new_map_variable.value_string;

							// Remove Cube2 coloring bytes from map title.
							clean_map_title = remove_cube2_texture_coloring_bytes(map_title);

							// Print cleaned map title.
							cout << clean_map_title.c_str() << endl;

							if(clean_map_title.find("Misan") != string::npos)
							{
								cout << "Misan found!" << endl;
							}

						}
						else
						{
							// Print the value of the data.
							cout << new_map_variable.value_string.c_str() << endl;
						}
						break;
					}
				}
					
				// TODO: Do not use push_back, but allocate memory ahead of time.

				// We are done parsing the data.
				// Now add it to the vector of map variables.
				map_variables.push_back(new_map_variable);
			}

			// Check if the number of variables to read is valid.
			if(map_header.number_of_map_variables > MAX_NUMVARS)
			{
				// Something went wrong when parsing the number of variables in the map.
				cout << "Warning: more than " << MAX_NUMVARS << " to read (" << map_header.number_of_map_variables << ")!" << endl;
				cout << "Cube2: Sauerbraten would have aborted reading after " << MAX_NUMVARS << " variables!" << endl;
			}
			
			cout << "----------------------------------------------------------------------------" << endl;
		}

		return true;
	}


	bool Cube2MapImporter::parse_game_name()
	{
		// Check the game name.
		if(map_header.version >= 16)
		{
			// The length of game name.
			int game_name_length = read_one_byte_from_buffer();

			// Read the name of the game.
			game_name = read_slice_from_buffer_as_string(game_name_length);

			// We need to read 1 more byte since the string is null-terminates.
			skip_reading_buffer_bytes(1);

			cout << "game name: " << game_name.c_str() << endl;

			// For which game was this made?
			if(0 == game_name.compare("fps"))
			{
				// A Cube2: Sauerbraten map!
				map_for_cube2_game = true;
			}
			else
			{
				// Eisenstern maybe?
				map_for_cube2_game = false;

				if(0 == game_name.compare("rpg"))
				{
					// Yes, Eisenstern!
					map_for_eisenstern_rpg = true;
				}
				else
				{
					// No, some other crazy project.
					map_for_eisenstern_rpg = false;

					// Well, this is some other map format then?
					cout << "Warning: This is not a Cube2 map and not an Eisenstern map either!" << endl;
				}
			}
		}

		return true;
	}


	bool Cube2MapImporter::parse_extra_game_info()
	{
		// Since the data layout has changed over time,
		// we need to support all other versions as well.
		int extra_game_info_size = 0;
			
		// In Cube map format version 16 they introduces a specification for
		// the game since they were working on Eisenstern..
		if(map_header.version >= 16)
		{
			// Read the size of the additional info.
			extra_game_info_size = read_unsigned_short_from_buffer();

			cout << "extra info size: " << extra_game_info_size << endl;

			// Additional information.
			int how_many_bytes_to_read  = read_unsigned_short_from_buffer();
        
			cout << "bytes to read: " << how_many_bytes_to_read << endl;
				
			if(how_many_bytes_to_read > 0)
			{
				// Read additional information.
				extra_game_information = read_slice_from_buffer(how_many_bytes_to_read);
			}
		}

		return true;
	}


	bool Cube2MapImporter::parse_texture_cache()
	{
		cout << "Loading texture cache." << endl;

		if(map_header.version < 14)
		{
			cout << "Loading 256 texture MRU bytes." << endl;

			// Pre-allocate the required memory.
			texture_mru.resize(256);

			// Load new texture format.
			for(std::size_t i=0; i<256; i++)
			{
				// Do not use push_back, since memory is already allocated.
				texture_mru[i] = read_unsigned_short_from_buffer();
			}
		}
		else
		{
			// In map format version greater or equal 14
			// we can have as many texture levels as we want.
			unsigned short how_many_texture_levels = read_unsigned_short_from_buffer();

			cout << "Loading " << how_many_texture_levels << " texture MRU bytes." << endl;
				
			// Allocate memory ahead of time.
			texture_mru.resize(how_many_texture_levels);
				
			if(how_many_texture_levels > 0)
			{
				// Load new texture format.
				for(std::size_t i=0; i<how_many_texture_levels; i++)
				{
					// Do not use push_back, since memory is already allocated.
					texture_mru[i] = read_unsigned_short_from_buffer();
				}
			}
		}

		cout << "Loaded " << texture_mru.size() << " texture MRU bytes." << endl;
			
		return true;
	}
	   	 
	
	void Cube2MapImporter::fix_entity_data(Entity &e, const int version)
	{
		if(version <= 10 && e.type >= 7)
		{
			e.type++;
		}
		if(version <= 12 && e.type >= 8)
		{
			e.type++;
		}
		if(version <= 14 && e.type >= ET_MAPMODEL && e.type <= 16)
		{
			if(e.type == 16)
			{
				e.type = ET_MAPMODEL;
			}
			else
			{
				e.type++;
			}
		}
		if(version <= 20 && e.type >= ET_ENVMAP)
		{
			e.type++;
		}
		if(version <= 21 && e.type >= ET_PARTICLES)
		{
			e.type++;
		}
		if(version <= 22 && e.type >= ET_SOUND)
		{
			e.type++;
		}
		if(version <= 23 && e.type >= ET_SPOTLIGHT)
		{
			e.type++;
		}
		if(version <= 30 && (e.type == ET_MAPMODEL || e.type == ET_PLAYERSTART))
		{
			e.attr1 = (int(e.attr1)+180)%360;
		}
		if(version <= 31 && e.type == ET_MAPMODEL)
		{
			int yaw = (int(e.attr1)%360 + 360)%360 + 7;
			e.attr1 = yaw - yaw%15;
		}
	}

	
	bool Cube2MapImporter::is_entity_inside_world(const Entity &e) const
	{
		return e.o.x>=0 && e.o.x<map_header.worldsize && e.o.y>=0 && e.o.y<map_header.worldsize && e.o.z>=0 && e.o.z<map_header.worldsize;
	}


	bool Cube2MapImporter::parse_entites()
	{
		cout << "Loading entites." << endl;

		// This may change for other games?
		int extra_entity_info_size = 0;

		// Allocate memory.
		extra_entity_info.resize(extra_entity_info_size);

		// How many entities to load.
		int number_of_entities_to_load = std::min(map_header.number_of_entities, MAXENTS);

		cout << "Entities: " << number_of_entities_to_load << "." << endl;

		if(number_of_entities_to_load > 0)
		{
			// Load all entites from the map.
			for(int i=0; i<number_of_entities_to_load; i++)
			{
				// A new entity to load.
				ExtendedEntity new_entity;

				// Fill out entity structure.
				read_memory_into_structure(&new_entity, sizeof(Entity), sizeof(Entity));

				// Set the data of the entity.
				new_entity.spawned = false;
				new_entity.inoctanode = false;

				// Fix the entity's data.
				fix_entity_data(new_entity, map_header.version);

				// Yes, this is a Cube2 map.
				if(map_for_cube2_game)
				{
					// Read extra information on entities.
					if(extra_entity_info_size > 0)
					{
						// Warning: Since there is no map which has this
						// data in it for testing, this part of the code
						// remains untested to this day.
						extra_entity_info = read_slice_from_buffer(extra_entity_info_size);
					}

					// Note: extra_entity_info isn't actually used in the game.
					// but we still need to read this from the buffer to have the
					// right read position in the stream.

					// Fix entity data.

					// TODO: Why again????
					fix_entity_data(new_entity, map_header.version);
				}
				else // No, Eisenstern maybe?
				{
					// Not the same game.
					// Skip reading these bytes.
					if(extra_entity_info_size > 0)
					{
						// Move read position forward.
						skip_reading_buffer_bytes(extra_entity_info_size);
					}
					// TODO: Read the extra bytes anyway?
				}

				// Entities should never be outside of the world.
				if(!is_entity_inside_world(new_entity))
				{
					// Lights can be placed outside of the world.
					if(new_entity.type != ET_LIGHT && new_entity.type != ET_SPOTLIGHT)
					{
						cout << "Warning: entity outside of world!" << endl;
					}
					else
					{
						cout << "Info: light is placed outside of game world." << endl;
					}
				}
				
				// There seems to be some problem with the mapmodel texture.
				if(map_header.version <= 14 && new_entity.type == ET_MAPMODEL)
				{
					// But why?
					new_entity.o.z += new_entity.attr3;

					if(new_entity.attr4)
					{
						cout << "Warning: mapmodel ent (index " << i << ") uses texture slot " << new_entity.attr4 << endl;
					}
					
					// But why?
					new_entity.attr3 = new_entity.attr4 = 0;
				}

				// TODO: Reserve memory ahead of time!

				// Add entity to the buffer.
				map_entities.push_back(new_entity);
			}
		}

		cout << "Loaded " << map_entities.size() << " entities." << endl;

		// TODO: Remove this! We should be able to load as many entites as we like!
		// TODO: Does it really work this way? Didn't we already read all the stuff anyways?

		// Check if there are too many entites in the world.
		if(map_header.number_of_entities > MAXENTS) 
		{
			// Do not read the following entites but move the read position
			// in the stream forward anyways.
			int how_many_bytes_to_skip = 0;

			cout << "Warning: map has " << map_header.number_of_entities << " entities!" << endl;
				
			// How many entites are we ignoring?
			int how_many_entites_not_to_read = map_header.number_of_entities - MAXENTS;

			// How many bytes for an entity.
			int byte_size_of_entity_structure = 0;

			// How many byte for one entity?
			if(map_for_cube2_game)
			{
				// TODO: Wait a second.. wat?!
				byte_size_of_entity_structure = sizeof(Entity) + extra_game_info_size;
			}
			else
			{
				// TODO: Wait a second.. wat?!
				byte_size_of_entity_structure = extra_entity_info_size;
			}

			// How many bytes overall not to read?
			how_many_bytes_to_skip = byte_size_of_entity_structure * how_many_entites_not_to_read;

			// Skip the following bytes!
			skip_reading_buffer_bytes(how_many_bytes_to_skip);
		}

		return true;
	}


	bool Cube2MapImporter::parse_vslots()
	{
		cout << "VSlots: " << map_header.number_of_vslots << "." << endl;

		// Initialise a vector of VSlots for preview.
		// Allocate the required memory and fill it with -1.
		// TODO: Check if we should change the vector to int?
		std::vector<int> preview_vslots(map_header.number_of_vslots, -1);
			
		// TODO: reserve memory in advance to minimize allocation.

		// How many VSlots to load?
		// TODO: Check if we should change to int?
		int vslots_to_load = map_header.number_of_vslots;

		// Continue as long as there are VSlots to be loaded.
		while(vslots_to_load > 0)
		{
			// What is this?
			int changed = read_int_from_buffer();

			// What is this?
			if(changed < 0)
			{
				// Use negative value.
				int negative_value = - changed;

				// Note: We use the negative value for iteration.
				for(int i=0; i<negative_value; i++)
				{
					// ?
					VSlot new_vslot(NULL, map_vertex_slots.size());
						
					// Add to vslot buffer.
					map_vertex_slots.push_back(new_vslot);
				}

				// What is this?
				vslots_to_load += changed;
			}
			else
			{
				std::size_t array_index = map_vertex_slots.size();

				// Read size from buffer.
				preview_vslots[array_index] = read_int_from_buffer();
					
				// A new vslot.
				VSlot new_vslot(NULL, map_vertex_slots.size());
					
				// Add to vslot buffer.
				map_vertex_slots.push_back(new_vslot);
				new_vslot.changed = changed;

				// Shader parameter.
				if(new_vslot.changed & (1<<VSLOT_SHPARAM))
				{
					// Read the number of shader parameters.
					std::size_t number_of_shader_parameters = read_unsigned_short_from_buffer();
						
					// Shader parameters?
					for(std::size_t i=0; i<number_of_shader_parameters; i++)
					{
						// A shader parameter.
						ShaderParam new_shader_parameter;
					
						// The length of the name of the shader parameter.
						int shader_param_name_length = read_unsigned_short_from_buffer();

						// Limit the length of the name of the shader parameter.
						shader_param_name_length = std::min(shader_param_name_length, MAXSTRLEN-1);

						// Read the name of the shader paramrt
						new_shader_parameter.name = read_slice_from_buffer_as_string(shader_param_name_length);
						new_shader_parameter.type = SHPARAM_LOOKUP;
						new_shader_parameter.index = -1;
						new_shader_parameter.loc = -1;
											
						// Read float values from buffer.
						for(std::size_t j=0; j<4; j++)
						{
							new_shader_parameter.val[j] = read_float_from_buffer();
						}

						ShaderParam& p = new_shader_parameter;
						
						cout << "Shader parameter " << p.name.c_str() << ": " << p.val[0] << " " << p.val[1] << " " << p.val[2] << " " << p.val[3] << endl;

						// Add new shader parameter to the buffer.
						new_vslot.params.push_back(new_shader_parameter);
					}
				}

				if(new_vslot.changed & (1<<VSLOT_SCALE))
				{
					new_vslot.scale = read_float_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_ROTATION))
				{
					new_vslot.rotation = read_int_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_OFFSET))
				{
					new_vslot.xoffset = read_int_from_buffer();
					new_vslot.yoffset = read_int_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_SCROLL))
				{
					new_vslot.scrollS = read_float_from_buffer();
					new_vslot.scrollT = read_float_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_LAYER))
				{
					new_vslot.layer = read_int_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_ALPHA))
				{
					new_vslot.alphafront = read_float_from_buffer();
					new_vslot.alphaback = read_float_from_buffer();
				}

				if(new_vslot.changed & (1<<VSLOT_COLOR)) 
				{
					for(std::size_t k=0; k<3; k++)
					{
						new_vslot.colorscale[k] = read_float_from_buffer();
					}
				}

				vslots_to_load --;
			}
		}

		for(std::size_t i=0; i<map_vertex_slots.size(); i++)
		{
			// TODO: Fix signed/unsigned warning!
			if(preview_vslots[i] <= map_vertex_slots.size())
			{
				map_vertex_slots[preview_vslots[i]].next = &map_vertex_slots[i];
			}
		}

		// Check for errors.
		if(map_vertex_slots.size() != map_header.number_of_vslots)
		{
			cout << "Error: Loading vslots failed!" << endl;
			return false;
		}
		else
		{
			cout << "Loaded " << map_vertex_slots.size() << " vslots." << endl;
		}

		return true;
	}


	static inline int convertoldmaterial(int mat)
	{
		return ((mat&7)<<MATF_VOLUME_SHIFT) | (((mat>>3)&3)<<MATF_CLIP_SHIFT) | (((mat>>5)&7)<<MATF_FLAG_SHIFT);
	}

	#define edgemake(a, b) ((b)<<4|a)
	#define edgeget(edge, coord) ((coord) ? (edge)>>4 : (edge)&0xF)
	#define edgeset(edge, coord, val) ((edge) = ((coord) ? ((edge)&0xF)|((val)<<4) : ((edge)&0xF0)|(val)))
	#define cubeedge(c, d, x, y) ((c).edges[(((d)<<2)+((y)<<1)+(x))])


	static inline int edgeval(cube &c, const ivec &p, int dim, int coord)
	{
		return edgeget(cubeedge(c, dim, p[R[dim]]>>3, p[C[dim]]>>3), coord);
	}


	void genvertp(cube &c, ivec &p1, ivec &p2, ivec &p3, plane &pl, bool solid = false)
	{
		int dim = 0;
		if(p1.y==p2.y && p2.y==p3.y) dim = 1;
		else if(p1.z==p2.z && p2.z==p3.z) dim = 2;

		int coord = p1[dim];
		ivec v1(p1), v2(p2), v3(p3);
		v1[dim] = solid ? coord*8 : edgeval(c, p1, dim, coord);
		v2[dim] = solid ? coord*8 : edgeval(c, p2, dim, coord);
		v3[dim] = solid ? coord*8 : edgeval(c, p3, dim, coord);

		pl.toplane(v1.tovec(), v2.tovec(), v3.tovec());
	}


	static void genedgespanvert(ivec &p, cube &c, vec &v)
	{
		ivec p1(8-p.x, p.y, p.z);
		ivec p2(p.x, 8-p.y, p.z);
		ivec p3(p.x, p.y, 8-p.z);

		plane plane1, plane2, plane3;
		genvertp(c, p, p1, p2, plane1);
		genvertp(c, p, p2, p3, plane2);
		genvertp(c, p, p3, p1, plane3);

		if(plane1==plane2) genvertp(c, p, p1, p2, plane1, true);
		if(plane1==plane3) genvertp(c, p, p1, p2, plane1, true);
		if(plane2==plane3) genvertp(c, p, p2, p3, plane2, true);

		v.x = max(0.0f, min(8.0f, v.x));
		v.y = max(0.0f, min(8.0f, v.y));
		v.z = max(0.0f, min(8.0f, v.z));

	}


	#define isempty(c) ((c).faces[0]==F_EMPTY)
	#define isentirelysolid(c) ((c).faces[0]==F_SOLID && (c).faces[1]==F_SOLID && (c).faces[2]==F_SOLID)

	#define loop(v,m) for(int v = 0; v<int(m); v++)
	#define loopi(m) loop(i,m)
	#define loopj(m) loop(j,m)
	#define loopk(m) loop(k,m)
	#define loopl(m) loop(l,m)

	void Cube2MapImporter::edgespan2vectorcube(cube &c)
	{
		if(isentirelysolid(c) || isempty(c)) return;
		cube o = c;
		loop(x, 2) loop(y, 2) loop(z, 2)
		{
			ivec p(8*x, 8*y, 8*z);
			vec v;
			genedgespanvert(p, o, v);

			edgeset(cubeedge(c, 0, y, z), x, int(v.x+0.49f));
			edgeset(cubeedge(c, 1, z, x), y, int(v.y+0.49f));
			edgeset(cubeedge(c, 2, x, y), z, int(v.z+0.49f));
		}
	}

	#define GENFACEVERTX(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSX(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(0, GENFACEVERTX(0,0, x0,y1,z1, d0,r1,c1), GENFACEVERTX(0,1, x0,y1,z0, d0,r1,c0), GENFACEVERTX(0,2, x0,y0,z0, d0,r0,c0), GENFACEVERTX(0,3, x0,y0,z1, d0,r0,c1)) \
		GENFACEORIENT(1, GENFACEVERTX(1,0, x1,y1,z1, d1,r1,c1), GENFACEVERTX(1,1, x1,y0,z1, d1,r0,c1), GENFACEVERTX(1,2, x1,y0,z0, d1,r0,c0), GENFACEVERTX(1,3, x1,y1,z0, d1,r1,c0))
	#define GENFACEVERTY(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(2, GENFACEVERTY(2,0, x1,y0,z1, c1,d0,r1), GENFACEVERTY(2,1, x0,y0,z1, c0,d0,r1), GENFACEVERTY(2,2, x0,y0,z0, c0,d0,r0), GENFACEVERTY(2,3, x1,y0,z0, c1,d0,r0)) \
		GENFACEORIENT(3, GENFACEVERTY(3,0, x0,y1,z0, c0,d1,r0), GENFACEVERTY(3,1, x0,y1,z1, c0,d1,r1), GENFACEVERTY(3,2, x1,y1,z1, c1,d1,r1), GENFACEVERTY(3,3, x1,y1,z0, c1,d1,r0))
	#define GENFACEVERTZ(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSZ(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(4, GENFACEVERTZ(4,0, x0,y0,z0, r0,c0,d0), GENFACEVERTZ(4,1, x0,y1,z0, r0,c1,d0), GENFACEVERTZ(4,2, x1,y1,z0, r1,c1,d0), GENFACEVERTZ(4,3, x1,y0,z0, r1,c0,d0)) \
		GENFACEORIENT(5, GENFACEVERTZ(5,0, x0,y0,z1, r0,c0,d1), GENFACEVERTZ(5,1, x1,y0,z1, r1,c0,d1), GENFACEVERTZ(5,2, x1,y1,z1, r1,c1,d1), GENFACEVERTZ(5,3, x0,y1,z1, r0,c1,d1))
	#define GENFACEVERTSXY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSX(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1)
	#define GENFACEVERTS(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSXY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSZ(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1)

	void genfaceverts(const cube &c, int orient, ivec v[4])
	{

		switch(orient)
		{
	#define GENFACEORIENT(o, v0, v1, v2, v3) \
			case o: v0 v1 v2 v3 break;
	#define GENFACEVERT(o, n, x,y,z, xv,yv,zv) \
				v[n] = ivec(edgeget(cubeedge(c, 0, y, z), x), \
							edgeget(cubeedge(c, 1, z, x), y), \
							edgeget(cubeedge(c, 2, x, y), z));
			GENFACEVERTS(0, 1, 0, 1, 0, 1, , , , , , )
		#undef GENFACEORIENT
		#undef GENFACEVERT
		}
	}


	cubeext *Cube2MapImporter::growcubeext(cubeext *old, int maxverts)
	{
		// TODO: Refactor!
		cubeext *ext = (cubeext *)new uchar[sizeof(cubeext) + maxverts*sizeof(vertinfo)];

		if(old)
		{
			ext->va = old->va;
			ext->ents = old->ents;
			ext->tjoints = old->tjoints;
		}
		else
		{
			ext->va = NULL;
			ext->ents = NULL;
			ext->tjoints = -1;
		}

		ext->maxverts = maxverts;
		
		return ext;
	}


	void Cube2MapImporter::setcubeext(cube &c, cubeext *ext)
	{
		cubeext *old = c.ext;
		if(old == ext) return;
		c.ext = ext;
		if(old) delete[] (uchar *)old;
	}


	cubeext * Cube2MapImporter::newcubeext(cube &c, int maxverts, bool init)
	{
		if(c.ext && c.ext->maxverts >= maxverts) return c.ext;
		cubeext *ext = growcubeext(c.ext, maxverts);
		if(init)
		{
			if(c.ext)
			{
				memcpy(ext->surfaces, c.ext->surfaces, sizeof(ext->surfaces));
				memcpy(ext->verts(), c.ext->verts(), c.ext->maxverts*sizeof(vertinfo));
			}
			else memset(ext->surfaces, 0, sizeof(ext->surfaces)); 
		}
		setcubeext(c, ext);
		return ext;
	}


	ushort encodenormal(const vec &n)
	{               
		if(n.iszero()) return 0;
		int yaw = int(-atan2(n.x, n.y)/RAD), pitch = int(asin(n.z)/RAD);
		return ushort(clamp(pitch + 90, 0, 180)*360 + (yaw < 0 ? yaw%360 + 360 : yaw%360) + 1);
	}


	cubeext *growcubeext(cubeext *old, int maxverts)
	{
		cubeext *ext = (cubeext *)new uchar[sizeof(cubeext) + maxverts*sizeof(vertinfo)];
		if(old)
		{
			ext->va = old->va;
			ext->ents = old->ents;
			ext->tjoints = old->tjoints;
		}
		else
		{
			ext->va = NULL;
			ext->ents = NULL;
			ext->tjoints = -1;
		}
		ext->maxverts = maxverts;
		return ext;
	}


	void setcubeext(cube &c, cubeext *ext)
	{
		cubeext *old = c.ext;
		if(old == ext) return;
		c.ext = ext;
		if(old) delete[] (uchar *)old;
	}


	cubeext *newcubeext(cube &c, int maxverts, bool init)
	{
		if(c.ext && c.ext->maxverts >= maxverts) return c.ext;
		cubeext *ext = growcubeext(c.ext, maxverts);
		if(init)
		{
			if(c.ext)
			{
				memcpy(ext->surfaces, c.ext->surfaces, sizeof(ext->surfaces));
				memcpy(ext->verts(), c.ext->verts(), c.ext->maxverts*sizeof(vertinfo));
			}
			else memset(ext->surfaces, 0, sizeof(ext->surfaces)); 
		}
		setcubeext(c, ext);
		return ext;
	}


	void setsurfaces(cube &c, const surfaceinfo *surfs, const vertinfo *verts, int numverts)
	{
		if(!c.ext || c.ext->maxverts < numverts) newcubeext(c, numverts, false);
		memcpy(c.ext->surfaces, surfs, sizeof(c.ext->surfaces));
		memcpy(c.ext->verts(), verts, numverts*sizeof(vertinfo));
	}


	void convertoldsurfaces(cube &c, const ivec &co, int size, surfacecompat *srcsurfs, int hassurfs, normalscompat *normals, int hasnorms, mergecompat *merges, int hasmerges)
	{
		surfaceinfo dstsurfs[6];
		vertinfo verts[6*2*MAXFACEVERTS];
		int totalverts = 0, numsurfs = 6;
		memset(dstsurfs, 0, sizeof(dstsurfs));
		loopi(6) if((hassurfs|hasnorms|hasmerges)&(1<<i))
		{
			surfaceinfo &dst = dstsurfs[i];
			vertinfo *curverts = NULL;
			int numverts = 0;
			surfacecompat *src = NULL, *blend = NULL;
			if(hassurfs&(1<<i))
			{
				src = &srcsurfs[i];
				if(src->layer&2) 
				{ 
					blend = &srcsurfs[numsurfs++];
					dst.lmid[0] = src->lmid;
					dst.lmid[1] = blend->lmid;
					dst.numverts |= LAYER_BLEND;
					if(blend->lmid >= LMID_RESERVED && (src->x != blend->x || src->y != blend->y || src->w != blend->w || src->h != blend->h || memcmp(src->texcoords, blend->texcoords, sizeof(src->texcoords))))
						dst.numverts |= LAYER_DUP;
				}
				else if(src->layer == 1) { dst.lmid[1] = src->lmid; dst.numverts |= LAYER_BOTTOM; }
				else { dst.lmid[0] = src->lmid; dst.numverts |= LAYER_TOP; } 
			}
			else dst.numverts |= LAYER_TOP;
			bool uselms = hassurfs&(1<<i) && (dst.lmid[0] >= LMID_RESERVED || dst.lmid[1] >= LMID_RESERVED || dst.numverts&~LAYER_TOP),
				 usemerges = hasmerges&(1<<i) && merges[i].u1 < merges[i].u2 && merges[i].v1 < merges[i].v2,
				 usenorms = hasnorms&(1<<i) && normals[i].normals[0] != bvec(128, 128, 128);
			if(uselms || usemerges || usenorms)
			{
				ivec v[4], pos[4], e1, e2, e3, n, vo = ivec(co).mask(0xFFF).shl(3);
				genfaceverts(c, i, v); 
				n.cross((e1 = v[1]).sub(v[0]), (e2 = v[2]).sub(v[0]));
				if(usemerges)
				{
					const mergecompat &m = merges[i];
					
					int offset = -n.dot(v[0].mul(size).add(vo));
					int dim = ((i)>>1);
					int vc = C[dim];
					int vr = R[dim];
					
					loopk(4)
					{
						const ivec &coords = facecoords[i][k];
						int cc = coords[vc] ? m.u2 : m.u1,
							rc = coords[vr] ? m.v2 : m.v1,
							dc = -(offset + n[vc]*cc + n[vr]*rc)/n[dim];
						ivec &mv = pos[k];
						mv[vc] = cc;
						mv[vr] = rc;
						mv[dim] = dc;
					}
				}
				else
				{
					int convex = (e3 = v[0]).sub(v[3]).dot(n), vis = 3;
					if(!convex)
					{
						if(ivec().cross(e3, e2).iszero()) { if(!n.iszero()) vis = 1; } 
						else if(n.iszero()) vis = 2;
					}
					int order = convex < 0 ? 1 : 0;
					pos[0] = v[order].mul(size).add(vo);
					pos[1] = vis&1 ? v[order+1].mul(size).add(vo) : pos[0];
					pos[2] = v[order+2].mul(size).add(vo);
					pos[3] = vis&2 ? v[(order+3)&3].mul(size).add(vo) : pos[0];
				}
				curverts = verts + totalverts;
				loopk(4)
				{
					if(k > 0 && (pos[k] == pos[0] || pos[k] == pos[k-1])) continue;
					vertinfo &dv = curverts[numverts++];
					dv.setxyz(pos[k]);
					if(uselms)
					{
						float u = src->x + (src->texcoords[k*2] / 255.0f) * (src->w - 1),
							  v = src->y + (src->texcoords[k*2+1] / 255.0f) * (src->h - 1);
						dv.u = ushort(floor(clamp((u) * float(USHRT_MAX+1)/LM_PACKW + 0.5f, 0.0f, float(USHRT_MAX))));
						dv.v = ushort(floor(clamp((v) * float(USHRT_MAX+1)/LM_PACKH + 0.5f, 0.0f, float(USHRT_MAX))));
					}
					else dv.u = dv.v = 0;
					dv.norm = usenorms && normals[i].normals[k] != bvec(128, 128, 128) ? encodenormal(normals[i].normals[k].tovec().normalize()) : 0;
				}
				dst.verts = totalverts;
				dst.numverts |= numverts;
				totalverts += numverts;
				if(dst.numverts&LAYER_DUP) loopk(4)
				{
					if(k > 0 && (pos[k] == pos[0] || pos[k] == pos[k-1])) continue;
					vertinfo &bv = verts[totalverts++];
					bv.setxyz(pos[k]);
					bv.u = ushort(floor(clamp((blend->x + (blend->texcoords[k*2] / 255.0f) * (blend->w - 1)) * float(USHRT_MAX+1)/LM_PACKW, 0.0f, float(USHRT_MAX))));
					bv.v = ushort(floor(clamp((blend->y + (blend->texcoords[k*2+1] / 255.0f) * (blend->h - 1)) * float(USHRT_MAX+1)/LM_PACKH, 0.0f, float(USHRT_MAX))));
					bv.norm = usenorms && normals[i].normals[k] != bvec(128, 128, 128) ? encodenormal(normals[i].normals[k].tovec().normalize()) : 0;
				}
			}    
		}
		setsurfaces(c, dstsurfs, verts, totalverts);
	}


	void Cube2MapImporter::loadc(const std::shared_ptr<cube>& c, const ivec &co, int size, bool &failed)
	{
		bool haschildren = false;
    
		// ?
		int octsav = read_one_byte_from_buffer();

		cout << "octsav:" << octsav << endl;

		switch(octsav&0x7)
		{
			case OCTSAV_CHILDREN:
			{
				c->children = load_octree_children(co, size>>1, failed);
				return;
			}
			case OCTSAV_LODCUBE:
			{
				haschildren = true;
				break;
			}
			case OCTSAV_EMPTY:
			{
				c->faces[0] = c->faces[1] = c->faces[2] = F_EMPTY;
				break;
			}
			case OCTSAV_SOLID:
			{
				c->faces[0] = c->faces[1] = c->faces[2] = F_SOLID;
				break;
			}
			case OCTSAV_NORMAL:
			{
				read_memory_into_structure(&c->edges[0], 12, sizeof(c->edges));
				break;
			}
			default:
			{
				failed = true;
				return;
			}
		}
		
		for(std::size_t i=0; i<6; i++)
		{
			if(map_header.version < 14)
			{
				c->texture[i] = read_one_byte_from_buffer();
			}
			else
			{
				c->texture[i] = read_unsigned_short_from_buffer();
			}
		}

		if(map_header.version < 7)
		{
			skip_reading_buffer_bytes(3);
		}
		else if(map_header.version <= 31)
		{
			uchar mask = read_one_byte_from_buffer();

			if(mask & 0x80) 
			{
				int mat = read_one_byte_from_buffer();

				if(map_header.version < 27)
				{
					static const ushort matconv[] = {
						MAT_AIR,
						MAT_WATER,
						MAT_CLIP,
						MAT_GLASS|MAT_CLIP,
						MAT_NOCLIP,
						MAT_LAVA|MAT_DEATH,
						MAT_GAMECLIP, MAT_DEATH
					};

					c->material = size_t(mat) < sizeof(matconv)/sizeof(matconv[0]) ? matconv[mat] : MAT_AIR;
				}
				else c->material = convertoldmaterial(mat);
			}
			
			surfacecompat surfaces[12];
			
			normalscompat normals[6];
			
			mergecompat merges[6];

			int hassurfs = 0;

			int hasnorms = 0;
			
			int hasmerges = 0;

			if(mask & 0x3F)
			{
				int numsurfs = 6;

				for(int i=0; i<numsurfs; i++)
				{
					if(i >= 6 || mask & (1 << i))
					{
						read_memory_into_structure(&surfaces[i], sizeof(surfacecompat), sizeof(surfacecompat));

						if(map_header.version < 10)
						{
							++surfaces[i].lmid;
						}
						if(map_header.version < 18)
						{
							if(surfaces[i].lmid >= LMID_AMBIENT1) ++surfaces[i].lmid;
							if(surfaces[i].lmid >= LMID_BRIGHT1) ++surfaces[i].lmid;
						}
						if(map_header.version < 19)
						{
							if(surfaces[i].lmid >= LMID_DARK) surfaces[i].lmid += 2;
						}
						if(i < 6)
						{
							if(mask & 0x40)
							{
								hasnorms |= 1<<i;
								read_memory_into_structure(&normals[i], sizeof(normalscompat), sizeof(normalscompat));
							}
							if(surfaces[i].layer != 0 || surfaces[i].lmid != LMID_AMBIENT) 
								hassurfs |= 1<<i;
							if(surfaces[i].layer&2) numsurfs++;
						}
					}
				}
			}

			if(map_header.version <= 8)
			{
				edgespan2vectorcube(*c);
			}

			if(map_header.version <= 11)
			{
				swap(c->faces[0], c->faces[2]);
				swap(c->texture[0], c->texture[4]);
				swap(c->texture[1], c->texture[5]);
				if(hassurfs&0x33)
				{
					swap(surfaces[0], surfaces[4]);
					swap(surfaces[1], surfaces[5]);
					hassurfs = (hassurfs&~0x33) | ((hassurfs&0x30)>>4) | ((hassurfs&0x03)<<4);
				}
			}

			if(map_header.version >= 20)
			{
				if(octsav&0x80)
				{
					int merged = read_one_byte_from_buffer();
					c->merged = merged&0x3F;
					if(merged&0x80)
					{
						int mask = read_one_byte_from_buffer();
						if(mask)
						{
							hasmerges = mask&0x3F;
							
							for(std::size_t i=0; i<6; i++)
							{
								if(mask&(1<<i))
								{
									mergecompat *m = &merges[i];
									
									read_memory_into_structure(&m, sizeof(mergecompat), sizeof(mergecompat));
									
									if(map_header.version <= 25)
									{
										int uorigin = m->u1 & 0xE000, vorigin = m->v1 & 0xE000;
										m->u1 = (m->u1 - uorigin) << 2;
										m->u2 = (m->u2 - uorigin) << 2;
										m->v1 = (m->v1 - vorigin) << 2;
										m->v2 = (m->v2 - vorigin) << 2;
									}
								}
							}
						}
					}
				}    
			}

			if(hassurfs || hasnorms || hasmerges)
			{
				convertoldsurfaces(*c, co, size, surfaces, hassurfs, normals, hasnorms, merges, hasmerges);
			}
		}
		else
		{
			if(octsav&0x40) 
			{
				if(map_header.version <= 32)
				{
					int mat = read_one_byte_from_buffer();
					c->material = convertoldmaterial(mat);
				}
				else c->material = read_unsigned_short_from_buffer();
			}

			if(octsav&0x80)
			{
				c->merged = read_one_byte_from_buffer();
			}

			if(octsav&0x20)
			{
				int surfmask, totalverts;
				surfmask = read_one_byte_from_buffer();
				totalverts = read_one_byte_from_buffer();
				
				newcubeext(*c, totalverts, false);

				memset(c->ext->surfaces, 0, sizeof(c->ext->surfaces));
				memset(c->ext->verts(), 0, totalverts*sizeof(vertinfo));
				int offset = 0;

				for(std::size_t i=0; i<6; i++)
				{
					if(surfmask&(1<<i)) 
					{
						surfaceinfo &surf = c->ext->surfaces[i];
						
						read_memory_into_structure(&surf, sizeof(surfacecompat), sizeof(surfacecompat));

						int vertmask = surf.verts, numverts = surf.totalverts();
						if(!numverts) { surf.verts = 0; continue; }
						surf.verts = offset;
						vertinfo *verts = c->ext->verts() + offset;
						offset += numverts;
						ivec v[4], n;
						
						int layerverts = surf.numverts&MAXFACEVERTS;
						
						int dim = ((i)>>1);
						int vc = C[dim];
						int vr = R[dim];
						int bias = 0;
						
						genfaceverts(*c, i, v);
						bool hasxyz = (vertmask&0x04)!=0, hasuv = (vertmask&0x40)!=0, hasnorm = (vertmask&0x80)!=0;
						
						if(hasxyz)
						{ 
							ivec e1, e2, e3;
							n.cross((e1 = v[1]).sub(v[0]), (e2 = v[2]).sub(v[0]));   
							if(n.iszero()) n.cross(e2, (e3 = v[3]).sub(v[0]));
							bias = -n.dot(ivec(v[0]).mul(size).add(ivec(co).mask(0xFFF).shl(3)));
						}
						else
						{
							int vis = layerverts < 4 ? (vertmask&0x02 ? 2 : 1) : 3, order = vertmask&0x01 ? 1 : 0, k = 0;
							ivec vo = ivec(co).mask(0xFFF).shl(3);

							verts[k++].setxyz(v[order].mul(size).add(vo));
							
							if(vis&1)
							{
								verts[k++].setxyz(v[order+1].mul(size).add(vo));
							}

							verts[k++].setxyz(v[order+2].mul(size).add(vo));
							
							if(vis&2)
							{
								verts[k++].setxyz(v[(order+3)&3].mul(size).add(vo));
							}
						}

						if(layerverts == 4)
						{
							if(hasxyz && vertmask&0x01)
							{
								ushort c1 = read_unsigned_short_from_buffer();
								ushort r1 = read_unsigned_short_from_buffer();
								ushort c2 = read_unsigned_short_from_buffer();
								ushort r2 = read_unsigned_short_from_buffer();

								ivec xyz;
								xyz[vc] = c1; xyz[vr] = r1; xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								verts[0].setxyz(xyz);
								xyz[vc] = c1; xyz[vr] = r2; xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								verts[1].setxyz(xyz);
								xyz[vc] = c2; xyz[vr] = r2; xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								verts[2].setxyz(xyz);
								xyz[vc] = c2; xyz[vr] = r1; xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								verts[3].setxyz(xyz);
								hasxyz = false;
							}
							if(hasuv && vertmask&0x02)
							{
								int uvorder = (vertmask&0x30)>>4;
								vertinfo &v0 = verts[uvorder], &v1 = verts[(uvorder+1)&3], &v2 = verts[(uvorder+2)&3], &v3 = verts[(uvorder+3)&3]; 
								v0.u = read_unsigned_short_from_buffer();
								v0.v = read_unsigned_short_from_buffer();
								v2.u = read_unsigned_short_from_buffer();
								v2.v = read_unsigned_short_from_buffer();
								v1.u = v0.u; v1.v = v2.v;
								v3.u = v2.u; v3.v = v0.v;
								if(surf.numverts&LAYER_DUP)
								{
									vertinfo &b0 = verts[4+uvorder], &b1 = verts[4+((uvorder+1)&3)], &b2 = verts[4+((uvorder+2)&3)], &b3 = verts[4+((uvorder+3)&3)];

									b0.u = read_unsigned_short_from_buffer();
									b0.v = read_unsigned_short_from_buffer();
									b2.u = read_unsigned_short_from_buffer();
									b2.v = read_unsigned_short_from_buffer();
									
									b1.u = b0.u; b1.v = b2.v;
									b3.u = b2.u; b3.v = b0.v;
								}
								hasuv = false;
							} 
						}

						if(hasnorm && vertmask&0x08)
						{
							ushort norm = read_unsigned_short_from_buffer();
							
							for(int k=0; k<layerverts; k++)
							{
								verts[k].norm = norm;
							}
							hasnorm = false;
						}
						
						if(hasxyz || hasuv || hasnorm)
						{
							for(int k=0; k<layerverts; k++)
							{
								vertinfo &v = verts[k];
								if(hasxyz)
								{
									ivec xyz;
									xyz[vc] = read_unsigned_short_from_buffer();
									xyz[vr] = read_unsigned_short_from_buffer();
									xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
									v.setxyz(xyz);
								}
								if(hasuv)
								{
									v.u = read_unsigned_short_from_buffer();
									v.v = read_unsigned_short_from_buffer();
								}    
								if(hasnorm)
								{
									v.norm = read_unsigned_short_from_buffer();
								}
							}

							if(surf.numverts&LAYER_DUP)
							{
								for(int k=0; k<layerverts; k++)
								{
									vertinfo &v = verts[k+layerverts], &t = verts[k];
									v.setxyz(t.x, t.y, t.z);
									if(hasuv)
									{
										v.u = read_unsigned_short_from_buffer();
										v.v = read_unsigned_short_from_buffer();
									}
									v.norm = t.norm;
								}
							}
						}
					}
				}
			}    
		}

		if(haschildren)
		{
			c->children = load_octree_children(co, size>>1, failed);
		}
		else
		{
			c->children = NULL;
		}
	}


	std::shared_ptr<cube> Cube2MapImporter::load_octree_children(const ivec &co, int size, bool &failed)
	{
		CubePointArray new_cubes = generate_new_cubes();
		
		for(std::size_t i=0; i<8; i++)
		{
			loadc(new_cubes.entry[i], ivec(i, co.x, co.y, co.z, size), size, failed);
			if(failed) break;
		}

		return new_cubes.entry[0];
	}
	

	bool Cube2MapImporter::parse_octree()
	{
		cout << "Loading octree geometry." << endl;

		octree_world_root = load_octree_children(ivec(0, 0, 0), map_header.worldsize >> 1, loading_octree_failed);

		cout << "Loading octree finished." << endl;

		return true;
	}


	bool Cube2MapImporter::parse_decompressed_data()
	{
		if(!parse_map_header())
		{
			return false;
		}

		if(!parse_map_variables())
		{
			return false;
		}

		if(!parse_game_name())
		{
			return false;
		}

		if(!parse_extra_game_info())
		{
			return false;
		}

		if(!parse_texture_cache())
		{
			return false;
		}

		if(!parse_entites())
		{
			return false;
		}

		if(!parse_vslots())
		{
			return false;
		}


		// Debug: read 100 bytes and compare RAM directly with Cube2.
		/*
		char mem_arr[100];
		
		for(int i=0; i<100; i++)
			mem_arr[i] = read_one_byte_from_buffer();
		*/


		if(!parse_octree())
		{
			return false;
		}

		return true;
	}


	bool Cube2MapImporter::load_map_file(const std::string& map_file_name)
	{
		importer_status = IMPORTER_WORKING;

		if(!load_file_into_memory(map_file_name))
		{	
			// Could not load file!
			return false;
		}
		if(!decompress_data())
		{
			// Could not decompress map file data!
			return false;
		}
		if(!parse_decompressed_data())
		{
			// Could not parse map data!
			return false;
		}

		importer_status = IMPORTER_READY;

		// Map loaded successfully!
		return true;
	}


};
};
