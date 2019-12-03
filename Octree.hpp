#pragma once

#include <vector>

#include "Entity.hpp"

#include <array>


namespace inexor {
namespace cube2_map_importer {

	#define uint unsigned int
	#define uchar unsigned char
	#define ushort unsigned short
	#define GLuint unsigned int

	struct vertex
	{
		vec pos;
		bvec norm;
		uchar reserved;
		float u, v;
		short lmu, lmv;
		bvec tangent;
		uchar bitangent;
	};


	struct elementset
	{
		ushort texture, lmid, envmap;
		uchar dim, layer;
		ushort length[2], minvert[2], maxvert[2];
	};


	struct materialsurface
	{
		enum
		{
			F_EDIT = 1<<0
		};

		ivec o;
		ushort csize, rsize;
		ushort material, skip;
		uchar orient, flags;
		union
		{
			short index;
			short depth;
		};
		union
		{
			Entity *light;
			ushort envmap;
			uchar ends;
		};
	};


	struct occludequery
	{
		void *owner;
		GLuint id;
		int fragments;
	};


	struct grasstri
	{
		vec v[4];
		int numv;
		vec4 tcu, tcv;
		plane surface;
		vec center;
		float radius;
		float minz, maxz;
		ushort texture, lmid;
	};

	struct octaentities
	{
		vector<int> mapmodels;
		vector<int> other;
		occludequery *query;
		octaentities *next, *rnext;
		int distance;
		ivec o;
		int size;
		ivec bbmin, bbmax;

		octaentities(const ivec &o, int size) : query(0), o(o), size(size), bbmin(o), bbmax(o)
		{
			bbmin.add(size);
		}
	};


	struct vtxarray
	{
		vtxarray *parent;
		std::vector<vtxarray *> children;
		vtxarray *next, *rnext; // linked list of visible VOBs
		vertex *vdata;           // vertex data
		ushort voffset;          // offset into vertex data
		ushort *edata, *skydata; // vertex indices
		GLuint vbuf, ebuf, skybuf; // VBOs
		ushort minvert, maxvert; // DRE info
		elementset *eslist;      // List of element indices sets (range) per texture
		materialsurface *matbuf; // buffer of material surfaces
		int verts, tris, texs, blendtris, blends, alphabacktris, alphaback, alphafronttris, alphafront, texmask, sky, explicitsky, skyfaces, skyclip, matsurfs, distance;
		double skyarea;
		ivec o;
		int size;                // location and size of cube.
		ivec geommin, geommax;   // BB of geom
		ivec shadowmapmin, shadowmapmax; // BB of shadowmapped surfaces
		ivec matmin, matmax;     // BB of any materials
		ivec bbmin, bbmax;       // BB of everything including children
		uchar curvfc, occluded;
		occludequery *query;
		vector<octaentities *> mapmodels;
		vector<grasstri> grasstris;
		int hasmerges, mergelevel;
		uint dynlightmask;
		bool shadowed;
	};

	
	enum
	{
		LAYER_TOP    = (1<<5),
		LAYER_BOTTOM = (1<<6),
		LAYER_DUP    = (1<<7),
		LAYER_BLEND  = LAYER_TOP|LAYER_BOTTOM,
		MAXFACEVERTS = 15
	};


	enum
	{
		LMID_AMBIENT = 0,
		LMID_AMBIENT1,
		LMID_BRIGHT,
		LMID_BRIGHT1,
		LMID_DARK,
		LMID_DARK1,
		LMID_RESERVED
	};


	enum
	{
		MATF_INDEX_SHIFT  = 0,
		MATF_VOLUME_SHIFT = 2,
		MATF_CLIP_SHIFT   = 5,
		MATF_FLAG_SHIFT   = 8,
		MATF_INDEX  = 3 << MATF_INDEX_SHIFT,
		MATF_VOLUME = 7 << MATF_VOLUME_SHIFT,
		MATF_CLIP   = 7 << MATF_CLIP_SHIFT,
		MATF_FLAGS  = 0xFF << MATF_FLAG_SHIFT
	};


	enum
	{
		OCTSAV_CHILDREN = 0,
		OCTSAV_EMPTY,
		OCTSAV_SOLID,
		OCTSAV_NORMAL,
		OCTSAV_LODCUBE
	};


	// ok
	struct surfacecompat
	{
		uchar texcoords[8];
		uchar w, h;
		ushort x, y;
		uchar lmid, layer;
	};
	
	
	// ok 
	struct normalscompat
	{
		bvec normals[4];
	};

	// ok 
	struct mergecompat
	{
		ushort u1, u2, v1, v2;
	};


	// hardcoded texture numbers
	enum
	{
		DEFAULT_SKY = 0,
		DEFAULT_GEOM
	};

