#pragma once


#include "Command.h"


class CommandMoveRightward : public Command
{
public:
	CommandMoveRightward(const Event&);
	virtual ~CommandMoveRightward(void);

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	Ogre::Vector3 mMovingDirection;
	Ogre::Radian mBodyYaw;
	Ogre::Real mDistance;	
};


