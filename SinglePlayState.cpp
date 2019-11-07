#include <StdAfx.h>
#include "SinglePlayState.h"
#include "SoulController.h"
#include "Application.h"
#include "Utility.h"
#include "EntityLoadListener.h"
#include "Body.h"
#include "Box.h"
#include "BodyFactory.h"
#include "CommonDefine.h"
#include "DebugManager.h"
#include "Tooltip.h"
#include "Cursor.h"
#include "DungeonManager.h"
#include "CustomSceneManager.h"


extern std::list<RenderableOcclusionPair*> g_RenderableOcclusionPairList;

RenderableOcclusionPair* renderableOcclusionPair = nullptr;


namespace State
{
	SinglePlayState::SinglePlayState(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager) :
	State(name, application, sceneManager),
	mSceneNode(nullptr),
	mDungeonNode(nullptr),
	mSoulController(nullptr),
	mDungeonManager(new Dungeon::Manager)
	{
	}
	
	SinglePlayState::~SinglePlayState()
	{
	}

	void SinglePlayState::enter()
	{
		mSceneNode = (Ogre::SceneNode*)mSceneManager.getRootSceneNode()->createChildSceneNode(
			GetName() + "/node/root");

		mSoulController.reset(
			new SoulController(mSceneManager, *mSceneNode));
		
		PutCreature();
		RandomizeDungeon(20, 20);
		SyncPlayer();

		CEGUI::System::getSingleton().executeScriptFile("singlePlay.enter.lua");

		mApplication.GetNewtonWorld()->getDebugger().init(&mSceneManager);
	}

	void SinglePlayState::pause()
	{
		mSceneManager.getRootSceneNode()->removeChild(mSceneNode);
	}

	void SinglePlayState::resume()
	{
		mSceneManager.getRootSceneNode()->addChild(mSceneNode);
	}

	void SinglePlayState::exit()
	{
		mSoulController.reset(nullptr);
		mApplication.GetBodyFactory().Release();
		mApplication.GetNewtonWorld()->getDebugger().deInit();

		mSceneNode->removeAndDestroyAllChildren();
		mSceneManager.destroySceneNode(mSceneNode);
		mSceneManager.destroyAllMovableObjects();

		mDungeonManager->Release();

		mSceneNode = nullptr;
		mDungeonNode = nullptr;

		CEGUI::System::getSingleton().executeScriptFile("singlePlay.exit.lua");

		mApplication.GetCameraMan()->getCamera()->setAutoTracking(false);
		mApplication.GetCameraMan()->setTarget(nullptr);
	}

	void SinglePlayState::RandomizeDungeon(int row, int column)
	{
		if(nullptr == mDungeonNode)
		{
			mDungeonNode = mSceneNode->createChildSceneNode();
		}
		else
		{
			mDungeonNode->removeAndDestroyAllChildren();

			//for(Ogre::SceneNode::ChildNodeIterator nodeIterator = mDungeonNode->getChildIterator();
			//	true == nodeIterator.hasMoreElements();
			//	nodeIterator.moveNext())
			//{
			//	//const Ogre::String& name = nodeIterator.peekNextKey();
			//	Ogre::SceneNode* const sceneNode = (Ogre::SceneNode*)nodeIterator.peekNextValue();

			//	sceneNode->removeAndDestroyAllChildren();
			//}
		}

		mDungeonManager->RandomizeBlueprint(row, column);
		mDungeonManager->Build(*mDungeonNode, mSceneManager);
	}


	bool SinglePlayState::frameStarted(const Ogre::FrameEvent& frameEvent)
	{
#ifdef _DEBUG
		{
			OgreNewt::Debugger& debug(Application::GetSingleton().GetNewtonWorld()->getDebugger());
			debug.showDebugInformation();
			debug.startRaycastRecording();
			debug.clearRaycastsRecorded();
		}
#endif

		return State::frameStarted(frameEvent);
	}


