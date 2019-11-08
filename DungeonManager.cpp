#include <StdAfx.h>
#include "DungeonManager.h"
#include "DungeonParser.h"
#include "DungeonBlueprint.h"
#include "CommonDefine.h"
#include "Utility.h"
#include "BodyFactory.h"
#include "Application.h"
#include "BodyDoor.h"


namespace Dungeon
{
	Manager::Manager() :
	mParser(new Parser),
	mBlueprint(new Blueprint),
	mFloorBody(nullptr)
	{
		mParser->LoadProperty("media/dungeon/tile.xml", mTileProperty);

		mBlueprint->SetTileSize(mTileProperty.mWidth);
	}


	Manager::~Manager()
	{
		Release();
	}


	void Manager::RandomizeBlueprint(int width, int height)
	{
		mBlueprint->Randomize(width, height);
	}


	void Manager::Build(
		Ogre::SceneNode& sceneNode,
		Ogre::SceneManager& sceneManager)
	{
		Ogre::Entity* wall = nullptr;
		const Ogre::String wallEntityName("entity/wall");

		if(true == sceneManager.hasEntity(wallEntityName))
		{
			wall = sceneManager.getEntity(wallEntityName);
		}
		else
		{
			wall = sceneManager.createEntity(
				wallEntityName, "Barrel.mesh");
				//mTileProperty.mWall.mMeshFileName);
			//wall->setMaterialName(mTileProperty.mWall.mMaterialName);
			wall->setMaterialName("Ogre/Tusks");
			wall->setQueryFlags(Common::QueryWall);
		}

		// LUJ, 상수값을 쓰지 않으려면, ManaulObject를 만들어야 한다.
		//		중요한 일은 아니므로 일단 연기
		{
			const Ogre::AxisAlignedBox axisAlignedBox(
				Ogre::Vector3(-0.5, -0.5, -0.5),
				Ogre::Vector3(0.5, 0.5, 0.5));
			wall->getMesh()->_setBounds(axisAlignedBox, false);
		}

		// LUJ, 충돌 블록을 만든다
		for(Blueprint::BlockIterator blockIterator = mBlueprint->GetBlockIterator();
			true == blockIterator.hasMoreElements();
			blockIterator.moveNext())
		{
			const RECT& rect = *(blockIterator.current());
			const Ogre::Vector2 tileSize(
				rect.right - rect.left + 1,
				rect.bottom - rect.top + 1);

			const Ogre::Vector3 blockPosition(
				(rect.left + rect.right) / 2.0f * mTileProperty.mWidth,
				mTileProperty.mHeight / 2,
				(rect.top + rect.bottom) / 2.0f * mTileProperty.mWidth);

			Ogre::Entity* const entity = wall->clone("entity/block" + Ogre::StringConverter::toString(Utility::GetUniqueIndex()));
			entity->setCastShadows(true);

			Ogre::SceneNode* const childSceneNode = sceneNode.createChildSceneNode();
			childSceneNode->attachObject(entity);

			//childSceneNode->showBoundingBox(true);
			childSceneNode->translate(blockPosition);
			childSceneNode->setScale(
				(tileSize.x * mTileProperty.mWidth),
				mTileProperty.mHeight,
				(tileSize.y * mTileProperty.mWidth));
			childSceneNode->setVisible(true);

			// LUJ, 물리 엔진으로 동작하는 충돌 블록을 만든다
			// {
			// 	OgreNewt::World* const newtonWorld = Application::GetSingleton().GetNewtonWorld();
			// 	OgreNewt::ConvexCollisionPtr collisionPointer(new OgreNewt::CollisionPrimitives::Box(
			// 		newtonWorld,
			// 		Ogre::Vector3(tileSize.x * mTileProperty.mWidth, mTileProperty.mHeight, tileSize.y * mTileProperty.mWidth),
			// 		0,
			// 		Ogre::Quaternion::IDENTITY,
			// 		Ogre::Vector3::ZERO));
			// 	OgreNewt::Body* const body = new OgreNewt::Body(newtonWorld, collisionPointer);
			// 
			// 	Ogre::Real mass = 100.0f;
			// 	Ogre::Vector3 inertia, offset;
			// 	collisionPointer->calculateInertialMatrix(inertia, offset);
			// 
			// 	body->setPositionOrientation(
			// 		blockPosition,
			// 		Ogre::Quaternion::IDENTITY);
			// 	body->attachNode(childSceneNode);
			// 	//body->setMassMatrix(mass, mass * inertia);
			// 	//body->setCenterOfMass(offset);
			// 	body->freeze();
			// 	//body->setStandardForceCallback();
			// }
		}

		// LUJ, 던전 지형을 만든다
		{
			const Ogre::String staticGeometryName = "staticGeometry/dungeon";

			sceneManager.destroyStaticGeometry(staticGeometryName);

			Ogre::StaticGeometry* const dungeonStaticGeometry = sceneManager.createStaticGeometry(staticGeometryName);
			Ogre::Vector3 minPosition(FLT_MAX, 0, FLT_MAX);
			Ogre::Vector3 maxPosition(-FLT_MAX, 0, -FLT_MAX);

			for(Blueprint::TileIterator TileIterator = mBlueprint->GetTileIterator();
				true == TileIterator.hasMoreElements();
				TileIterator.moveNext())
			{
				auto iterator = TileIterator.current();
				const Blueprint::Tile& tile = iterator->first;
				const Blueprint::TileProperty& tileProperty = iterator->second;

				const Ogre::Vector3 tilePosition(
					tile.first * mTileProperty.mWidth,
					mTileProperty.mHeight / 2,
					tile.second * mTileProperty.mWidth);

				switch(tileProperty.mType)
				{
				case Blueprint::TileWall:
				case Blueprint::TileNone:
				case Blueprint::TileBoundary:
					{
						const Ogre::Vector3 tileScale(
							mTileProperty.mWidth,
							mTileProperty.mHeight,
							mTileProperty.mWidth);

						dungeonStaticGeometry->addEntity(
							wall,
							tilePosition,
							Ogre::Quaternion::IDENTITY,
							tileScale);
						break;
					}
				case Blueprint::TileDoor:
					{
						const Ogre::String doorEntityName = "entity/door" + Ogre::StringConverter::toString(Utility::GetUniqueIndex());

						Body* const body = Application::GetSingleton().GetBodyFactory().AddBody(
							BodyFactory::TypeDoor,
							sceneNode.getName(),
							doorEntityName);

						BodyDoor* const bodyDoor = dynamic_cast< BodyDoor* >(body);

						// LUJ, 리팩토링 여지가 있는 코드지만... 애니메이션 상태 처리하는데 너무 시간이 많이 걸려서
						//		일단 이렇게 작업하고, 나중에 꼭 수정하기로 나와 약속함
						{
							Common::SideType sideType = Common::SideNone;

							switch(tileProperty.mSide)
							{
							case Dungeon::Blueprint::SideEast:
								{
									sideType = Common::SideEast;
									break;
								}
							case Dungeon::Blueprint::SideNorth:
								{
									sideType = Common::SideNorth;
									break;
								}
							case Dungeon::Blueprint::SideWest:
								{
									sideType = Common::SideWest;
									break;
								}
							case Dungeon::Blueprint::SideSouth:
								{
									sideType = Common::SideSouth;
									break;
								}
							}

							bodyDoor->SetupBody(
								sceneManager,
								sceneNode,
								tilePosition,
								mTileProperty,
								sideType);
						}

						mDoorContainer.push_back(doorEntityName);

						// LUJ, Building door frames
						for(auto iterator = mTileProperty.mDoor.mFrameContainer.begin();
							iterator != mTileProperty.mDoor.mFrameContainer.end();
							++iterator)
						{
							const DoorProperty::Frame& frame = *iterator;
							
							const Ogre::Vector3 frameScale(
								frame.mScale.x * frame.mSize.x * mTileProperty.mWidth,
								frame.mScale.y * frame.mSize.y * mTileProperty.mHeight,
								frame.mScale.z * frame.mSize.z * mTileProperty.mWidth);

							Ogre::Vector3 framePosition(
								tilePosition.x + mTileProperty.mWidth * (1.0f - frame.mScale.x) / 2.0f - mTileProperty.mWidth * frame.mPosition.x,
								tilePosition.y + mTileProperty.mHeight * (1.0f - frame.mScale.y) / 2.0f - mTileProperty.mHeight * frame.mPosition.y,
								tilePosition.z + mTileProperty.mWidth * (1.0f - frame.mScale.z) / 2.0f - mTileProperty.mWidth * frame.mPosition.z);

							Ogre::Degree rotatingDegree(0);

							switch(tileProperty.mSide)
							{
							case Blueprint::SideEast:
								{
									rotatingDegree = -90.0f;
									break;
								}
							case Blueprint::SideNorth:
								{
									rotatingDegree = 0.0f;
									break;
								}
							case Blueprint::SideWest:
								{
									rotatingDegree = 90.0f;
									break;
								}
							case Blueprint::SideSouth:
								{
									rotatingDegree = 180.0f;
									break;
								}
							}

							Utility::RotateXZ(
								framePosition.x,
								framePosition.z,
								rotatingDegree,
								tilePosition,
								framePosition);

							const Ogre::Quaternion frameOrientation(rotatingDegree, Ogre::Vector3::UNIT_Y);

							Ogre::Entity* frameEntity = sceneManager.createEntity(
								"entity/door/frame" + Ogre::StringConverter::toString(Utility::GetUniqueIndex()),
								frame.mMeshFileName);
							frameEntity->setMaterialName(frame.mMaterialName);
							frameEntity->setQueryFlags(Common::QueryWall);

							dungeonStaticGeometry->addEntity(
								frameEntity,
								framePosition,
								frameOrientation,
								frameScale);

							// building collision box
							{
								Ogre::SceneNode* const childSceneNode = sceneNode.createChildSceneNode();
								childSceneNode->attachObject(frameEntity);
								//childSceneNode->showBoundingBox(true);
								childSceneNode->translate(framePosition);
								childSceneNode->setScale(frameScale);
								childSceneNode->setOrientation(frameOrientation);
								childSceneNode->setVisible(true);
							}
						}

						break;
					}
				}

				minPosition.x = std::min(minPosition.x, tilePosition.x);
				minPosition.z = std::min(minPosition.z, tilePosition.z);
				maxPosition.x = std::max(maxPosition.x, tilePosition.x);
				maxPosition.z = std::max(maxPosition.z, tilePosition.z);
			}

			// TODO, 크기를 조정안하고 크게 해보자
			{
				const float width = abs(maxPosition.x - minPosition.x);
				const float height = abs(maxPosition.z - minPosition.z);
				
				Ogre::MeshManager::getSingleton().createPlane(
					"floor.mesh",
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::Plane(Ogre::Vector3::UNIT_Y, 0),
					width,
					height,
					10,
					10,
					true,
					1,
					10,
					10,
					Ogre::Vector3::UNIT_Z);

				Ogre::Entity* floor = nullptr;
				const Ogre::String floorEntityName("entity/floor");

				if(true == sceneManager.hasEntity(floorEntityName))
				{
					floor = sceneManager.getEntity(floorEntityName);
				}
				else
				{
					floor = sceneManager.createEntity("entity/floor", "floor.mesh");
					floor->setMaterialName("map/dirt");
					floor->setCastShadows(false);
				}
				
				assert(floor);

				dungeonStaticGeometry->addEntity(
					floor,
					Ogre::Vector3(width / 2.0f, 0, height / 2.0f));

				Ogre::SceneNode* const floorNode = sceneNode.createChildSceneNode("floorNode");
				assert(floorNode);
				floorNode->attachObject(floor);
				floorNode->setVisible(true);

				//OgreNewt::World* const newtonWorld = Application::GetSingleton().GetNewtonWorld();
				// LUJ, 이걸 안하면 물리가 전혀 작동안한다! 몰라서 한참 헤맸다. 제기랄
				//newtonWorld->setWorldSize(Ogre::Vector3(-width, -2500, -height), Ogre::Vector3(width, 2500, height));

				//OgreNewt::CollisionPrimitives::TreeCollisionSceneParser* treeCollisionSceneParser = new OgreNewt::CollisionPrimitives::TreeCollisionSceneParser(newtonWorld);
				//treeCollisionSceneParser->parseScene(&sceneNode, true, OgreNewt::CollisionPrimitives::FW_REVERSE);
				//treeCollisionSceneParser->parseScene(&sceneNode, true, 1);

				//mFloorBody = new OgreNewt::Body(newtonWorld, OgreNewt::CollisionPtr(treeCollisionSceneParser));

#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
				delete treeCollisionSceneParser;
#endif
				//mFloorBody->attachNode(floorNode);
				//mFloorBody->setPositionOrientation(Ogre::Vector3(width / 2.0f, 0, height / 2.0f), Ogre::Quaternion::IDENTITY);
			}

			dungeonStaticGeometry->setCastShadows(false);
			//dungeonStaticGeometry->build();
		}
	}


