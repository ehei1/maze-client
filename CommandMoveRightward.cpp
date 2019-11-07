#include "StdAfx.h"
#include "CommandMoveRightward.h"
#include "Event.h"
#include "Body.h"
#include "Utility.h"


CommandMoveRightward::CommandMoveRightward(const Event& event) :
Command(event),
mDistance(0)
{
	SetValue(event);
}


CommandMoveRightward::~CommandMoveRightward(void)
{

}


void CommandMoveRightward::SetValue(const Event& event)
{
}


void CommandMoveRightward::Initialize(Body& body)
{
	mMovingDirection = body.GetSceneNode()->getOrientation().xAxis() * -1.0f;
	mDistance = body.GetDistance(mMovingDirection);

	if(0.0f < mDistance)
	{
		body.SetBackwardMoving(true);
		body.Move(mMovingDirection, mDistance);
		mBodyYaw = body.GetSceneNode()->getOrientation().getYaw();
	}
}


void CommandMoveRightward::Release(Body& body)
{
	//body.Stop();

	Utility::OutputDebug(L"CommandMoveRightward::Release()\n");
}


bool CommandMoveRightward::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	if(1.0f >= std::abs(mDistance))
	{
		Utility::OutputDebug(L" CommandMoveRightward stop...\n");
		return false;
	}
	else if(body.GetSceneNode()->getOrientation().getYaw() != mBodyYaw)
	{
		Initialize(body);
	}

	const Ogre::Vector3 movedPosition = body.GetCurrentSpeed() * frameEvent.timeSinceLastFrame * mMovingDirection;
	
	body.GetSceneNode()->translate(movedPosition);

	mDistance = mDistance - movedPosition.length();

	Utility::OutputDebug(L"Rightward moving. distance: %f moved length: %f, speed: %f\n", mDistance, movedPosition.length(), body.GetCurrentSpeed());
	return true;
}