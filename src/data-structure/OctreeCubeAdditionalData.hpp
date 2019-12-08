#pragma once

#include "VertexInfo.hpp"
#include "SurfaceInfo.hpp"
#include "VertexArray.hpp"

#include <memory>
#include <vector>


namespace inexor {
namespace cube2_map_importer {

	// 
	#define uchar unsigned char

	// 
	struct OctreeCubeAdditionalData
	{
		OctreeCubeAdditionalData(std::shared_ptr<OctreeCubeAdditionalData>& old_cube_extension, int max_vertices)
		{
			if(old_cube_extension)
			{
				vertex_array = old_cube_extension->vertex_array;
				entities = old_cube_extension->entities;
				tjoints = old_cube_extension->tjoints;
			}
			else
			{
				vertex_array.clear();
				entities.clear();
				tjoints = -1;
			}

			maxverts = max_vertices;

			// Clear buffers
			vertices.clear();
			vertex_array.clear();
			entities.clear();

			// TODO: Does this even work?
			vertices.resize(maxverts);
		}

		void reset_surfaces()
		{
			std::memset(surfaces, 0, sizeof(surfaces));
		}

		void reset_vertices()
		{
			vertices.clear();
			vertices.reserve(maxverts);
		}

		// TODO: Is this the solution ?
		std::vector<VertexInfo> vertices;

		// vertex array for children, or NULL.
		std::vector<VertexArray> vertex_array;
		
		// map entities inside cube.
		std::vector<OctaEntities> entities;

		// render info for each surface.
		SurfaceInfo surfaces[6];
		
		// linked list of t-joints.
		int tjoints;
		
		// allocated space for verts.
		uchar maxverts;
	}; 

};
};
