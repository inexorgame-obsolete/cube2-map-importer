#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

#include <zlib.h>


namespace inexor {
namespace cube2_map_importer {


	// A class for reading .ogz files which are compressed with zlib.
	// This class load the entire compressed map into memory and decompresses it with zlib.
	// The decompressed data can then be accessed by the class interface.
	// Parsing of the data itself will be implemented in the classes which inherit from this.
	class CompressedFileReader
	{
		protected:
		
			// The path+filename of the map file.
			std::string map_file_path_name;

			// The data of the compressed file.
			std::vector<char> compressed_data_buffer;

			// The size of the compressed file.
			std::size_t compressed_data_size;

			// The data of the decompressed file.
			// Decompression is done by the zlib library.
			std::vector<unsigned char> decompressed_data_buffer;
			
			// The read position of the decompressed data buffer.
			std::size_t buffer_read_position;

		
		protected:
			
			// Read memory from buffer and use it to fill out a structure.
			void read_memory_into_structure(void* target_memory, const std::size_t number_of_bytes_to_read, const std::size_t target_memory_size);

			// Reads a slice from the buffer.
			std::vector<unsigned char> read_slice_from_buffer(const std::size_t how_manumber_of_bytes_to_readny_bytes);

			// Reads a slice from the buffer as std::string.
			std::string read_slice_from_buffer_as_string(const std::size_t number_of_bytes_to_read);

			// Reads one byte from the buffer.
			unsigned char read_one_byte_from_buffer();

			// Reads a float value from the buffer.
			float read_float_from_buffer();

			// Reads an integer value from the buffer.
			int read_int_from_buffer();

			// Reads an unsigned int from the buffer.
			unsigned int read_unsigned_int_from_buffer();

			// Reads a short value from the buffer.
			short read_short_from_buffer();

			// Reads an unsigned short from the buffer.
			unsigned short read_unsigned_short_from_buffer();
			
			// Skips a certain amount of bytes before reading again.
			bool skip_reading_buffer_bytes(const std::size_t how_many_bytes_to_skip);

			// Loads an entire file into memory.
			bool load_file_into_memory(const std::string& map_file_name);
			
			// Decompress the loaded Cube2 map file using zlib.
			bool decompress_data();

			// Resets the file buffer data.
			void reset_data();
			
			// Dump the decompressed data into a file.
			void dump_decompressed_data_to_file(const std::string& file_name);


		public:

			// 
			CompressedFileReader();

			// 
			~CompressedFileReader();

	};

};
};