	bool SinglePlayState::frameRenderingQueued(const Ogre::FrameEvent& frameEvent)
	{
		if(nullptr != mSoulController.get())
		{
			if(true == mSoulController->frameRendering(frameEvent))
			{
				const unsigned int pixelCountForTurningOffShadow = 100;

				if(pixelCountForTurningOffShadow > renderableOcclusionPair->GetPixelCount())
				{
					mSoulController->GetBody()->TurnOnShadow();
				}
				else
				{
					mSoulController->GetBody()->TurnOffShadow();
				}
			}
		}

		mApplication.GetBodyFactory().frameRendering(frameEvent);

		if(false == mEntityListenerContainer.empty())
		{
			EntityLoadListener* entityLoadListener = mEntityListenerContainer.front();
			mEntityListenerContainer.pop();

			if(entityLoadListener->IsDestroy())
			{
				Utility::SafeDelete(entityLoadListener);
			}
			else
			{
				entityLoadListener->Load();

				// 라운드 로빈마냥 공정하게 처리되도록 후미에 넣는다
				mEntityListenerContainer.push(entityLoadListener);
			}
		}

		return true;
	}


	void SinglePlayState::PutCreature()
	{
		Body* const body = mApplication.GetBodyFactory().AddBody(
			BodyFactory::TypeOrc,
			mSceneNode->getName(),
			"entity/hero");

		mSoulController->SetBody(body);

		mSceneManager.setShadowTechnique(
			//	Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
			Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
			//	Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
		mSceneManager.setAmbientLight(Ogre::ColourValue::ZERO);

		// add light to player
		{
			Ogre::Light* const light = mSceneManager.createLight("playerLight");
			light->setType(Ogre::Light::LT_POINT);
			light->setDiffuseColour(0.9, 0.9, 0.9);
			//light->setSpecularColour(0.9, 0.9, 0.9);

			Ogre::SceneNode* const lightNode = body->GetSceneNode()->createChildSceneNode();
			lightNode->attachObject(light);
			lightNode->setPosition(0, 3, 3);
		}

		// add billboard to body for hardware occlusion query
		{
			Ogre::BillboardSet* billboardSet = new Ogre::BillboardSet("A super unique name", 1);
			assert(billboardSet);

			billboardSet->setMaterialName("hoq_blank");
			billboardSet->setBillboardOrigin(Ogre::BBO_CENTER);

			Ogre::Billboard* const billboard = billboardSet->createBillboard(Ogre::Vector3::ZERO);
			assert(billboard);

			const Ogre::Real billBoardSize = body->GetEntity()->getBoundingRadius();

			billboard->setDimensions(billBoardSize, billBoardSize);

			billboardSet->_updateBounds();

			body->GetSceneNode()->attachObject(billboardSet);

			renderableOcclusionPair = new RenderableOcclusionPair(billboardSet);
			assert(renderableOcclusionPair);

			g_RenderableOcclusionPairList.push_back(renderableOcclusionPair);
		}
	}

	bool SinglePlayState::keyPressed(const OIS::KeyEvent& keyEvent)
	{
		mSoulController->keyPressed(keyEvent);

		switch(keyEvent.key)
		{
		case OIS::KC_TAB:
			{
				//OgreBites::SdkCameraMan* const cameraMan = mApplication.GetCameraMan();

				//switch(cameraMan->getStyle())
				//{
				//case OgreBites::CS_FREELOOK:
				//	{
				//		cameraMan->setStyle(OgreBites::CS_ORBIT);
				//		cameraMan->getCamera()->setAutoTracking(true, mSoulController->GetSceneNode());
				//		break;
				//	}
				//case OgreBites::CS_ORBIT:
				//	{
				//		cameraMan->getCamera()->setAutoTracking(false);
				//		cameraMan->setStyle(OgreBites::CS_FREELOOK);
				//		break;
				//	}
				//}

				break;
			}
		case OIS::KC_N:
			{
				// put sword at cursor
				{
					Body* const body = mApplication.GetBodyFactory().AddBody(
						BodyFactory::TypeSword,
						mSceneNode->getName(),
						"entity/sword" + Ogre::StringConverter::toString(Utility::GetUniqueIndex()));
					body->GetSceneNode()->setPosition(mSoulController->GetCursor().GetPosition());
					body->GetSceneNode()->yaw(Ogre::Radian(Ogre::Degree(rand() % 360)));
				}

				break;
			}
		case OIS::KC_SPACE:
			{
				//static int count = 0;

				//Body* const body = mApplication.GetBodyFactory().AddBody(
				//	BodyFactory::TypeBox,
				//	mSceneNode->getName(),
				//	Ogre::String("Box") + Ogre::StringConverter::toString(++count));

				//body->GetSceneNode()->setScale(3, 3, 3);

				//OgreNewt::CollisionPrimitives::Box* boxPtr = new OgreNewt::CollisionPrimitives::Box(
				//	mApplication.GetNewtonWorld(),
				//	body->GetSceneNode()->getScale() * body->GetEntity()->getBoundingBox().getSize(),
				//	count);
				//OgreNewt::CollisionPtr collisionPtr(boxPtr);

				//OgreNewt::Body* const newtonBody = new OgreNewt::Body(mApplication.GetNewtonWorld(), collisionPtr);				
				//body->SetNewtonBody(newtonBody);

				//Ogre::Camera* const camera = mApplication.GetCameraMan()->getCamera();
				//// we get the position and direction from the camera...
				//Ogre::Quaternion camorient = camera->getDerivedOrientation();
				//Ogre::Vector3 direction = camorient * Ogre::Vector3(0, 0, -1);
				//// then make the visual object (again a cylinder)
				//Ogre::Vector3 position = camera->getDerivedPosition();

				//newtonBody->attachNode(body->GetSceneNode());
				//newtonBody->setPositionOrientation(position, camorient);

				//Ogre::Real mass = 10.0;
				//Ogre::Vector3 inertia, centerOfMass;
				//boxPtr->calculateInertialMatrix(inertia, centerOfMass);

				//inertia *= mass;
				//newtonBody->setMassMatrix(mass, inertia);
				//newtonBody->setCenterOfMass(centerOfMass);
				//newtonBody->setVelocity(direction * 50.0);
				//newtonBody->setStandardForceCallback();
				break;
			}
		}

		return State::keyPressed(keyEvent);
	}

	bool SinglePlayState::keyReleased(const OIS::KeyEvent& keyEvent)
	{
		mSoulController->keyReleased(keyEvent);

		return State::keyReleased(keyEvent);
	}

	bool SinglePlayState::mouseMoved(const OIS::MouseEvent& mouseEvent)
	{
		mSoulController->mouseMoved(mouseEvent);

		return State::mouseMoved(mouseEvent);
	}

	bool SinglePlayState::mousePressed(const OIS::MouseEvent& mouseEvent, OIS::MouseButtonID mouseButtonId)
	{
		mSoulController->mousePressed(
			mouseEvent,
			mouseButtonId);

		return State::mousePressed(mouseEvent, mouseButtonId);
	}


	bool SinglePlayState::mouseReleased(const OIS::MouseEvent& mouseEvent, OIS::MouseButtonID mouseButtonId)
	{
		mSoulController->mouseReleased(
			mouseEvent,
			mouseButtonId);

		return State::mouseReleased(mouseEvent, mouseButtonId);
	}


	void SinglePlayState::CreateCollisionField()
	{
		Ogre::Vector3 size(2.0,0.5,2.0);
		Ogre::Entity* floor;
		Ogre::SceneNode* floornode;
		const char *levelName = "track.mesh";
		//	const char *levelName = "simple_terrain.mesh";

		floor = mSceneManager.createEntity("SinglePlay/entity/field", levelName );
		floornode = mSceneNode->createChildSceneNode(GetName() + "/node/field" );
		floornode->attachObject( floor );
		//	floor->setMaterialName( "Simple/BeachStones" );
		floornode->setScale(size);
		floor->setCastShadows( false );

		char serializeCollisionName[256];
		strcpy (serializeCollisionName, levelName);
		strtok (serializeCollisionName, ".");
		strcat (serializeCollisionName, ".col");

		FILE* file = fopen (serializeCollisionName, "rb");
		if (!file) {
			// serialize the mesh so that next time loads faster
			OgreNewt::CollisionSerializer saveLevelCollision; 
			OgreNewt::CollisionPtr tmpSphape = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision( mApplication.GetNewtonWorld(), floor, true, 0 ));
			saveLevelCollision.exportCollision(tmpSphape, serializeCollisionName);
			file = fopen (serializeCollisionName, "rb");
		}

		// load level collision from serialized file
		Ogre::FileHandleDataStream streamFile (file);
		OgreNewt::CollisionSerializer loadLevelCollision; 
		OgreNewt::CollisionPtr col = loadLevelCollision.importCollision(streamFile, mApplication.GetNewtonWorld());
		fclose (file);
		OgreNewt::Body* bod = new OgreNewt::Body( mApplication.GetNewtonWorld(), OgreNewt::CollisionPtr(col));
		bod->attachNode( floornode );

		//	bod->setPositionOrientation( Ogre::Vector3(0.0,-2.0,0.0), Ogre::Quaternion::IDENTITY );

		// set the world bound to the size of the world AABB
		Ogre::AxisAlignedBox worldAABB (bod->getAABB());
		worldAABB.getMinimum().y -= 1000.0f;
		worldAABB.getMaximum().y += 1000.0f;
		mApplication.GetNewtonWorld()->setWorldSize (worldAABB.getMinimum(), worldAABB.getMaximum());
	}

