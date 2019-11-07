#pragma once


#include "Command.h"


class CommandMoveStop : public Command
{
public:
	CommandMoveStop(const Event&);
	virtual ~CommandMoveStop(void);

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	EventType mStoppingEventType;
};


