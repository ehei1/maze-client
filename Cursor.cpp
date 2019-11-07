#include <StdAfx.h>
#include "Cursor.h"
#include "Application.h"
#include "CommonDefine.h"
#include "Body.h"
#include "ToolTip.h"
#include "Utility.h"


Cursor::Cursor(Ogre::SceneManager& sceneManager, Ogre::SceneNode& parentSceneNode, const Ogre::Vector2& radius) :
mCursorObject(nullptr),
// accuracy is the count of points (and lines).
// Higher values make the mCursorObject smoother, but may slowdown the performance.
// The performance also is related to the count of mCursorObjects.
mAccuracy(35),
mEnableMaterialName("SinglePlay/material/enableCursor"),
mDisableMaterialName("SinglePlay/material/disableCursor"),
mCircleType(CircleClose),
mPlane(Ogre::Vector3::UNIT_Y, 0),
mCursorPosition(Ogre::Vector3::ZERO),
mSceneQuery(new Ogre::DefaultAxisAlignedBoxSceneQuery(&sceneManager)),
mRaySceneQuery(new Ogre::DefaultRaySceneQuery(&sceneManager))
{
	SetRadius(radius);

	mPlane.setQueryFlags(Common::QueryPlane);

	// make cursor object
	{
		const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());
		
		mCursorObject = sceneManager.createManualObject(
			"manual/cursor" + uniqueIndex);
		mCursorObject->setQueryFlags(Common::QueryNone);
		mCursorObject->setCastShadows(false);

		Ogre::SceneNode* const sceneNode = parentSceneNode.createChildSceneNode(
			"node/cursor" + uniqueIndex);
		sceneNode->attachObject(mCursorObject);
	}

	AddMaterial(mEnableMaterialName, Ogre::ColourValue::Green);
	AddMaterial(mDisableMaterialName, Ogre::ColourValue::Red);
}


Cursor::~Cursor()
{
}


void Cursor::AddMaterial(const char* materialName, const Ogre::ColourValue& colourValue)
{
	const char* const groupName = "Mesh";

	Ogre::MaterialManager& materialManager = Ogre::MaterialManager::getSingleton();
	Ogre::ResourcePtr resourcePointer = materialManager.getByName(materialName);

	if(nullptr == resourcePointer.get())
	{
		Ogre::MaterialPtr materialPointer = materialManager.create(
			materialName,
			groupName);
		materialPointer->setReceiveShadows(false);
		materialPointer->getTechnique(0)->setLightingEnabled(true);
		materialPointer->getTechnique(0)->getPass(0)->setDiffuse(colourValue);
		materialPointer->getTechnique(0)->getPass(0)->setAmbient(colourValue);
		materialPointer->getTechnique(0)->getPass(0)->setSelfIllumination(colourValue);
	}
}


void Cursor::frameRendering(const Ogre::FrameEvent& frameEvent)
{
	switch(mCircleType)
	{
	case CircleOpen:
		{
			DrawOpenedCircle(frameEvent, mCursorPosition);
			break;
		}
	case CircleClose:
		{
			DrawClosedCircle(frameEvent, mCursorPosition);
			break;
		}
	}

	// update radius
	{
		mRadiusValue.mBias += frameEvent.timeSinceLastFrame;

		if(mRadius.x + mRadiusValue.mBias > mRadius.y)
		{
			mRadius *= -1;
			std::swap(mRadius.x, mRadius.y);

			mRadiusValue.mFactor *= -1;
			mRadiusValue.mBias = 0;
		}
	}
}


// http://www.ogre3d.org/tikiwiki/Circle3D
void Cursor::DrawClosedCircle(const Ogre::FrameEvent& frameEvent, const Ogre::Vector3& cursorPosition)
{
	mCursorObject->clear();
	mCursorObject->begin(mEnableMaterialName, Ogre::RenderOperation::OT_LINE_STRIP);

	const Ogre::Real radius = GetRadius();
	unsigned int pointIndex = 0;

	for(float theta = 0;
		theta <= Ogre::Math::TWO_PI;
		theta += Ogre::Math::PI / mAccuracy, ++pointIndex)
	{
		const Ogre::Vector3 position(
			radius * cos(theta),
			1,
			radius * sin(theta));

		mCursorObject->position(position + cursorPosition);
		mCursorObject->index(pointIndex);
	}

	// Rejoins the last point to the first.
	mCursorObject->index(0);
	mCursorObject->end();
}


void Cursor::DrawOpenedCircle(const Ogre::FrameEvent& frameEvent, const Ogre::Vector3& cursorPosition)
{
	mCursorObject->clear();
	mCursorObject->begin(mDisableMaterialName, Ogre::RenderOperation::OT_LINE_LIST);

	const Ogre::Real radius = GetRadius();
	unsigned int pointIndex = 0;

	for(float theta = 0 + mRadiusValue.mBias;
		theta <= Ogre::Math::TWO_PI + mRadiusValue.mBias;
		theta += Ogre::Math::PI / mAccuracy, ++pointIndex)
	{
		const Ogre::Vector3 position(
			radius * cos(theta),
			1,
			radius * sin(theta));

		mCursorObject->position(position + cursorPosition);
		mCursorObject->index(pointIndex);
	}

	mCursorObject->end();
}


