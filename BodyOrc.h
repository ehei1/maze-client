#pragma once


#include "Body.h"


class BodyOrc :
	public Body
{
	friend class BodyFactory;


public:
	bool Attach(Body&);

	virtual void UseLeftHand();
	virtual void UseRightHand();


protected:
	virtual void frameRendering(const Ogre::FrameEvent&);


private:
	BodyOrc();
	virtual ~BodyOrc();

	void SetupBody(const Ogre::String& parentNodeName, const char* meshName, Common::QueryType);

	void setupAnimations();
	void updateBody(Ogre::Real deltaTime);
	void updateAnimations(Ogre::Real deltaTime);
	void fadeAnimations(Ogre::Real deltaTime);


private:
	Ogre::Entity* mSword1;
	Ogre::Entity* mSword2;
	Ogre::RibbonTrail* mSwordTrail;
	bool mSwordsDrawn;
	Ogre::Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
	Ogre::Real mVerticalVelocity;     // for jumping
	Ogre::Real mTimer;                // general timer to see how long animations have been playing

	typedef std::map< Ogre::String, Ogre::Entity* > AttachedEntityContainer;
	AttachedEntityContainer mAttachedEntityContainer;
};