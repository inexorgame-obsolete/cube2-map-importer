#include "CompressedFileReader.hpp"


namespace inexor {
namespace cube2_map_importer {


	CompressedFileReader::CompressedFileReader()
	{
		reset_data();
	}


	CompressedFileReader::~CompressedFileReader()
	{
	}


	void CompressedFileReader::reset_data()
	{
		map_file_path_name = "";
		compressed_data_buffer.clear();
		decompressed_data_buffer.clear();
		compressed_data_size = 0;
		buffer_read_position = 0;
	}


	bool CompressedFileReader::load_file_into_memory(const std::string& file_name)
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
			compressed_data_size = static_cast<std::size_t>(cube2_map_file.tellg());

			cout << "File size: " << compressed_data_size << " bytes." << endl;

			cout << "Allocating memory." << endl;

			// Preallocate the memory for the file buffer
			// to prevent allocation during the read operation.
			compressed_data_buffer.resize(compressed_data_size);

			// Set the reading pointer to the start
			// of the file before starting to read data.
			cube2_map_file.seekg(0, std::ios::beg);

			cout << "Reading file data." << endl;

			// Read the entire file into memory.
			cube2_map_file.read(&compressed_data_buffer.data()[0], compressed_data_size);

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

	
	bool CompressedFileReader::decompress_data()
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
		
		// Make the decompressed data buffer smaller to free the unused memory.
		decompressed_data_buffer.resize(decompression_stream.total_out);

		cout << "Finished decompressing." << endl;

		return true;
	}


	std::vector<unsigned char> CompressedFileReader::read_slice_from_buffer(const std::size_t number_of_bytes_to_read)
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

	
	std::string CompressedFileReader::read_slice_from_buffer_as_string(const std::size_t number_of_bytes_to_read)
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

		return return_value;
	}


	unsigned char CompressedFileReader::read_one_byte_from_buffer()
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


	float CompressedFileReader::read_float_from_buffer()
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


	int CompressedFileReader::read_int_from_buffer()
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


	unsigned int CompressedFileReader::read_unsigned_int_from_buffer()
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


	short CompressedFileReader::read_short_from_buffer()
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


	unsigned short CompressedFileReader::read_unsigned_short_from_buffer()
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


	void CompressedFileReader::read_memory_into_structure(void* target_memory, const std::size_t number_of_bytes_to_read, const std::size_t target_memory_size)
	{
		// Read memory from buffer.
		std::vector<unsigned char> memory_source = read_slice_from_buffer(number_of_bytes_to_read);

		// Copy memory to structure.
		std::memcpy(target_memory, &memory_source[0], target_memory_size);
	}


	bool CompressedFileReader::skip_reading_buffer_bytes(const std::size_t number_of_bytes_to_skip)
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

	
	void CompressedFileReader::dump_decompressed_data_to_file(const std::string& file_name)
	{
		ofstream outfile(file_name.c_str(), ios::out|ios::binary);
		outfile.write((char*)&decompressed_data_buffer[0], decompressed_data_buffer.size());
		outfile.close();
	}

};
};
