#include "Cube2MapDataParser.hpp"
#include "Helpers.hpp"


namespace inexor {
namespace cube2_map_importer {


	Cube2MapDataParser::Cube2MapDataParser()
	{
	}


	Cube2MapDataParser::~Cube2MapDataParser()
	{
	}


	void Cube2MapDataParser::abort_loading()
	{
		// TODO: Implement!
	}


	void Cube2MapDataParser::set_map_data_pointer(std::shared_ptr<Cube2Map> cube_map_ptr)
	{
		cube2map = cube_map_ptr;
	}


	bool Cube2MapDataParser::read_map_header()
	{
		cout << "----------------------------------------------------------------------------" << endl;

		cout << "Reading map header." << endl;

		// TODO: Refactor this!

		// Define the number of bytes of the structure which will be filled out.
		#define MAP_OCTA_HEADER_READ_SIZE 7 * sizeof(int)

		// Read most of the map header, but not everything.
		// blendmap, numvars and numvslots will be read later.
		read_memory_into_structure(&cube2map->header, MAP_OCTA_HEADER_READ_SIZE, MAP_OCTA_HEADER_READ_SIZE);
		
		cout << "Validating map header." << endl;
		
		// Check if the magic number of this file's header is valid.
		if(0 != std::memcmp(cube2map->header.magic, "OCTA", 4))
		{
			// Something is wrong with the header section of this map!
			cout << "Error: Invalid magic number in map header!" << endl;
			return false;
		}
		else
		{
			// The magic number of this file is correct.
			cout << "Magic number: OCTA" << endl;
		}
		
		// Check if world size is valid.
		if(cube2map->header.worldsize <= 0)
		{
			cout << "Error: Invalid world size in map header!" << endl;
			return false;
		}
		
		cout << "Entities: " << cube2map->header.number_of_entities << endl;

		// Check if the number of entites in this map is valid.
		// Please note that a map can have 0 entities!
		if(cube2map->header.number_of_entities < 0)
		{
			cout << "Error: Invalid amount of entites!" << endl;
			return false;
		}

		cout << "Map version: " << cube2map->header.version << endl;

		// Check if the mapversion is supported.
		if(cube2map->header.version > CUBE2_MAP_IMPORTER_HIGHEST_SUPPORTED_MAPVERSION)
		{
			cout << "This map requires a newer version of the map loader!" << endl;
			return false;
		}

		// In older versions of Cube2 the memory layout was different.
		// Support older map formats as well!
		if(cube2map->header.version <= 28)
		{
			auto p = cube2map->old_game_version_header;

			// Read every byte manually.
			p.lightprecision = read_int_from_buffer();
			p.lighterror = read_int_from_buffer();
			p.lightlod = read_int_from_buffer();
			p.ambient = read_one_byte_from_buffer();
			p.watercolour[0] = read_one_byte_from_buffer();
			p.watercolour[1] = read_one_byte_from_buffer();
			p.watercolour[2] = read_one_byte_from_buffer();
			p.blendmap = read_one_byte_from_buffer();
			p.lerpangle = read_one_byte_from_buffer();
			p.lerpsubdiv = read_one_byte_from_buffer();
			p.lerpsubdivsize = read_one_byte_from_buffer();
			p.bumperror = read_one_byte_from_buffer();
			p.skylight[0] = read_one_byte_from_buffer();
			p.skylight[1] = read_one_byte_from_buffer();
			p.skylight[2] = read_one_byte_from_buffer();
			p.lavacolour[0] = read_one_byte_from_buffer();
			p.lavacolour[1] = read_one_byte_from_buffer();
			p.lavacolour[2] = read_one_byte_from_buffer();
			p.waterfallcolour[0] = read_one_byte_from_buffer();
			p.waterfallcolour[1] = read_one_byte_from_buffer();
			p.waterfallcolour[2] = read_one_byte_from_buffer();

			for(std::size_t i=0; i<10; i++)
			{
				p.reserved[i] = read_one_byte_from_buffer();
			}

			for(std::size_t i=0; i<128; i++)
			{
				p.maptitle[i] = read_one_byte_from_buffer();
			}
		}
		else
		{
			// Looks like the change the arrangement of bytes in map version 30.
			int extra = 0;

			if(cube2map->header.version <= 29)
			{
				extra++;
			}

			cout << "Extra bytes: " << extra << endl;
				
			// Size of the rest of the bytes to read.
			#define BLENDMAP_BYTE_SIZE sizeof(cube2map->header) - (7+extra)*sizeof(int)

			// Read additional bytes for blend maps.
			// Important note: We read the memory into map_header, not compatible_map_header!
			read_memory_into_structure(&cube2map->header.blendmap, BLENDMAP_BYTE_SIZE, BLENDMAP_BYTE_SIZE);
		}

		if(cube2map->header.version <= 28)
		{
			auto p = cube2map->old_game_version_header;

			if(p.lightprecision)
			{
				cout << "Light precision: " << static_cast<int>(p.lightprecision) << endl;
			}
				
			if(p.lighterror)
			{
				cout << "Light error: " << static_cast<int>(p.lighterror) << endl;
			}
        
			if(p.bumperror)
			{
				cout << "Bump error: " << static_cast<int>(p.bumperror) << endl;
			}
        
			if(p.lightlod)
			{
				cout << "Light lod: " << static_cast<int>(p.lightlod) << endl;
			}
        
			if(p.ambient)
			{
				cout << "Ambient: " << static_cast<int>(p.ambient) << endl;
			}
        
			if(p.skylight)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(p.skylight);
				cout << "Skylight: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(p.watercolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(p.watercolour);
				cout << "Watercolor: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(p.waterfallcolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(p.waterfallcolour);
				cout << "Waterfallcolour: ";
				print_rgb_color_to_stdcout(output_color);
			}
        
			if(p.lavacolour)
			{
				RGBColor output_color = convert_3_bytes_to_rgb(p.lavacolour);
				cout << "Lavacolour: ";
				print_rgb_color_to_stdcout(output_color);
			}
				
			if(p.lerpangle)
			{
				cout << "Lerpangle: " << static_cast<int>(p.lerpangle) << endl;
			}
        
			if(p.lerpsubdiv)
			{
				cout << "Lerpsubdiv: " << static_cast<int>(p.lerpsubdiv) << endl;
			}

			if(p.lerpsubdivsize)
			{
				cout << "Lerpsubdivsize: " << static_cast<int>(p.lerpsubdivsize) << endl;
			}
			
			
			if(std::strlen(p.maptitle) > 0)
			{
				// Store the name of the map title.
				cube2map->map_title = p.maptitle;

				// Remove Cube2 text colorisation from map title.
				cube2map->clean_map_title = remove_cube2_text_colorisation(cube2map->map_title);

				cout << "Maptitle: " << cube2map->map_title.c_str() << endl;

				// Check if Misan made this map.
				if(cube2map->clean_map_title.find("Misan") != string::npos)
				{
					cout << "Misan found!" << endl;
				}
			}

			if(cube2map->header.blendmap)
			{
				cout << "Blendmap: " << cube2map->header.blendmap << endl;

				cube2map->header.blendmap = p.blendmap;
			}

			cube2map->header.number_of_map_variables = 0;

			cube2map->header.number_of_vslots = 0;
		}
		else if(cube2map->header.version <= 29)
		{
			cube2map->header.number_of_vslots = 0;
		}

