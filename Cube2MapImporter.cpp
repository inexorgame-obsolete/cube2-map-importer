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

	
	bool Cube2MapImporter::parse_map_header()
	{
		cout << "Parsing map header." << endl;

		// Define the number of bytes from the structure which are filled out.
		#define MAP_HEADER_READ_SIZE 7 * sizeof(int)

		// Read most of the map header, but not everything.
		// blendmap, numvars and numvslots will be read later.
		read_memory_into_structure(&map_header, MAP_HEADER_READ_SIZE, MAP_HEADER_READ_SIZE);

		cout << "Validating map header." << endl;

		// Copy magic number and null-terminate it automatically.
		std::string magic_number(map_header.magic);

		cout << "Magic number: " << magic_number.c_str() << endl;
		
		// Check if magic number "OCTA" is valid.
		if(0 != std::memcmp(map_header.magic, "OCTA", 4))
		{
			// Something is wrong with the header section of this map!
			cout << "Error: Invalid magic number in map header!" << endl;
			return false;
		}
		
		cout << "Worldsize: " << map_header.worldsize << endl;
		
		// Check if world size is valid.
		if(map_header.worldsize <= 0)
		{
			cout << "Error: Invalid world size in map header!" << endl;
			return false;
		}
		
		cout << "Entities: " << map_header.numents << endl;

		// Check if number of entites is valid.
		// Please note that a map can have 0 entities though!
		if(map_header.numents < 0)
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
			// Read additional bytes for light precision.
			#define EXTRA_DATA_BYTE_SIZE sizeof(CompatibilityMapHeader) - 7*sizeof(int)
			read_memory_into_structure(&compatible_map_header.lightprecision, EXTRA_DATA_BYTE_SIZE, sizeof(OctaHeader));
		}
		else
		{
			// Looks like the change the arrangement of bytes in map version 30.
			int extra = 0;
			if(map_header.version <= 29) extra++;

			// Print message about the extra byte size to read.
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
				cout << "Light precision: " << compatible_map_header.lightprecision << endl;
			}
				
			if(compatible_map_header.lighterror)
			{
				cout << "Light error: " << compatible_map_header.lighterror << endl;
			}
        
			if(compatible_map_header.bumperror)
			{
				cout << "Bump error: " << compatible_map_header.bumperror << endl;
			}
        
			if(compatible_map_header.lightlod)
			{
				cout << "Light lod: " << compatible_map_header.lightlod << endl;
			}
        
			if(compatible_map_header.ambient)
			{
				cout << "Ambient: " << compatible_map_header.ambient << endl;
			}
        
			if(compatible_map_header.skylight)
			{
				cout << "Skylight: " << (int(compatible_map_header.skylight[0])<<16) << ", " << (int(compatible_map_header.skylight[1])<<8) << ", " << int(compatible_map_header.skylight[2]) << endl;
			}
        
			if(compatible_map_header.watercolour)
			{
				cout << "Watercolour: " << (int(compatible_map_header.watercolour[0])<<16) << ", " << (int(compatible_map_header.watercolour[1])<<8) << ", " << int(compatible_map_header.watercolour[2]) << endl;
			}
        
			if(compatible_map_header.waterfallcolour)
			{
				cout << "Waterfallcolour: " << (int(compatible_map_header.waterfallcolour[0])<<16) << ", " << (int(compatible_map_header.waterfallcolour[1])<<8) << ", " << int(compatible_map_header.waterfallcolour[2]) << endl;
			}
        
			if(compatible_map_header.lavacolour)
			{
				cout << "Lavacolour: " << (int(compatible_map_header.lavacolour[0])<<16) << ", " << (int(compatible_map_header.lavacolour[1])<<8) << ", " << int(compatible_map_header.lavacolour[2]) << endl;
			}
				
			if(compatible_map_header.lerpangle)
			{
				cout << "Lerpangle: " << compatible_map_header.lerpangle << endl;
			}
        
			if(compatible_map_header.lerpsubdiv)
			{
				cout << "Lerpsubdiv: " << compatible_map_header.lerpsubdiv << endl;
			}

			if(compatible_map_header.lerpsubdivsize)
			{
				cout << "Lerpsubdivsize: " << compatible_map_header.lerpsubdivsize << endl;
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
			}

			map_header.numvars = 0;

			map_header.numvslots = 0;
		}
		else if(map_header.version <= 29)
		{
			map_header.numvslots = 0;
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
		const std::size_t number_of_entries = 11;

		// The map variables which are rgb color codes.
		const std::string color_map_variables[number_of_entries] = {"lavacolour", "skylight", "watercolour", "cloudcolour",
		"fogcolour", "fogdomecolour", "skyboxcolour", "ambient", "shadowmapambient", "sunlight", "cloudboxcolor"}; 

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


	bool Cube2MapImporter::parse_map_variables()
	{
		// Number of map variables.
		cout << "Map variables: " << map_header.numvars << endl;

		
		// Check if we read something wrong!
		if(map_header.numvars > 0)
		{
			cout << "----------------------------------------------------------------------------" << endl;
			
			// Loop through all variables and load them.
			for(int i=0; i<map_header.numvars; i++)
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
							cout << "rgb(" << new_map_variable.value_color.r << ", " << new_map_variable.value_color.g << ", " << new_map_variable.value_color.b << ")" << endl;
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
			if(map_header.numvars > MAX_NUMVARS)
			{
				// Something went wrong when parsing the number of variables in the map.
				cout << "Warning: more than " << MAX_NUMVARS << " to read (" << map_header.numvars << ")!" << endl;
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

			// Print the name of the game to the console.
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

			// Print the number of bytes to load for texture data.
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
		int number_of_entities_to_load = std::min(map_header.numents, MAXENTS);

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
		if(map_header.numents > MAXENTS) 
		{
			// Do not read the following entites but move the read position
			// in the stream forward anyways.
			int how_many_bytes_to_skip = 0;

			cout << "Warning: map has " << map_header.numents << " entities!" << endl;
				
			// How many entites are we ignoring?
			int how_many_entites_not_to_read = map_header.numents - MAXENTS;

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
		cout << "VSlots: " << map_header.numvslots << "." << endl;

		// Initialise a vector of VSlots for preview.
		// Allocate the required memory and fill it with -1.
		// TODO: Check if we should change the vector to int?
		std::vector<std::size_t> preview_vslots(map_header.numvslots, -1);
			
		// TODO: reserve memory in advance to minimize allocation.

		// How many VSlots to load?
		// TODO: Check if we should change to int?
		std::size_t vslots_to_load = map_header.numvslots;

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
				// Read size from buffer.
				preview_vslots[map_vertex_slots.size()] = read_int_from_buffer();
					
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

						// TODO: Remove this?
						ShaderParam& p = new_shader_parameter;
						
						cout << "Shader parameter " << p.name.c_str() << ": " << p.val[0] << " " << p.val[1] << " " << p.val[2] << " " << p.val[3] << endl;


						// TODO: pre-allocate memory instead of push_back!

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
			if(preview_vslots[i] <= map_vertex_slots.size())
			{
				// TODO: Debug!
				map_vertex_slots[preview_vslots[i]].next = &map_vertex_slots[i];
			}
		}

		
		// Check for errors.
		if(map_vertex_slots.size() != map_header.numvslots)
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


	bool Cube2MapImporter::parse_decompressed_data()
	{
		// Please note: Do NOT mix up the order of function calls!

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
