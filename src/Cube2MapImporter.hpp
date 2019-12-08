#pragma once

#include "Cube2MapDataParser.hpp"
#include "Cube2Map.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <mutex>
using namespace std;


namespace inexor {
namespace cube2_map_importer {


	// The status of the map importer.
	// Asking for the status is important for multithreading.
	enum CUBE2_MAP_IMPORTER_STATUS
	{
		IMPORTER_READY = 0,
		IMPORTER_WORKING
	};
	

	// An interface for loading Cube2 maps.
	class Cube2MapImporter : public Cube2MapDataParser
	{
		private:

			// The status of the map importer interface.
			// Asking for status is important for multithreading.
			CUBE2_MAP_IMPORTER_STATUS importer_status;
		
			// The mutex of this class.
			std::mutex map_importer_mutex;


		public:
			
			Cube2MapImporter();

			~Cube2MapImporter();

			// Returns the status of the map importer.
			const CUBE2_MAP_IMPORTER_STATUS get_status() const;
			
			// Loads a Cube2 map file.
			// The file will be decompressed using zlib.
			// The decompressed data will be parsed into the Cube2Map data class.
			bool load_map_file(std::shared_ptr<Cube2Map>& map_data_ptr, const std::string& map_file_name);
			

	};

};
};
