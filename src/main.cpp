#include <iostream>
#include <thread>
using namespace std;

#include "Cube2MapImporter.hpp"


// TODO: Make sure this program runs on x64 bit!
// TODO: There is a memory leak somewhere! Maybe we shouldn't allocate that much memory?
// TODO: Load unzipped files as well (Do not unzip in the progress)?
// TODO: Join threads.
// TODO: multithreading!
// TODO: Display CRC32 checksum!


// The main program start.
int main(int argc, char* argv[])
{
	// Check how many maps will be loaded?
	// The argument with index 0 is the path of the program itself.
	std::size_t number_of_files = argc - 1;
	
	cout << "There are " << number_of_files << " maps to load." << endl;

	// Initialise the Cube2 map importer.
	std::vector<inexor::cube2_map_importer::Cube2MapImporter> maps(number_of_files);
	
	// The threads of the main program.
	//std::vector<std::thread> map_loading_threads(number_of_files);

	cout << "----------------------------------------------------------------------------" << endl;

	// Loop through all maps and load them.
	for(int i=1; i<argc; i++)
	{
		// We must subtract 1 from the index since argv[0]
		// is the path and name to the program itself.
		std::size_t array_index = i - 1;

		using namespace inexor::cube2_map_importer;

		// The map importer interface.
		Cube2MapImporter cube_map_importer;
		
		// Allocate the data of the map.
		std::shared_ptr<Cube2Map> example_map = std::make_shared<Cube2Map>();

		// Load the map!
		cube_map_importer.load_map_file(example_map, argv[i]);

		cout << "----------------------------------------------------------------------------" << endl;

		cin.get();
		cout << endl << endl << endl;
	}

	cin.get();
	return 0;
}
