#include <StdAfx.h>
#include "SoulOrc.h"
#include "SoulMessageMove.h"
#include "SoulMessageDirection.h"
#include "Body.h"
#include "PlayingController.h"


SoulOrc::SoulOrc()
{

}


SoulOrc::~SoulOrc()
{

}


void SoulOrc::frameRendering(Body& body, const Ogre::FrameEvent& frameEvent)
{
	SoulMessage* const soulMessage = Soul::PeekMessage();

	if(nullptr == soulMessage)
	{
		return;
	}
	else if(false == Soul::IsRunning())
	{
		OnStart(body, soulMessage);
	}

	switch(soulMessage->GetType())
	{
	case SoulMessage::TypeMove:
		{
			SoulMessageMove* const soulMessageMove = (SoulMessageMove*)soulMessage;
			
			const Ogre::Vector3& goalPosition = body.GetGoalPosition();

			// move to goal
			if(false == goalPosition.isZeroLength())
			{
				const Ogre::Vector3 distanceVector = body.GetBodyNode()->getPosition() - goalPosition;

				if(distanceVector.length() < 1)
				{
					body.Stop();

					Soul::SetRunning(false);
				}
				else
				{
					const Ogre::Vector3 movingVector = body.GetCurrentSpeed() * frameEvent.timeSinceLastFrame * body.GetDirection();

					body.GetBodyNode()->translate(movingVector);
					body.SetMovingVector(movingVector);
				}
			}

			break;
		}
	case SoulMessage::TypeDirection:
		{
			SoulMessageDirection* const soulMessageDirection = (SoulMessageDirection*)soulMessage;

			// yaw to direction
			body.GetBodyNode()->setDirection(
				soulMessageDirection->GetDirection(),
				Ogre::Node::TS_WORLD,
				Ogre::Vector3::UNIT_Z);

			Soul::SetRunning(false);
			break;
		}
	}

	if(false == Soul::IsRunning())
	{
		OnEnd(body, soulMessage);
	}
}


void SoulOrc::OnStart(Body& body, SoulMessage* soulMessage)
{
	Soul::OnStart(body, soulMessage);

	switch(soulMessage->GetType())
	{
	case SoulMessage::TypeMove:
		{
			SoulMessageMove* const soulMessageMove = (SoulMessageMove*)soulMessage;

			Ogre::Vector3 goalPosition = soulMessageMove->GetPosition() - body.GetBodyNode()->getPosition();
			goalPosition.y = 0;

			const Ogre::Real distanceToGoal = goalPosition.length();
			goalPosition.normalise();

			body.Move(
				goalPosition,
				std::min(distanceToGoal, Soul::GetDistance(body, goalPosition)));
			break;
		} 
	}
}


void SoulOrc::OnEnd(Body& body, SoulMessage* soulMessage)
{
	Soul::OnEnd(body, soulMessage);
}