#pragma once


namespace Common
{
	enum QueryType
	{
		QueryNone = 1 << 0,
		QueryWall = 1 << 1,
		QueryCreature = 1 << 2,
		QueryItem = 1 << 3,
		QueryDoor = 1 << 4,
		QueryPlane = 1 << 5,
		QueryBox = 1 << 6,
	};


	enum SideType
	{
		SideNone,
		SideEast,
		SideWest,
		SideNorth,
		SideSouth,
	};
}


// render queues
#define RENDER_QUEUE_OUTLINE_GLOW_OBJECTS	Ogre::RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_OUTLINE_GLOW_GLOWS		Ogre::RENDER_QUEUE_MAIN + 2
#define RENDER_QUEUE_FULL_GLOW_ALPHA_GLOW	Ogre::RENDER_QUEUE_MAIN + 3
#define RENDER_QUEUE_FULL_GLOW_GLOW			Ogre::RENDER_QUEUE_MAIN + 4
#define LAST_STENCIL_OP_RENDER_QUEUE		RENDER_QUEUE_FULL_GLOW_GLOW

// stencil values
#define STENCIL_VALUE_FOR_OUTLINE_GLOW 1
#define STENCIL_VALUE_FOR_FULL_GLOW 2
#define STENCIL_FULL_MASK 0xFFFFFFFF