#pragma once


#include "Body.h"


namespace Dungeon { struct DoorProperty; }
namespace Dungeon { struct TileProperty; }
namespace Common { enum SideType; }


class BodyDoor :
	public Body
{
	friend class BodyFactory;


public:
	virtual ~BodyDoor();

	void SetupBody(
		Ogre::SceneManager&,
		Ogre::SceneNode&,
		const Ogre::Vector3& tilePosition,
		const Dungeon::TileProperty&,
		Common::SideType);


protected:
	BodyDoor();


private:
	typedef std::list< Ogre::Entity* > EntityContainer;
	EntityContainer mEntityContainer;
};