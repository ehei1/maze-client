#pragma once

class Box :
	public Ogre::MovableObject::Listener
{
public:
	enum State
	{
		StateNone,
		StateFall,
	};

public:
	Box();
	virtual ~Box();
	virtual void objectAttached(Ogre::MovableObject*);
	virtual void objectMoved(Ogre::MovableObject*);
	virtual bool objectRendering(const Ogre::MovableObject*, const Ogre::Camera*);
	virtual void objectDestroyed(Ogre::MovableObject*);

private:
	State SetPositionRelatively(const Ogre::MovableObject&, const Ogre::Vector3& targetPosition, const Ogre::Vector3& speed) const;

public:
	static Ogre::MovablePlane mPlane;

private:
	State mState;
	Ogre::RaySceneQuery* mRaySceneQuery;
};