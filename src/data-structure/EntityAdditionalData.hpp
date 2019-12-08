#pragma once

#include "Entity.hpp"
#include "EntityLight.hpp"


namespace inexor {
namespace cube2_map_importer {

	// Extended entity structure.
	// The part of the entity that doesn't get saved to disk.
	struct EntityAdditionalData : Entity
	{
		enum
		{
			F_NOVIS     = 1<<0,
			F_NOSHADOW  = 1<<1,
			F_NOCOLLIDE = 1<<2,
			F_ANIM      = 1<<3
		};

		// The only dynamic state of a map entity.
		unsigned char spawned, inoctanode, visible, flags;

		// 
		EntityLight light;
		
		// TODO: Replace with std::shared_ptr
		EntityAdditionalData *attached;

		EntityAdditionalData()
		{
			visible = false;
			flags = 0;
			attached = NULL;
		}
	};


};
};
