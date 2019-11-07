#pragma once


#include "Processor.h"


class ProcessorMove : public Processor
{
public:
	ProcessorMove();
	virtual ~ProcessorMove();
	
	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual bool Receive(const Event&, Body&);
};