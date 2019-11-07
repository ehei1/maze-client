#include <StdAfx.h>
#include "BodyBox.h"
#include "Tooltip.h"
#include "CommonDefine.h"


BodyBox::BodyBox()
{
	
}


BodyBox::~BodyBox()
{
}


void BodyBox::SetupBody(const Ogre::String& parentNodeName, const char* meshName)
{
	Body::SetupBody(parentNodeName, meshName, Common::QueryBox);
}