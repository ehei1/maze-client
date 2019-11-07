#include <StdAfx.h>
#include "BodyDoor.h"
#include "Tooltip.h"
#include "CommonDefine.h"
#include "DungeonManager.h"
#include "DungeonBlueprint.h"
#include "Utility.h"
#include "Application.h"


BodyDoor::BodyDoor()
{
}


BodyDoor::~BodyDoor()
{
	Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();

	for(EntityContainer::const_iterator iterator = mEntityContainer.begin();
		mEntityContainer.end() != iterator;
		++iterator)
	{
		Ogre::Entity* entity = *iterator;

		if(Ogre::SceneNode* const sceneNode = entity->getParentSceneNode())
		{
			if(nullptr != sceneNode &&
				true == sceneManager.hasSceneNode(sceneNode->getName()))
			{
				sceneManager.destroySceneNode(sceneNode);
			}
		}

		sceneManager.destroyEntity(entity->getName());
	}

	mEntity = nullptr;
	mSceneNode = nullptr;
}


void BodyDoor::SetupBody(
	Ogre::SceneManager& sceneManager,
	Ogre::SceneNode& sceneNode,
	const Ogre::Vector3& tilePosition,
	const Dungeon::TileProperty& tileProperty,
	Common::SideType sideType)
{
	for(auto iterator = tileProperty.mDoor.mDoorContainer.begin();
		iterator != tileProperty.mDoor.mDoorContainer.end();
		++iterator)
	{
		const Dungeon::DoorProperty::Door& door = *iterator;

		const Ogre::Vector3 doorScale(
			door.mScale.x * door.mSize.x * tileProperty.mWidth,
			door.mScale.y * door.mSize.y * tileProperty.mHeight,
			door.mScale.z * door.mSize.z * tileProperty.mWidth);

		Ogre::Vector3 doorPosition(
			tilePosition.x + tileProperty.mWidth * (1.0f - door.mScale.x) / 2.0f - tileProperty.mWidth * door.mPosition.x,
			tilePosition.y + tileProperty.mHeight * (1.0f - door.mScale.y) / 2.0f - tileProperty.mHeight * door.mPosition.y,
			tilePosition.z + tileProperty.mWidth * (1.0f - door.mScale.z) / 2.0f - tileProperty.mWidth * door.mPosition.z);

		Ogre::Degree rotatingDegree(0);

		switch(sideType)
		{
		case Common::SideEast:
			{
				rotatingDegree = -90.0f;
				break;
			}
		case Common::SideNorth:
			{
				rotatingDegree = 0.0f;
				break;
			}
		case Common::SideWest:
			{
				rotatingDegree = 90.0f;
				break;
			}
		case Common::SideSouth:
			{
				rotatingDegree = 180.0f;
				break;
			}
		}

		Utility::RotateXZ(
			doorPosition.x,
			doorPosition.z,
			rotatingDegree,
			tilePosition,
			doorPosition);

		// LUJ, 문의 충돌 처리를 정확히 하기 위해, 엔터티를 별도로 관리한다.
		//		mEntity는 대표 엔터티 이상의 의미는 없다
		mEntity = sceneManager.createEntity(
			"entity/door" + Ogre::StringConverter::toString(Utility::GetUniqueIndex()),
			door.mMeshFileName);
		mEntity->setMaterialName(door.mMaterialName);
		mEntity->setQueryFlags(Common::QueryDoor | Common::QueryWall);
		mEntity->setListener(this);

		mEntityContainer.push_back(mEntity);
		
		mSceneNode = sceneNode.createChildSceneNode();
		mSceneNode->attachObject(mEntity);
		//mSceneNode->showBoundingBox(true);
		mSceneNode->translate(doorPosition);
		mSceneNode->setScale(doorScale);
		mSceneNode->setOrientation(Ogre::Quaternion(rotatingDegree, Ogre::Vector3::UNIT_Y));

		SetHeight(mEntity->getBoundingRadius());
	}

	GetTooltip().AddCaption(
		Ogre::ColourValue::White,
		Ogre::ColourValue(0.3, 0.5, 0.3),
		_T("안녕하신가 힘세고 강한 아침"));
}