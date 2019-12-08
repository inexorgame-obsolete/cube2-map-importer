#pragma once

#include "Elementset.hpp"
#include "OccludeQuery.hpp"
#include "Vertex.hpp"
#include "GrassStrips.hpp"
#include "MaterialSurface.hpp"
#include "OctaEntities.hpp"


#include <vector>

namespace inexor {
namespace cube2_map_importer {

	// 
	#define ushort unsigned short
	#define uchar unsigned char
	#define GLuint unsigned int
	#define uint unsigned int

	// 
	struct VertexArray
	{
		VertexArray *parent;
		std::vector<VertexArray *> children;
		VertexArray *next, *rnext; // linked list of visible VOBs
		Vertex *vdata;           // vertex data
		ushort voffset;          // offset into vertex data
		ushort *edata, *skydata; // vertex indices
		GLuint vbuf, ebuf, skybuf; // VBOs
		ushort minvert, maxvert; // DRE info
		ElementSet *eslist;      // List of element indices sets (range) per texture
		MaterialSurface *matbuf; // buffer of material surfaces
		int verts, tris, texs, blendtris, blends, alphabacktris, alphaback, alphafronttris, alphafront, texmask, sky, explicitsky, skyfaces, skyclip, matsurfs, distance;
		double skyarea;
		ivec o;
		int size;                // location and size of cube.
		ivec geommin, geommax;   // BB of geom
		ivec shadowmapmin, shadowmapmax; // BB of shadowmapped surfaces
		ivec matmin, matmax;     // BB of any materials
		ivec bbmin, bbmax;       // BB of everything including children
		uchar curvfc, occluded;
		OccludeQuery *query;
		std::vector<OctaEntities *> mapmodels;
		std::vector<GrassStrips> grasstris;
		int hasmerges, mergelevel;
		uint dynlightmask;
		bool shadowed;
	};

};
};
