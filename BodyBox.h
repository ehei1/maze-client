#pragma once


#include "Body.h"


class BodyBox :
	public Body
{
	friend class BodyFactory;


public:
	virtual ~BodyBox();
	virtual void SetupBody(const Ogre::String& parentNodeName, const char* meshName);


protected:
	BodyBox();
};