#pragma once

#include <vector>

#include "Entity.hpp"


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


	struct surfacecompat
	{
		uchar texcoords[8];
		uchar w, h;
		ushort x, y;
		uchar lmid, layer;
	};
	
	
	struct normalscompat
	{
		bvec normals[4];
	};

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


	#define LM_MINW 2
	#define LM_MINH 2
	#define LM_MAXW 128
	#define LM_MAXH 128
	#define LM_PACKW 512
	#define LM_PACKH 512

	#define dimension(orient) ((orient)>>1)


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


	struct vertinfo
	{
		ushort x, y, z, u, v, norm;

		void setxyz(ushort a, ushort b, ushort c) { x = a; y = b; z = c; }
		void setxyz(const ivec &v) { setxyz(v.x, v.y, v.z); }
		void set(ushort a, ushort b, ushort c, ushort s = 0, ushort t = 0, ushort n = 0) { setxyz(a, b, c); u = s; v = t; norm = n; }
		void set(const ivec &v, ushort s = 0, ushort t = 0, ushort n = 0) { set(v.x, v.y, v.z, s, t, n); }
		ivec getxyz() const { return ivec(x, y, z); }
	};


	struct cubeext
	{
		vtxarray *va;            // vertex array for children, or NULL
		octaentities *ents;      // map entities inside cube
		surfaceinfo surfaces[6]; // render info for each surface
		int tjoints;             // linked list of t-joints
		uchar maxverts;          // allocated space for verts

		vertinfo *verts()
		{
			return (vertinfo *)(this+1);
		}
	}; 


	struct cube
	{
		cube *children;          // points to 8 cube structures which are its children, or NULL. -Z first, then -Y, -X
		cubeext *ext;            // extended info for the cube
		union
		{
			uchar edges[12];     // edges of the cube, each uchar is 2 4bit values denoting the range.
								 // see documentation jpgs for more info.
			uint faces[3];       // 4 edges of each dimension together representing 2 perpendicular faces
		};
		ushort texture[6];       // one for each face. same order as orient.
		ushort material;         // empty-space material
		uchar merged;            // merged faces of the cube
		union
		{
			uchar escaped;       // mask of which children have escaped merges
			uchar visible;       // visibility info for faces
		};
	};

	const uint F_EMPTY = 0;             // all edges in the range (0,0)
	const uint F_SOLID = 0x80808080;    // all edges in the range (0,8)


};
};
