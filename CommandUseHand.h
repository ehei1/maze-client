#pragma once


#include "Command.h"


enum HandType;


class CommandUseHand : public Command
{
public:
	CommandUseHand(const Event&);
	virtual ~CommandUseHand(void);

	virtual void Initialize(Body&);
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual void Release(Body&);
	virtual void SetValue(const Event&);


private:
	HandType mHandType;
};


