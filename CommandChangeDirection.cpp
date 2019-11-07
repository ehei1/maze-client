#include "StdAfx.h"
#include "CommandChangeDirection.h"
#include "Event.h"
#include "Body.h"


CommandChangeDirection::CommandChangeDirection(const Event& event) :
Command(event)
{
	SetValue(event);
}


CommandChangeDirection::~CommandChangeDirection(void)
{
}



void CommandChangeDirection::SetValue(const Event& event)
{
	event.GetValue(mDirection, "direction");
}


void CommandChangeDirection::Initialize(Body& body)
{}


bool CommandChangeDirection::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	body.GetSceneNode()->setDirection(
		mDirection,
		Ogre::Node::TS_WORLD,
		Ogre::Vector3::UNIT_Z);
	return true;
}


void CommandChangeDirection::Release(Body&)
{

}