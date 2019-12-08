#pragma once

#include "vector_geometry.hpp"


namespace inexor {
namespace cube2_map_importer {


	#define uchar unsigned char	

	// 
	struct Vertex
	{
		vec pos;
		bvec norm;
		uchar reserved;
		float u, v;
		short lmu, lmv;
		bvec tangent;
		uchar bitangent;
	};

};
};
