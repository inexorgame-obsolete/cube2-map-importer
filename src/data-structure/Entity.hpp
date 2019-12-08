#pragma once

#include "vector_geometry.hpp"


namespace inexor {
namespace cube2_map_importer {

	// entity type enumeration.
	enum ENTITY_TYPE
	{
		ET_EMPTY = 0,
		ET_LIGHT,
		ET_MAPMODEL,
		ET_PLAYERSTART,
		ET_ENVMAP,
		ET_PARTICLES,
		ET_SOUND,
		ET_SPOTLIGHT,
		ET_GAMESPECIFIC
	};


	// static entity types.
	enum STATIC_ENTITY_TYPE
	{
		NOTUSED = ET_EMPTY,         // entity slot not in use in map
		LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
		MAPMODEL = ET_MAPMODEL,     // attr1 = angle, attr2 = idx
		PLAYERSTART,                // attr1 = angle, attr2 = team
		ENVMAP = ET_ENVMAP,         // attr1 = radius
		PARTICLES = ET_PARTICLES,
		MAPSOUND = ET_SOUND,
		SPOTLIGHT = ET_SPOTLIGHT,
		I_SHELLS, I_BULLETS, I_ROCKETS, I_ROUNDS, I_GRENADES, I_CARTRIDGES,
		I_HEALTH, I_BOOST,
		I_GREENARMOUR, I_YELLOWARMOUR,
		I_QUAD,
		TELEPORT,                   // attr1 = idx, attr2 = model, attr3 = tag
		TELEDEST,                   // attr1 = angle, attr2 = idx
		MONSTER,                    // attr1 = angle, attr2 = monstertype
		CARROT,                     // attr1 = tag, attr2 = type
		JUMPPAD,                    // attr1 = zpush, attr2 = ypush, attr3 = xpush
		BASE,
		RESPAWNPOINT,
		BOX,                        // attr1 = angle, attr2 = idx, attr3 = weight
		BARREL,                     // attr1 = angle, attr2 = idx, attr3 = weight, attr4 = health
		PLATFORM,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
		ELEVATOR,                   // attr1 = angle, attr2 = idx, attr3 = tag, attr4 = speed
		FLAG,                       // attr1 = angle, attr2 = team
		MAXENTTYPES
	};


	// Entity structure for persistent map entities.
	struct Entity                                   
	{
		vec o;                                      
		short attr1, attr2, attr3, attr4, attr5;
		unsigned char type;                                
		unsigned char reserved;
	};

};
};
