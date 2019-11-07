#include <StdAfx.h>
#include "BodySword.h"
#include "Tooltip.h"
#include "CommonDefine.h"


BodySword::BodySword()
{
	
}


BodySword::~BodySword()
{
}


void BodySword::SetupBody(const Ogre::String& parentNodeName, const char* meshName)
{
	Body::SetupBody(parentNodeName, meshName, Common::QueryItem);

	GetTooltip().AddCaption(
		Ogre::ColourValue(0.5, 0.7, 0.5),
		Ogre::ColourValue(0.3, 0.5, 0.3),
		_T("평범한 오크라면 버릴 칼"));
}