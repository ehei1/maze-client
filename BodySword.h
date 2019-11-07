#pragma once


#include "Body.h"


class BodySword :
	public Body
{
	friend class BodyFactory;


public:
	virtual ~BodySword();
	virtual void SetupBody(const Ogre::String& parentNodeName, const char* meshName);


protected:
	BodySword();
};