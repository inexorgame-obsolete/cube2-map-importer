#pragma once

namespace inexor {
namespace cube2_map_importer {

	// 
	#define GLuint unsigned int

	// 
	struct OccludeQuery
	{
		void *owner;
		GLuint id;
		int fragments;
	};

};
};
