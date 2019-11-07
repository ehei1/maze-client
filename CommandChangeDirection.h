#pragma once


#include "Command.h"


class CommandChangeDirection : public Command
{
public:
	CommandChangeDirection(const Event&);
	virtual ~CommandChangeDirection();

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	Ogre::Vector3 mDirection;
};


