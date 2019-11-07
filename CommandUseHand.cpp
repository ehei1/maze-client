#include "StdAfx.h"
#include "CommandUseHand.h"
#include "Event.h"
#include "Body.h"
#include "EventCommon.h"


CommandUseHand::CommandUseHand(const Event& event) :
Command(event)
{
	SetValue(event);
}


CommandUseHand::~CommandUseHand(void)
{

}


void CommandUseHand::SetValue(const Event& event)
{
	event.GetValue(mHandType, "hand");
}


void CommandUseHand::Initialize(Body& body)
{
	
}


void CommandUseHand::Release(Body& body)
{
	
}


bool CommandUseHand::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	switch(mHandType)
	{
	case HandLeft:
		{
			body.UseLeftHand();
			break;
		}
	case HandRight:
		{
			body.UseRightHand();
			break;
		}
	}

	return false;
}