		cout << "World size: " << cube2map->header.worldsize << endl;

		// Calculate the world scale.
		while(1 << cube2map->world_scale < cube2map->header.worldsize)
		{
			cube2map->world_scale++;
		}
		
		cout << "World scale: " << cube2map->world_scale << endl;

		return true;
	}


	bool Cube2MapDataParser::read_map_variables()
	{
		cout << "Map variables: " << cube2map->header.number_of_map_variables << endl;

		if(cube2map->header.number_of_map_variables > 0)
		{
			cout << "----------------------------------------------------------------------------" << endl;
			
			cout << "Reserving " << cube2map->header.number_of_map_variables << " map variable slots." << endl;

			// Reserve memory ahead of time instead of using cube2map->variables.push_back().
			cube2map->variables.resize(cube2map->header.number_of_map_variables);
			
			// Loop through all variables and read them.
			for(int i=0; i<cube2map->header.number_of_map_variables; i++)
			{
				// The new variable which is being read.
				MapVariable new_map_variable;
				
				// Which type of variable are we expecting?
				int var_type = read_one_byte_from_buffer();

				// The length of the string.
				int var_length = read_short_from_buffer();

				// Read the map variable's name.
				new_map_variable.name = read_slice_from_buffer_as_string(var_length);

				// Print the name of the map variable.
				cout << new_map_variable.name << ": ";

				// Parse the map variable data depending on the memory type.
				switch(var_type)
				{
					case ID_VAR:
					{
						// Set this as an integer variable.
						new_map_variable.type = ID_VAR;

						new_map_variable.value_int = read_int_from_buffer();

						// Check if this variable is a color variable (e.g. skylight).
						// If so, convert the integer bytes to rgb() values.
						// Those rgb values are easier to read and understand.
						if(is_integer_map_variable_a_color_value(new_map_variable.name))
						{
							// This is a color variable.
							new_map_variable.type = ID_COLORVAR;

							convert_integer_value_to_rgb_code(new_map_variable.value_int, new_map_variable.value_color);
							
							// Print color as RGB value.
							print_rgb_color_to_stdcout(new_map_variable.value_color);
						}
						else
						{
							// This is not a color variable.
							// Instead this is just a normal integer value.
							cout << new_map_variable.value_int << endl;
						}

						break;
					}
					case ID_FVAR:
					{
						// Set this as a float variable.
						new_map_variable.type = ID_FVAR;

						new_map_variable.value_float = read_float_from_buffer();
						
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
							
						// Check if this is the maptitle!
						if(0 == new_map_variable.name.compare("maptitle"))
						{
							// Store the name of the map.
							cube2map->map_title = new_map_variable.value_string;

							// Remove Cube2 console text colorisation from map title.
							cube2map->clean_map_title = remove_cube2_text_colorisation(cube2map->map_title);

							// Print cleaned map title.
							cout << cube2map->clean_map_title.c_str() << endl;

							// Check for Misan.
							if(cube2map->clean_map_title.find("Misan") != string::npos)
							{
								cout << "Misan found!" << endl;
							}
						}
						else
						{
							// No, this map variable is not the map title.
							// Print the value of the map variable to the console.
							cout << new_map_variable.value_string.c_str() << endl;
						}
						break;
					}
				}
					
				// We are done parsing the data.
				// Now add it to the vector of map variables.
				cube2map->variables[i] = new_map_variable;
			}

			// Check if the number of variables to read is valid.
			if(cube2map->header.number_of_map_variables > MAX_NUMVARS)
			{
				// Something went wrong when parsing the number of variables in the map.
				cout << "Warning: more than " << MAX_NUMVARS << " map variables to read (" << cube2map->header.number_of_map_variables << ")!" << endl;
				cout << "Cube2: Sauerbraten would have aborted reading after " << MAX_NUMVARS << " variables!" << endl;
			}
			
			cout << "----------------------------------------------------------------------------" << endl;
		}

		return true;
	}