void Cursor::SetRadius(const Ogre::Vector2& radius)
{
	mRadius = radius;
	mRadiusValue.mBias = 0;
	mRadiusValue.mFactor = (radius.x <= radius.y ? 1 : -1);
}


void Cursor::mouseMoved(const OIS::MouseEvent& mouseEvent, const Ogre::Vector3& soulPosition)
{
	mCursorObject->setVisible(true);

	// put ray, get intersect point.
	// http://www.ogre3d.org/addonforums/viewtopic.php?f=8&t=13870
	{
		Ogre::Camera* const camera = Application::GetSingleton().GetCameraMan()->getCamera();
		assert(nullptr != camera);

		const Ogre::Ray ray = camera->getCameraToViewportRay(
			float(mouseEvent.state.X.abs) / mouseEvent.state.width,
			float(mouseEvent.state.Y.abs) / mouseEvent.state.height);

		mRaySceneQuery->setRay(ray);
		mRaySceneQuery->setSortByDistance(true);
		mRaySceneQuery->setQueryMask(Common::QueryItem | Common::QueryCreature | Common::QueryDoor);
		const Ogre::RaySceneQueryResult& sceneQueryResult = mRaySceneQuery->execute();

		const std::pair< bool, Ogre::Real > result = ray.intersects(mPlane);

		if(true == result.first)
		{
			const Ogre::Real distance = result.second;
			mMouseCursorPosition = ray.getOrigin() + ray.getDirection() * distance;
		}

		if(false == sceneQueryResult.empty())
		{
			const Ogre::RaySceneQueryResult::const_iterator iterator = sceneQueryResult.begin();

			if(nullptr != iterator->movable)
			{
				mCursorPosition = iterator->movable->getParentNode()->getPosition();
			}
		}
		else
		{
			mCursorPosition = mMouseCursorPosition;
		}
	}

	const Ogre::Real visionRange(20.0f);

	if(const bool isValidDistanceByCursor = (visionRange > Ogre::Vector3(mCursorPosition - soulPosition).length()))
	{
		mCircleType = CircleClose;
	}
	else
	{
		mCircleType = CircleOpen;
	}

	mTooltipContainer.clear();

	// show tooltip
	{
		const Ogre::AxisAlignedBox& cursorBoudingBox = mCursorObject->getBoundingBox();
		
		mSceneQuery->setBox(
			Ogre::AxisAlignedBox(mCursorPosition - cursorBoudingBox.getHalfSize(), mCursorPosition + cursorBoudingBox.getHalfSize()));		
		mSceneQuery->setQueryMask(
			Common::QueryItem | Common::QueryCreature | Common::QueryDoor);

		const Ogre::SceneQueryResult& sceneQueryResult = mSceneQuery->execute();

		for(auto iterator = sceneQueryResult.movables.begin();
			sceneQueryResult.movables.end() != iterator;
			++iterator)
		{
			const Ogre::MovableObject* const movableObject = *iterator;

			if(Body* const body = dynamic_cast< Body* >(movableObject->getListener()))
			{
				printf("%d %s\n", GetTickCount(), body->GetEntity()->getName().c_str());

				boost::shared_ptr< Tooltip > tooltip;

				switch(mCircleType)
				{
				case CircleOpen:
					{
						tooltip = boost::shared_ptr< Tooltip >(new Tooltip);
						tooltip->AddCaption(
							Ogre::ColourValue::Red,
							Ogre::ColourValue::Red,
							_T("?"));
						break;
					}
				case CircleClose:
					{
						tooltip = boost::shared_ptr< Tooltip >(new Tooltip(body->GetTooltip()));
						break;
					}
				}

				tooltip->SetVisible(true);
				tooltip->SetPosition(
					movableObject->getParentSceneNode()->getPosition() + Ogre::Vector3(0, movableObject->getBoundingRadius() * 2.0f, 0));

				const TooltipUnit tooltipUnit(tooltip, movableObject->getName());
				mTooltipContainer.push_back(tooltipUnit);
			}
		}
	}
}


void Cursor::keyPressed(const OIS::KeyEvent&)
{
	mCursorObject->setVisible(false);
	mTooltipContainer.clear();
}


void Cursor::keyReleased(const OIS::KeyEvent&)
{
	mCursorObject->setVisible(false);
	mTooltipContainer.clear();
}


Body* Cursor::GetTooltipBody(Common::QueryType queryType) const
{
	for(TooltipContainer::const_iterator iterator = mTooltipContainer.begin();
		mTooltipContainer.end() != iterator;
		++iterator)
	{
		const TooltipUnit& tooltipUnit = *iterator;
		const Ogre::String& nodeName = tooltipUnit.second;
		Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();

		if(false == sceneManager.hasEntity(nodeName))
		{
			continue;
		}

		Ogre::Entity* const entity = sceneManager.getEntity(nodeName);

		if(0 == (entity->getQueryFlags() & queryType))
		{
			continue;
		}
		else if(Body* const body = dynamic_cast< Body* >(entity->getListener()))
		{
			return body;
		}
	}

	return nullptr;
}