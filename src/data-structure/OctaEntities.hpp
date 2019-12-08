#pragma once

#include "OctaEntities.hpp"
#include "OccludeQuery.hpp"
#include "vector_geometry.hpp"

#include <vector>


namespace inexor {
namespace cube2_map_importer {

	// 
	struct OctaEntities
	{
		std::vector<int> mapmodels;
		std::vector<int> other;
		OccludeQuery *query;
		OctaEntities *next, *rnext;
		int distance;
		ivec o;
		int size;
		ivec bbmin, bbmax;

		OctaEntities(const ivec &o, int size) : query(0), o(o), size(size), bbmin(o), bbmax(o)
		{
			bbmin.add(size);
		}
	};

};
};
