#include "StdAfx.h"
#include "CommandMoveLeftward.h"
#include "Event.h"
#include "Body.h"
#include "Utility.h"


CommandMoveLeftward::CommandMoveLeftward(const Event& event) :
Command(event),
mDistance(0)
{
	SetValue(event);
}


CommandMoveLeftward::~CommandMoveLeftward(void)
{

}


void CommandMoveLeftward::SetValue(const Event& event)
{
}


void CommandMoveLeftward::Initialize(Body& body)
{
	mMovingDirection = body.GetSceneNode()->getOrientation().xAxis();
	mDistance = body.GetDistance(mMovingDirection);

	if(0.0f < mDistance)
	{
		body.SetBackwardMoving(true);
		body.Move(mMovingDirection, mDistance);
		mBodyYaw = body.GetSceneNode()->getOrientation().getYaw();
	}
}


void CommandMoveLeftward::Release(Body& body)
{
	//body.Stop();

	Utility::OutputDebug(L"CommandMoveLeftward::Release()\n");
}


bool CommandMoveLeftward::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	if(1.0f >= std::abs(mDistance))
	{
		Utility::OutputDebug(L"CommandMoveLeftward stop...\n");
		return false;
	}
	else if(body.GetSceneNode()->getOrientation().getYaw() != mBodyYaw)
	{
		Initialize(body);
	}

	const Ogre::Vector3 movedPosition = body.GetCurrentSpeed() * frameEvent.timeSinceLastFrame * mMovingDirection;
	
	body.GetSceneNode()->translate(movedPosition);

	mDistance = mDistance - movedPosition.length();

	Utility::OutputDebug(L"Leftward moving. distance: %f moved length: %f, speed: %f\n", mDistance, movedPosition.length(), body.GetCurrentSpeed());
	return true;
}