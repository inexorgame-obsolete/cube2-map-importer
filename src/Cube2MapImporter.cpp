#include "Cube2MapImporter.hpp"


namespace inexor {
namespace cube2_map_importer {


	Cube2MapImporter::Cube2MapImporter()
	{
		importer_status = IMPORTER_READY;
	}


	Cube2MapImporter::~Cube2MapImporter()
	{
	}


	const CUBE2_MAP_IMPORTER_STATUS Cube2MapImporter::get_status() const
	{
		return importer_status;
	}


	bool Cube2MapImporter::load_map_file(std::shared_ptr<Cube2Map>& map_data_ptr, const std::string& map_file_name)
	{
		importer_status = IMPORTER_WORKING;

		// 
		set_map_data_pointer(map_data_ptr);

		// TODO: Use lock guard!

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