	void SinglePlayState::PutField(const Ogre::Vector2& blockCount, Ogre::Real blockSize)
	{
		const Ogre::Real size(blockSize);
		const Ogre::Vector2 rectangleCount(blockCount);
		const Ogre::Real width = size * rectangleCount.x;
		const Ogre::Real height = size * rectangleCount.y;

		DebugManager::LinePositionContainer linePositionContainer;

		for(Ogre::Real count = std::max(rectangleCount.x, rectangleCount.y);
			0 <= count;
			--count)
		{
			const Ogre::Real row = std::min(rectangleCount.y, count) * size;
			const Ogre::Real column = std::min(rectangleCount.x, count) * size;

			linePositionContainer.push_back(
				DebugManager::LinePosition(Ogre::Vector3(0, 0, row), Ogre::Vector3(width, 0, row)));
			linePositionContainer.push_back(
				DebugManager::LinePosition(Ogre::Vector3(column, 0, 0), Ogre::Vector3(column, 0, height)));
		}

		mApplication.GetDebugManager().DrawLine(
			linePositionContainer,
			Ogre::ColourValue::Green);
	}


	void SinglePlayState::LoadDungeon(const char* fileName)
	{
		mSceneManager.destroyAllStaticGeometry();
		mDungeonNode->removeAndDestroyAllChildren();

		mDungeonManager->Load(fileName);
		mDungeonManager->Build(*mDungeonNode, mSceneManager);
	}