	const POINT Manager::GetStartPoint() const
	{
		return mBlueprint->GetStartPoint();
	}


	void Manager::Save(const char* fileName) const
	{
		auto ConvertTileTypeToText = [](Blueprint::TileType type) -> const char*
		{
			switch(type)
			{
			case Blueprint::TileBlock:
				{
					return "block";
				}
			case Blueprint::TileBoundary:
				{
					return "boundary";
				}
			case Blueprint::TileCorridor:
				{
					return "corridor";
				}
			case Blueprint::TileDoor:
				{
					return "door";
				}
			case Blueprint::TileDownStairs:
				{
					return "downstair";
				}
			case Blueprint::TileFloor:
				{
					return "floor";
				}
			case Blueprint::TileNone:
				{
					return "none";
				}
			case Blueprint::TileUpStairs:
				{
					return "upstair";
				}
			case Blueprint::TileWall:
				{
					return "wall";
				}
			}

			return "?";
		};

		auto ConvertSideTypeToText = [](Blueprint::Side side) -> const char*
		{
			switch(side)
			{
			case Blueprint::SideEast:
				{
					return "east";
				}
			case Blueprint::SideWest:
				{
					return "west";
				}
			case Blueprint::SideNorth:
				{
					return "north";
				}
			case Blueprint::SideSouth:
				{
					return "south";
				}
			}

			return "?";
		};

		TiXmlDocument document;
		document.LinkEndChild(new TiXmlDeclaration("1.0", "", ""));

		TiXmlNode* const rootNode = document.LinkEndChild(new TiXmlElement("Blueprint"));

		// 타일 저장
		{
			TiXmlNode* const parentNode = rootNode->LinkEndChild(new TiXmlElement("tile"));

			for(Blueprint::TileIterator tileIterator = mBlueprint->GetTileIterator();
				true == tileIterator.hasMoreElements();
				tileIterator.moveNext())
			{
				auto iterator = tileIterator.current();
				const Blueprint::Tile& tile = iterator->first;
				const Blueprint::TileProperty& tileProperty = iterator->second;

				TiXmlElement* const element = new TiXmlElement("data");
				parentNode->LinkEndChild(element);

				element->SetAttribute("column", tile.first);
				element->SetAttribute("row", tile.second);
				element->SetAttribute("type", tileProperty.mType);
				element->SetAttribute("side", tileProperty.mSide);
			}
		}

		// 블록 저장
		{
			TiXmlNode* const parentNode = rootNode->LinkEndChild(new TiXmlElement("block"));

			for(Blueprint::BlockIterator blockIterator = mBlueprint->GetBlockIterator();
				true == blockIterator.hasMoreElements();
				blockIterator.moveNext())
			{
				const RECT& rect = *(blockIterator.current());

				TiXmlElement* const element = new TiXmlElement("data");
				parentNode->LinkEndChild(element);

				element->SetAttribute("left", rect.left);
				element->SetAttribute("top", rect.top);
				element->SetAttribute("right", rect.right);
				element->SetAttribute("bottom", rect.bottom);
			}
		}

		document.SaveFile(fileName);
	}


	bool Manager::Load(const char* fileName)
	{
		// LUJ, 문도 지도의 일부이므로 같이 제거해줘야한다.
		//		어쨌든 방법이 그리 깔끔하지 않은 건 사실이다..
		while(false == mDoorContainer.empty())
		{
			const Ogre::String& bodyName = *(mDoorContainer.begin());

			Application::GetSingleton().GetBodyFactory().RemoveBody(bodyName);

			mDoorContainer.pop_front();
		}

		return mBlueprint->Load(fileName);
	}


	void Manager::LoadTileProperty(const char* filePath)
	{
		mParser->LoadProperty(filePath, mTileProperty);

		mBlueprint->SetTileSize(mTileProperty.mWidth);
	}


	void Manager::Release()
	{
		Utility::SafeDelete(mFloorBody);
	}
}