#pragma once


#include "Soul.h"


class SoulMessage;


class SoulOrc : public Soul
{
public:
	SoulOrc();
	virtual ~SoulOrc();

	virtual void frameRendering(Body&, const Ogre::FrameEvent&);
	

private:
	virtual void OnStart(Body&, SoulMessage*);
	virtual void OnEnd(Body&, SoulMessage*);
};