	// cube empty-space materials
	enum
	{
		MAT_AIR      = 0,                      // the default, fill the empty space with air
		MAT_WATER    = 1 << MATF_VOLUME_SHIFT, // fill with water, showing waves at the surface
		MAT_LAVA     = 2 << MATF_VOLUME_SHIFT, // fill with lava
		MAT_GLASS    = 3 << MATF_VOLUME_SHIFT, // behaves like clip but is blended blueish
		MAT_NOCLIP   = 1 << MATF_CLIP_SHIFT,  // collisions always treat cube as empty
		MAT_CLIP     = 2 << MATF_CLIP_SHIFT,  // collisions always treat cube as solid
		MAT_GAMECLIP = 3 << MATF_CLIP_SHIFT,  // game specific clip material
		MAT_DEATH    = 1 << MATF_FLAG_SHIFT,  // force player suicide
		MAT_ALPHA    = 4 << MATF_FLAG_SHIFT   // alpha blended
	};


	// 
	#define LM_MINW 2
	#define LM_MINH 2
	#define LM_MAXW 128
	#define LM_MAXH 128
	#define LM_PACKW 512
	#define LM_PACKH 512


	#define PI  (3.1415927f)
	#define RAD (PI / 180.0f)


	// TODO: Remove this!
	#define edgemake(a, b) ((b)<<4|a)
	#define edgeget(edge, coord) ((coord) ? (edge)>>4 : (edge)&0xF)
	#define edgeset(edge, coord, val) ((edge) = ((coord) ? ((edge)&0xF)|((val)<<4) : ((edge)&0xF0)|(val)))
	#define cubeedge(c, d, x, y) ((c).edges[(((d)<<2)+((y)<<1)+(x))])
	#define isempty(c) ((c).faces[0]==F_EMPTY)
	#define isentirelysolid(c) ((c).faces[0]==F_SOLID && (c).faces[1]==F_SOLID && (c).faces[2]==F_SOLID)

	#define loop(v,m) for(int v = 0; v<int(m); v++)
	#define loopi(m) loop(i,m)
	#define loopj(m) loop(j,m)
	#define loopk(m) loop(k,m)
	#define loopl(m) loop(l,m)

	const uint F_EMPTY = 0;             // all edges in the range (0,0)
	const uint F_SOLID = 0x80808080;    // all edges in the range (0,8)

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


	struct surfaceinfo
	{
		uchar lmid[2];
		uchar verts, numverts;

		int totalverts() const { return numverts&LAYER_DUP ? (numverts&MAXFACEVERTS)*2 : numverts&MAXFACEVERTS; }
		bool used() const { return lmid[0] != LMID_AMBIENT || lmid[1] != LMID_AMBIENT || numverts&~LAYER_TOP; }
		void clear() { lmid[0] = LMID_AMBIENT; lmid[1] = LMID_AMBIENT; numverts = (numverts&MAXFACEVERTS) | LAYER_TOP; }
		void brighten() { lmid[0] = LMID_BRIGHT; lmid[1] = LMID_AMBIENT; numverts = (numverts&MAXFACEVERTS) | LAYER_TOP; }
	};


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


	// 
	struct CubeExtension
	{
		// vertex array for children, or NULL.
		std::vector<vtxarray> vertex_array;
		
		// map entities inside cube.
		std::vector<octaentities> entities;

		// render info for each surface.
		surfaceinfo surfaces[6];
		
		// linked list of t-joints.
		int tjoints;
		
		// allocated space for verts.
		uchar maxverts;
	}; 


	// 
	struct cube
	{
		// Points to 8 children cubes, or NULL. -Z first, then -Y, -X
		// We use std::array to enable multiple (8) return values for functions.
		std::array<std::shared_ptr<cube>, 8> children;

		// TODO: Debug!
		cube(uint face = F_EMPTY, int mat = MAT_AIR)
		{
			for(std::size_t i=0; i<8; i++)
			{
				children[i] = NULL;
			}

			extension = NULL;
			visible = NULL;
			merged = NULL;
		
			for(std::size_t k=0; k<3; k++)
			{
				faces[k] = face;
			}

			for(std::size_t l=0; l<6; l++)
			{
				texture[l] = DEFAULT_GEOM;
			}

			material = material;
		}


		// extended info of the cube.
		std::shared_ptr<CubeExtension> extension;


		// TODO: Remove unions?
		union
		{
			// edges of the cube, each uchar is 2 4bit values denoting the range.
			// see documentation jpgs for more info.
			uchar edges[12];
			
			// 4 edges of each dimension together representing 2 perpendicular faces.
			uint faces[3];
		};
		
		// one for each face. same order as orient.
		ushort texture[6];

		// empty-space material.
		ushort material;

		// merged faces of the cube.
		uchar merged;
		
		// TODO: Remove unions?
		union
		{
			// mask of which children have escaped merges.
			uchar escaped;

			// visibility info for faces.
			uchar visible;
		};
	};


};
};