	bool Cube2MapDataParser::read_game_name()
	{
		if(cube2map->header.version >= 16)
		{
			// The length of game name.
			int game_name_length = read_one_byte_from_buffer();

			// Read the name of the game.
			cube2map->game_name = read_slice_from_buffer_as_string(game_name_length);

			// We need to read 1 more byte since the string is null-terminated.
			skip_reading_buffer_bytes(1);

			cout << "Game name: " << cube2map->game_name.c_str() << endl;

			// For which game was this made?
			if(0 == cube2map->game_name.compare("fps"))
			{
				// A Cube2: Sauerbraten map!
				// This should be the case for most maps.
				cube2map->map_for_cube2_game = true;
			}
			else
			{
				// Eisenstern maybe?
				cube2map->map_for_cube2_game = false;

				if(0 == cube2map->game_name.compare("rpg"))
				{
					// Yes, Eisenstern!
					// This is the case for k_rpg1
					cube2map->map_for_eisenstern_rpg = true;
				}
				else
				{
					// No, some other crazy project.
					cube2map->map_for_eisenstern_rpg = false;

					// Well, this is some other map format then?
					cout << "Warning: This is not a Cube2 map and not an Eisenstern map either!" << endl;
				}
			}
		}

		return true;
	}

	
	bool Cube2MapDataParser::read_extra_game_info()
	{
		// Since the data layout has changed over time,
		// we need to support all other versions as well.
		int extra_game_info_size = 0;
			
		// In Cube map format version 16 they introduces a specification for
		// the game since they were working on Eisenstern..
		if(cube2map->header.version >= 16)
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
				cube2map->extra_game_info = read_slice_from_buffer(how_many_bytes_to_read);
			}
		}

		return true;
	}
	
	
	bool Cube2MapDataParser::read_texture_cache()
	{
		cout << "Loading texture cache." << endl;

		if(cube2map->header.version < 14)
		{
			cout << "Loading 256 texture MRU bytes." << endl;

			// Pre-allocate the required memory.
			cube2map->texture_mru.resize(256);

			// Load new texture format.
			for(std::size_t i=0; i<256; i++)
			{
				// Do not use push_back, since memory is already allocated.
				cube2map->texture_mru[i] = read_unsigned_short_from_buffer();
			}
		}
		else
		{
			// In map format version greater or equal 14
			// we can have as many texture levels as we want.
			unsigned short how_many_texture_levels = read_unsigned_short_from_buffer();

			cout << "Loading " << how_many_texture_levels << " texture MRU bytes." << endl;
				
			// Allocate memory ahead of time.
			cube2map->texture_mru.resize(how_many_texture_levels);
				
			if(how_many_texture_levels > 0)
			{
				// Load new texture format.
				for(std::size_t i=0; i<how_many_texture_levels; i++)
				{
					// Do not use push_back, since memory is already allocated.
					cube2map->texture_mru[i] = read_unsigned_short_from_buffer();
				}
			}
		}

		cout << "Loaded " << cube2map->texture_mru.size() << " texture MRU bytes." << endl;
			
		return true;
	}


	bool Cube2MapDataParser::read_entites()
	{
		cout << "Loading entites." << endl;

		// This may change for other games?
		int extra_entity_info_size = 0;

		// Allocate memory.
		cube2map->extra_entity_info.resize(extra_entity_info_size);

		// How many entities to load.
		int number_of_entities_to_load = std::min(cube2map->header.number_of_entities, MAXENTS);

		cout << "Entities: " << number_of_entities_to_load << "." << endl;

		if(number_of_entities_to_load > 0)
		{
			// Load all entites from the map.
			for(int i=0; i<number_of_entities_to_load; i++)
			{
				// A new entity to load.
				EntityAdditionalData new_entity;

				// Fill out entity structure.
				read_memory_into_structure(&new_entity, sizeof(Entity), sizeof(Entity));

				// Set the data of the entity.
				new_entity.spawned = false;
				new_entity.inoctanode = false;

				// Fix the entity's data.
				fix_entity_data(new_entity, cube2map->header.version);

				// Yes, this is a Cube2 map.
				if(cube2map->map_for_cube2_game)
				{
					// Read extra information on entities.
					if(extra_entity_info_size > 0)
					{
						// Warning: Since there is no map which has this
						// data in it for testing, this part of the code
						// remains untested to this day.
						cube2map->extra_entity_info = read_slice_from_buffer(extra_entity_info_size);
					}

					// Note: extra_entity_info isn't actually used in the game.
					// but we still need to read this from the buffer to have the
					// right read position in the stream.

					// Fix entity data.

					// TODO: Why again????
					fix_entity_data(new_entity, cube2map->header.version);
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
				if(!is_entity_inside_world(new_entity, cube2map->header.worldsize))
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
				if(cube2map->header.version <= 14 && new_entity.type == ET_MAPMODEL)
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
				cube2map->entities.push_back(new_entity);
			}
		}

		cout << "Loaded " << cube2map->entities.size() << " entities." << endl;

		// TODO: Remove this! We should be able to load as many entites as we like!
		// TODO: Does it really work this way? Didn't we already read all the stuff anyways?

		// Check if there are too many entites in the world.
		if(cube2map->header.number_of_entities > MAXENTS) 
		{
			// Do not read the following entites but move the read position
			// in the stream forward anyways.
			int how_many_bytes_to_skip = 0;

			cout << "Warning: map has " << cube2map->header.number_of_entities << " entities!" << endl;
				
			// How many entites are we ignoring?
			int how_many_entites_not_to_read = cube2map->header.number_of_entities - MAXENTS;

			// How many bytes for an entity.
			int byte_size_of_entity_structure = 0;

			// How many byte for one entity?
			if(cube2map->map_for_cube2_game)
			{
				// TODO: Wait a second.. wat?!
				byte_size_of_entity_structure = sizeof(Entity) + cube2map->extra_game_info_size;
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


	bool Cube2MapDataParser::read_vslots()
	{
		cout << "VSlots: " << cube2map->header.number_of_vslots << "." << endl;

		// Initialise a vector of VSlots for preview.
		// Allocate the required memory and fill it with -1.
		// TODO: Shouldn't we initiate with -1 as value?
		std::vector<std::size_t> preview_vslots(cube2map->header.number_of_vslots, 0);
		
		// TODO: reserve memory in advance to minimize allocation.

		// How many VSlots to load?
		// TODO: Check if we should change to int?
		int vslots_to_load = cube2map->header.number_of_vslots;

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
					VSlot new_vslot(NULL, cube2map->vertex_slots.size());
						
					// Add to vslot buffer.
					cube2map->vertex_slots.push_back(new_vslot);
				}

				// What is this?
				vslots_to_load += changed;
			}
			else
			{
				std::size_t array_index = cube2map->vertex_slots.size();

				// Read size from buffer.
				preview_vslots[array_index] = read_int_from_buffer();
					
				// A new vslot.
				VSlot new_vslot(NULL, cube2map->vertex_slots.size());
					
				// Add to vslot buffer.
				cube2map->vertex_slots.push_back(new_vslot);
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

		for(std::size_t i=0; i<cube2map->vertex_slots.size(); i++)
		{
			// TODO: Fix signed/unsigned warning!
			if(preview_vslots[i] <= cube2map->vertex_slots.size())
			{
				cube2map->vertex_slots[preview_vslots[i]].next = &cube2map->vertex_slots[i];
			}
		}

		// Check for errors.
		if(cube2map->vertex_slots.size() != cube2map->header.number_of_vslots)
		{
			cout << "Error: Loading vslots failed!" << endl;
			return false;
		}
		else
		{
			cout << "Loaded " << cube2map->vertex_slots.size() << " vslots." << endl;
		}

		return true;
	}

	
	void Cube2MapDataParser::fill_octree_node_with_data(std::shared_ptr<OctreeCube>& c, const ivec &co, int size, bool &failed)
	{
		bool haschildren = false;
		
		// TODO: What does "sav" stand for?
		int octsav = read_one_byte_from_buffer();

		switch(octsav&0x7)
		{
			case OCTSAV_CHILDREN:
			{
				// Load the children octree nodes!
				c->haschildren = true;
				c->children = load_octree_node(co, size>>1, failed);
				return;
			}
			case OCTSAV_LODCUBE:
			{
				// This octree node does have children!
				haschildren = true;
				break;
			}
			case OCTSAV_EMPTY:
			{
				c->faces[0] = F_EMPTY;
				c->faces[1] = F_EMPTY;
				c->faces[2] = F_EMPTY;
				break;
			}
			case OCTSAV_SOLID:
			{
				c->faces[0] = F_SOLID;
				c->faces[1] = F_SOLID;
				c->faces[2] = F_SOLID;
				break;
			}
			case OCTSAV_NORMAL:
			{
				// 
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
			if(cube2map->header.version < 14)
			{
				c->texture[i] = read_one_byte_from_buffer();
			}
			else
			{
				c->texture[i] = read_unsigned_short_from_buffer();
			}
		}

		if(cube2map->header.version < 7)
		{
			skip_reading_buffer_bytes(3);
		}
		else if(cube2map->header.version <= 31)
		{
			uchar mask = read_one_byte_from_buffer();

			if(mask & 0x80) 
			{
				int mat = read_one_byte_from_buffer();

				if(cube2map->header.version < 27)
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

					if(std::size_t(mat) < sizeof(matconv)/sizeof(matconv[0]))
					{
						c->material = matconv[mat];
					}
					else
					{
						c->material = MAT_AIR;
					}
				}
				else
				{
					c->material = convert_old_material(mat);
				}
			}
			
			SurfaceCompat surfaces[12];
			NormalsCompat normals[6];
			MergeCompat merges[6];

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
						// 
						read_memory_into_structure(&surfaces[i], sizeof(SurfaceCompat), sizeof(SurfaceCompat));

						if(cube2map->header.version < 10)
						{
							++surfaces[i].lmid;
						}

						if(cube2map->header.version < 18)
						{
							if(surfaces[i].lmid >= LMID_AMBIENT1)
							{
								++surfaces[i].lmid;
							}
							
							if(surfaces[i].lmid >= LMID_BRIGHT1)
							{
								++surfaces[i].lmid;
							}
						}
						
						if(cube2map->header.version < 19)
						{
							if(surfaces[i].lmid >= LMID_DARK)
							{
								surfaces[i].lmid += 2;
							}
						}

						if(i < 6)
						{
							if(mask & 0x40)
							{
								hasnorms |= 1<<i;

								// 
								read_memory_into_structure(&normals[i], sizeof(NormalsCompat), sizeof(NormalsCompat));
							}

							if(surfaces[i].layer != 0 || surfaces[i].lmid != LMID_AMBIENT)
							{
								hassurfs |= 1<<i;
							}

							if(surfaces[i].layer&2)
							{
								numsurfs++;
							}
						}
					}
				}
			}

			if(cube2map->header.version <= 8)
			{
				edgespan2vectorcube(c);
			}

			if(cube2map->header.version <= 11)
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

			if(cube2map->header.version >= 20)
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
									MergeCompat *m = &merges[i];
									
									// 
									read_memory_into_structure(&m, sizeof(MergeCompat), sizeof(MergeCompat));
									
									if(cube2map->header.version <= 25)
									{
										int uorigin = m->u1 & 0xE000;
										int vorigin = m->v1 & 0xE000;

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
				convert_old_surfaces(c, co, size, surfaces, hassurfs, normals, hasnorms, merges, hasmerges);
			}
			
		}
		else
		{
			if(octsav&0x40) 
			{
				if(cube2map->header.version <= 32)
				{
					int mat = read_one_byte_from_buffer();

					c->material = convert_old_material(mat);
				}
				else
				{
					c->material = read_unsigned_short_from_buffer();
				}
			}

			if(octsav&0x80)
			{
				c->merged = read_one_byte_from_buffer();
			}

			if(octsav&0x20)
			{
				int surfmask = read_one_byte_from_buffer();
				int totalverts = read_one_byte_from_buffer();
				int offset = 0;
				
				// 
				new_cube_extension(*c, totalverts, false);

				// 
				c->extension->reset_surfaces();

				// 
				//c->extension->reset_vertices();

				for(std::size_t i=0; i<6; i++)
				{
					if(surfmask&(1<<i)) 
					{
						// 
						SurfaceInfo &surf = c->extension->surfaces[i];
						
						// 
						read_memory_into_structure(&surf, sizeof(SurfaceInfo), sizeof(SurfaceInfo));

						int vertmask = surf.verts;
						int numverts = surf.totalverts();
						
						if(!numverts)
						{
							surf.verts = 0;
							continue;
						}
						
						surf.verts = offset;
						
						// TODO: Why is this out of the vectors range?
						VertexInfo *verts = &c->extension->vertices[offset];

						offset += numverts;

						ivec v[4];
						ivec n;
						
						int layerverts = surf.numverts&MAXFACEVERTS;
						int dim = ((i)>>1);
						int vc = C[dim];
						int vr = R[dim];
						int bias = 0;

						// TODO: Refactor/Resolve this!
						genfaceverts(*c, i, v);

						bool hasxyz = (vertmask&0x04)!=0;
						bool hasuv = (vertmask&0x40)!=0;
						bool hasnorm = (vertmask&0x80)!=0;
						
						if(hasxyz)
						{
							ivec e1, e2, e3;

							n.cross((e1 = v[1]).sub(v[0]), (e2 = v[2]).sub(v[0]));   
							
							if(n.iszero())
							{
								n.cross(e2, (e3 = v[3]).sub(v[0]));
							}

							bias = -n.dot(ivec(v[0]).mul(size).add(ivec(co).mask(0xFFF).shl(3)));
						}
						else
						{
							// TODO: Resolve This!
							int vis = layerverts < 4 ? (vertmask&0x02 ? 2 : 1) : 3;
							int order = vertmask&0x01 ? 1 : 0;
							int k = 0;

							ivec vo = ivec(co).mask(0xFFF).shl(3);

							// TODO: Which address does this point to?
							verts[k++].setxyz(v[order].mul(size).add(vo));
							
							if(vis&1)
							{
								verts[k++].setxyz(v[order+1].mul(size).add(vo));
							}
							
							// TODO: Which address does this point to?
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
								
								xyz[vc] = c1; xyz[vr] = r1;
								xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								// TODO: Which address does this point to?
								verts[0].setxyz(xyz);

								xyz[vc] = c1; xyz[vr] = r2;
								xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								// TODO: Which address does this point to?
								verts[1].setxyz(xyz);
								
								xyz[vc] = c2; xyz[vr] = r2;
								xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								// TODO: Which address does this point to?
								verts[2].setxyz(xyz);
								
								xyz[vc] = c2; xyz[vr] = r1;
								xyz[dim] = -(bias + n[vc]*xyz[vc] + n[vr]*xyz[vr])/n[dim];
								// TODO: Which address does this point to?
								verts[3].setxyz(xyz);
								
								hasxyz = false;
							}

							if(hasuv && vertmask&0x02)
							{
								int uvorder = (vertmask&0x30)>>4;
								
								// TODO: Which address does this point to?
								VertexInfo &v0 = verts[uvorder];
								VertexInfo &v1 = verts[(uvorder+1)&3];
								VertexInfo &v2 = verts[(uvorder+2)&3];
								VertexInfo &v3 = verts[(uvorder+3)&3]; 

								v0.u = read_unsigned_short_from_buffer();
								v0.v = read_unsigned_short_from_buffer();
								v2.u = read_unsigned_short_from_buffer();
								v2.v = read_unsigned_short_from_buffer();

								v1.u = v0.u;
								v1.v = v2.v;
								v3.u = v2.u;
								v3.v = v0.v;
								
								if(surf.numverts&LAYER_DUP)
								{
									// TODO: Which address does this point to?
									VertexInfo &b0 = verts[4+uvorder];
									VertexInfo &b1 = verts[4+((uvorder+1)&3)];
									VertexInfo &b2 = verts[4+((uvorder+2)&3)];
									VertexInfo &b3 = verts[4+((uvorder+3)&3)];

									b0.u = read_unsigned_short_from_buffer();
									b0.v = read_unsigned_short_from_buffer();
									b2.u = read_unsigned_short_from_buffer();
									b2.v = read_unsigned_short_from_buffer();

									b1.u = b0.u;
									b1.v = b2.v;
									b3.u = b2.u;
									b3.v = b0.v;
								}

								hasuv = false;
							} 
						}

						if(hasnorm && vertmask&0x08)
						{
							ushort norm = read_unsigned_short_from_buffer();
							
							// TODO: Which address does this point to?
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
								VertexInfo &v = verts[k];

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
						}

						if(surf.numverts&LAYER_DUP)
						{
							for(int k=0; k<layerverts; k++)
							{
								VertexInfo &v = verts[k+layerverts];
								VertexInfo &t = verts[k];
									
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

		if(haschildren)
		{
			c->haschildren = true;

			// Time to load the children of this octree node!
			c->children = load_octree_node(co, size>>1, failed);
		}
	}


	std::array<std::shared_ptr<OctreeCube>, 8> Cube2MapDataParser::load_octree_node(const ivec &co, int size, bool &failed)
	{
		// The 8 sub-cubes of this octree node.
		// They might be NULL if this octree node is a leaf node.
		std::array<std::shared_ptr<OctreeCube>, 8> octree_node;

		for(std::size_t i=0; i<8; i++)
		{
			// Create a new shared pointer for every sub-cube.
			octree_node[i] = std::make_shared<OctreeCube>();

			// Fill the sub cubes with data.
			fill_octree_node_with_data(octree_node[i], ivec(i, co.x, co.y, co.z, size), size, failed);

			// TODO: Refactor this!
			// TODO: Implement abort_loading() method!
			if(failed)
			{
				cout << "load_octree_node FAILED!" << endl;
				break;
			}
		}

		// Increase the total number of existing octree nodes.
		cube2map->number_of_octree_nodes++;

		return octree_node;
	}


	bool Cube2MapDataParser::read_octree()
	{
		cout << "Loading octree geometry" << endl;

		// Create a shared pointer for the world root!
		// It is important to do this before calling load_octree_node
		// since otherwise the function will return std::nullptr!
		cube2map->octree_world_root = std::make_shared<OctreeCube>();

		// The octree world root has children.
		cube2map->octree_world_root->haschildren = true;

		// Load the root of the octree game world.
		// TODO: Refactor function call!
		cube2map->octree_world_root->children = load_octree_node(ivec(0, 0, 0), cube2map->header.worldsize >> 1, cube2map->loading_octree_failed);
		
		cout << "Loaded " << cube2map->number_of_octree_nodes << " octree nodes." << endl;

		return true;
	}


	bool Cube2MapDataParser::parse_decompressed_data()
	{
		//#define INEXOR_CUBE2_MAP_IMPORTER_DECOMPRESSED_DATA_MEMORY_DUMP

		#ifdef INEXOR_CUBE2_MAP_IMPORTER_DECOMPRESSED_DATA_MEMORY_DUMP
			dump_decompressed_data_to_file("decompressed_data.bin");
		#endif

		if(!read_map_header())
		{
			return false;
		}

		if(!read_map_variables())
		{
			return false;
		}

		if(!read_game_name())
		{
			return false;
		}

		if(!read_extra_game_info())
		{
			return false;
		}

		if(!read_texture_cache())
		{
			return false;
		}

		if(!read_entites())
		{
			return false;
		}

		if(!read_vslots())
		{
			return false;
		}

		if(!read_octree())
		{
			return false;
		}
		
		// TODO: Validate octree.
		// TODO: Read lightmaps!
		// TODO: Load PVS visibility tree!
		// TODO: Load blend maps!
		// TODO: Generate crc32 checksum from the decompressed data.

		return true;
	}


};
};
