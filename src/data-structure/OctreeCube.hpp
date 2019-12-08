#pragma once

#include "OctreeCubeAdditionalData.hpp"
#include "MaterialsEnumeration.hpp"

#include <memory>
#include <array>


namespace inexor {
namespace cube2_map_importer {
	
	// 
	#define uint unsigned int
	#define uchar unsigned char
	#define ushort unsigned short

	// 
	const uint F_EMPTY = 0;             // all edges in the range (0,0)
	const uint F_SOLID = 0x80808080;    // all edges in the range (0,8)
	
	// 
	struct OctreeCube
	{
		// Points to 8 children cubes, or NULL. -Z first, then -Y, -X
		// We use std::array to enable multiple (8) return values for functions.
		std::array<std::shared_ptr<OctreeCube>, 8> children;


		// Default constructor.
		OctreeCube(uint face = F_EMPTY, int mat = MAT_AIR)
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

			material = mat;

			haschildren = false;
		}


		void discard_children()
		{
			for(std::size_t i=0; i<8; i++)
			{
				children[i] = NULL;
			}
		}


		// Extended info of the cube.
		std::shared_ptr<OctreeCubeAdditionalData> extension;
	
		// Does this cube have children octree nodes?
		bool haschildren;

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
