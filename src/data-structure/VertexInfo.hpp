#pragma once

#include "vector_geometry.hpp"


namespace inexor {
namespace cube2_map_importer {

	// 
	#define ushort unsigned short

	// 
	struct VertexInfo
	{
		ushort x, y, z, u, v, norm;

		void setxyz(ushort a, ushort b, ushort c) { x = a; y = b; z = c; }
		void setxyz(const ivec &v) { setxyz(v.x, v.y, v.z); }
		void set(ushort a, ushort b, ushort c, ushort s = 0, ushort t = 0, ushort n = 0) { setxyz(a, b, c); u = s; v = t; norm = n; }
		void set(const ivec &v, ushort s = 0, ushort t = 0, ushort n = 0) { set(v.x, v.y, v.z, s, t, n); }
		ivec getxyz() const { return ivec(x, y, z); }
	};

};
};