	Dungeon::Manager& SinglePlayState::GetDungeonManager()
	{
		return *mDungeonManager;
	}


	void SinglePlayState::SyncPlayer()
	{
		if(Body* const body = mSoulController->GetBody())
		{
			const POINT& startPoint = mDungeonManager->GetStartPoint();

			body->GetSceneNode()->setPosition(
				startPoint.x,
				5,
				startPoint.y);
			//body->GetNewtonBody()->setPositionOrientation(
			//	body->GetSceneNode()->getPosition(),
			//	body->GetSceneNode()->getOrientation());

			const Ogre::Vector3& bodyPosition = body->GetSceneNode()->getPosition();
			const Ogre::Real distance = 70.0f;
			const Ogre::Vector3 direction = Utility::GetDirection(*(body->GetSceneNode()));
			const Ogre::Vector3 relativeCameraPosition = direction * -distance;
			const Ogre::Real cameraHeight = 80.0f;

			auto camera = mApplication.GetCameraMan()->getCamera();
			camera->setPosition(
				bodyPosition.x + relativeCameraPosition.x,
				bodyPosition.y + cameraHeight,
				bodyPosition.z + relativeCameraPosition.z);
			camera->setAutoTracking(
				true,
				body->GetSceneNode(),
				bodyPosition - camera->getPosition());

			mSoulController->SetCamera(camera);
		}
	}


	void SinglePlayState::RebuildDungeon()
	{
		mDungeonNode->removeAndDestroyAllChildren();

		mDungeonManager->Build(*mDungeonNode, mSceneManager);
	}

	//void SinglePlayState::PutBox(const Ogre::Vector3& position, Ogre::Real height)
	//{
	//	const Ogre::String unqueIndex = Ogre::StringConverter::toString(mApplication.GetUniqueIndex());
	//	EntityLoadListener::Parameter parameter;
	//	parameter.mMeshName = "box.mesh";
	//	parameter.mMaterialName = "box.material";
	//	parameter.mEntityName = GetName() + "/entity/box" + unqueIndex;
	//	parameter.mNodeName = GetName() + "/node/box" + unqueIndex;
	//	parameter.mParentNodeName = mSceneNode->getName();
	//	parameter.mPosition = position + Ogre::Vector3(0, height, 0);
	//	parameter.mScale = Ogre::Vector3(5, 5, 5);
	//	parameter.mListener = new Box(mApplication.GetSceneManager());

	//	mEntityListenerContainer.push(new EntityLoadListener(parameter));
	//}
}