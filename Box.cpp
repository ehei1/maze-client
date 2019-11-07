#include <StdAfx.h>
#include "Box.h"
#include "Utility.h"
#include "Application.h"


Ogre::MovablePlane Box::mPlane("None/plane/base");


Box::Box() :
mState(StateNone),
mRaySceneQuery(nullptr)
{

}


Box::~Box()
{}


void Box::objectMoved(Ogre::MovableObject* movableObject)
{
}


void Box::objectAttached(Ogre::MovableObject* movableObject)
{
	mRaySceneQuery = Application::GetSingleton().GetSceneManager().createRayQuery(Ogre::Ray());
	mState = StateFall;
	movableObject->getParentSceneNode()->showBoundingBox(true);
}


bool Box::objectRendering(const Ogre::MovableObject* movableObject, const Ogre::Camera* camera)
{
	switch(mState)
	{
	case StateFall:
		{
			const Ogre::Vector3& position = movableObject->getParentNode()->getPosition();

			// check with other objects
			{
				const Ogre::Ray ray(
					movableObject->getParentNode()->getPosition() + Ogre::Vector3::UNIT_Y * movableObject->getBoundingRadius() + 1,
					Ogre::Vector3::NEGATIVE_UNIT_Y);

				mRaySceneQuery->clearResults();
				mRaySceneQuery->setRay(ray);
				mRaySceneQuery->setSortByDistance(true);

				Ogre::RaySceneQueryResult& raySceneQueryResult = mRaySceneQuery->execute();

				for(Ogre::RaySceneQueryResult::iterator iterator = raySceneQueryResult.begin();
					raySceneQueryResult.end() != iterator;
					++iterator)
				{
					const Ogre::RaySceneQueryResultEntry& raySceneQueryResultEntry = *iterator;
					Ogre::MovableObject* const collidedMovableObject = raySceneQueryResultEntry.movable;

					if(nullptr == collidedMovableObject)
					{
						continue;
					}
					else if(collidedMovableObject == movableObject)
					{
						continue;
					}

					mState = SetPositionRelatively(
						*movableObject,
						Ogre::Vector3(position.x, collidedMovableObject->getParentNode()->getPosition().y + movableObject->getBoundingRadius(), position.z),
						Ogre::Vector3(0, -5, 0));
					break;
				}
			}

			break;
		}
	}

	return Ogre::MovableObject::Listener::objectRendering(movableObject, camera);
}


void Box::objectDestroyed(Ogre::MovableObject*)
{
	if(mRaySceneQuery)
	{
		Application::GetSingleton().GetSceneManager().destroyQuery(mRaySceneQuery);
		mRaySceneQuery = nullptr;
	}

	// if you use it, memory was leaked. sorry.
	// TODO: There is memory leak! How can delete it without inserting container?
}


Box::State Box::SetPositionRelatively(const Ogre::MovableObject& movableObject, const Ogre::Vector3& targetPosition, const Ogre::Vector3& speed) const
{
	const Ogre::Vector3& position = movableObject.getParentNode()->getPosition();
	const Ogre::Vector3 movedPosition(
		position.x + speed.x,
		std::max(targetPosition.y, position.y + speed.y),
		position.z + speed.z);

	movableObject.getParentNode()->setPosition(
		movedPosition);

	return targetPosition.y == movedPosition.y ? StateNone : StateFall;
}