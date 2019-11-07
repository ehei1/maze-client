#pragma once


#include "Command.h"


class CommandMoveBackward : public Command
{
public:
	CommandMoveBackward(const Event&);
	virtual ~CommandMoveBackward(void);

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	Ogre::Vector3 mMovingDirection;
	Ogre::Radian mBodyYaw;

	Ogre::Real mDistance;	
};


