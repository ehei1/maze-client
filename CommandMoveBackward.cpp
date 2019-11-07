#include "StdAfx.h"
#include "CommandMoveBackward.h"
#include "Event.h"
#include "Body.h"
#include "Utility.h"


CommandMoveBackward::CommandMoveBackward(const Event& event) :
Command(event),
mDistance(0)
{
	SetValue(event);
}


CommandMoveBackward::~CommandMoveBackward(void)
{

}


void CommandMoveBackward::SetValue(const Event& event)
{}


void CommandMoveBackward::Initialize(Body& body)
{
	mMovingDirection = Utility::GetDirection(*(body.GetSceneNode())) * -1.0f;
	//mDistance = std::min(std::numeric_limits< Ogre::Real >::max(), body.GetDistance(mMovingDirection));
	mDistance = body.GetDistance(mMovingDirection);

	if(0 <= mDistance)
	{
		body.SetBackwardMoving(true);
		body.Move(mMovingDirection, mDistance);

		mBodyYaw = body.GetSceneNode()->getOrientation().getYaw();
	}
}


void CommandMoveBackward::Release(Body& body)
{
	//body.Stop();

	Utility::OutputDebug(L"CommandMoveBackward::Release()\n");
}


bool CommandMoveBackward::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	if(1.0f >= std::abs(mDistance))
	{
		return false;
	}
	else if(mBodyYaw != body.GetSceneNode()->getOrientation().getYaw())
	{
		Initialize(body);
	}

	const Ogre::Vector3 movedPosition = body.GetCurrentSpeed() * frameEvent.timeSinceLastFrame * mMovingDirection;
	
	body.GetSceneNode()->translate(movedPosition);

	mDistance = mDistance - movedPosition.length();

	Utility::OutputDebug(L"backward moving. distance: %f moved length: %f, speed: %f\n", mDistance, movedPosition.length(), body.GetCurrentSpeed());
	return true;
}