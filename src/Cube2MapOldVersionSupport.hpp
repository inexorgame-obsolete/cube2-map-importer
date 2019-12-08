#pragma once

#include "data-structure/SurfaceCompat.hpp"
#include "data-structure/NormalsCompat.hpp"
#include "data-structure/MergeCompat.hpp"
#include "data-structure/OctreeCube.hpp"

#include <memory>


namespace inexor {
namespace cube2_map_importer {


	// The macro behind this has been resolved.
	const ivec facecoords[6][4] =
	{
		{ {0,8,8}, {0,8,0}, {0,0,0}, {0,0,8} },
		{ {8,8,8}, {8,0,8}, {8,0,0}, {8,8,0} },
		{ {8,0,8}, {0,0,8}, {0,0,0}, {8,0,0} },
		{ {0,8,0}, {0,8,8}, {8,8,8}, {8,8,0} },
		{ {0,0,0}, {0,8,0}, {8,8,0}, {8,0,0} },
		{ {0,0,8}, {8,0,8}, {8,8,8}, {0,8,8} }
	};


	// Some legacy lightmap constants.
	#define LM_MINW 2
	#define LM_MINH 2
	#define LM_MAXW 128
	#define LM_MAXH 128
	#define LM_PACKW 512
	#define LM_PACKH 512


	// TODO: Resolve this!
	#define GENFACEVERTX(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSX(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(0, GENFACEVERTX(0,0, x0,y1,z1, d0,r1,c1), GENFACEVERTX(0,1, x0,y1,z0, d0,r1,c0), GENFACEVERTX(0,2, x0,y0,z0, d0,r0,c0), GENFACEVERTX(0,3, x0,y0,z1, d0,r0,c1)) \
		GENFACEORIENT(1, GENFACEVERTX(1,0, x1,y1,z1, d1,r1,c1), GENFACEVERTX(1,1, x1,y0,z1, d1,r0,c1), GENFACEVERTX(1,2, x1,y0,z0, d1,r0,c0), GENFACEVERTX(1,3, x1,y1,z0, d1,r1,c0))
	#define GENFACEVERTY(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(2, GENFACEVERTY(2,0, x1,y0,z1, c1,d0,r1), GENFACEVERTY(2,1, x0,y0,z1, c0,d0,r1), GENFACEVERTY(2,2, x0,y0,z0, c0,d0,r0), GENFACEVERTY(2,3, x1,y0,z0, c1,d0,r0)) \
		GENFACEORIENT(3, GENFACEVERTY(3,0, x0,y1,z0, c0,d1,r0), GENFACEVERTY(3,1, x0,y1,z1, c0,d1,r1), GENFACEVERTY(3,2, x1,y1,z1, c1,d1,r1), GENFACEVERTY(3,3, x1,y1,z0, c1,d1,r0))
	#define GENFACEVERTZ(o,n, x,y,z, xv,yv,zv) GENFACEVERT(o,n, x,y,z, xv,yv,zv)
	#define GENFACEVERTSZ(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEORIENT(4, GENFACEVERTZ(4,0, x0,y0,z0, r0,c0,d0), GENFACEVERTZ(4,1, x0,y1,z0, r0,c1,d0), GENFACEVERTZ(4,2, x1,y1,z0, r1,c1,d0), GENFACEVERTZ(4,3, x1,y0,z0, r1,c0,d0)) \
		GENFACEORIENT(5, GENFACEVERTZ(5,0, x0,y0,z1, r0,c0,d1), GENFACEVERTZ(5,1, x1,y0,z1, r1,c0,d1), GENFACEVERTZ(5,2, x1,y1,z1, r1,c1,d1), GENFACEVERTZ(5,3, x0,y1,z1, r0,c1,d1))
	#define GENFACEVERTSXY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSX(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1)
	#define GENFACEVERTS(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSXY(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1) \
		GENFACEVERTSZ(x0,x1, y0,y1, z0,z1, c0,c1, r0,r1, d0,d1)

	
	// TODO: Resolve this?
	#define edgeget(edge, coord) ((coord) ? (edge)>>4 : (edge)&0xF)
	#define cubeedge(c, d, x, y) ((c).edges[(((d)<<2)+((y)<<1)+(x))])


	// A class for supporting older Cube2 map formats.
	class Cube2MapOldVersionSupport
	{
		protected:
			
			// 
			ushort encode_normal(const vec &n);
			
			// 
			std::shared_ptr<OctreeCubeAdditionalData> new_cube_extension(OctreeCube &c, int maxverts, bool init);

			// 
			void edgespan2vectorcube(std::shared_ptr<OctreeCube> &c);

			// 
			void genfaceverts(const OctreeCube &c, int orient, ivec v[4]);
			
			// TODO: Refactor to std::shard_ptr
			void set_surfaces(OctreeCube &c, const SurfaceInfo *surfs, std::vector<VertexInfo>& verts, int numverts);
			
			// 
			void genvertp(OctreeCube &c, ivec &p1, ivec &p2, ivec &p3, plane &pl, bool solid = false);

			// 
			int edgeval(OctreeCube &c, const ivec &p, int dim, int coord);

			// 
			void genedgespanvert(ivec &p, OctreeCube &c, vec &v);

			// 
			void fix_entity_data(Entity &e, const int version);

			// 
			int convert_old_material(int mat);

			// TODO: Refactor to std::shard_ptr
			void convert_old_surfaces(std::shared_ptr<OctreeCube> &c, const ivec &co, int size, SurfaceCompat *srcsurfs, int hassurfs, NormalsCompat *normals, int hasnorms, MergeCompat *merges, int hasmerges);

	};

};
};
