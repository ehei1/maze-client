#include "StdAfx.h"
#include "CommandMoveForward.h"
#include "Event.h"
#include "Body.h"
#include "Utility.h"


CommandMoveForward::CommandMoveForward(const Event& event) :
Command(event),
mDistance(0)
{
	SetValue(event);
}


CommandMoveForward::~CommandMoveForward(void)
{

}


void CommandMoveForward::SetValue(const Event&)
{}


void CommandMoveForward::Initialize(Body& body)
{
	mMovingDirection = Utility::GetDirection(*(body.GetSceneNode()));
	mDistance = body.GetDistance(mMovingDirection);

	if(0 <= mDistance)
	{
		body.SetBackwardMoving(false);
		body.Move(mMovingDirection, mDistance);

		mBodyYaw = body.GetSceneNode()->getOrientation().getYaw();
	}
}


void CommandMoveForward::Release(Body& body)
{
	//body.Stop();

	Utility::OutputDebug(L"CommandMoveForward::Release()\n");
}


bool CommandMoveForward::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	if(1.0f >= std::abs(mDistance))
	{
		Utility::OutputDebug(L"stop...\n");
		return false;
	}
	else if(body.GetSceneNode()->getOrientation().getYaw() != mBodyYaw)
	{
		Initialize(body);
	}

	const Ogre::Vector3 movedPosition = body.GetCurrentSpeed() * frameEvent.timeSinceLastFrame * mMovingDirection;
	
	body.GetSceneNode()->translate(movedPosition);

	mDistance = mDistance - movedPosition.length();

	Utility::OutputDebug(L"forward moving. distance: %f moved length: %f, speed: %f\n", mDistance, movedPosition.length(), body.GetCurrentSpeed());
	return true;
}