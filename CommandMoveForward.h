#pragma once


#include "Command.h"


class CommandMoveForward : public Command
{
public:
	CommandMoveForward(const Event&);
	virtual ~CommandMoveForward(void);

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	Ogre::Radian mBodyYaw;
	Ogre::Vector3 mMovingDirection;
	Ogre::Real mDistance;	
};


