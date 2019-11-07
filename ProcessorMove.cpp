#include "StdAfx.h"
#include "ProcessorMove.h"
#include "CommandMoveForward.h"
#include "CommandMoveLeftward.h"
#include "CommandMoveRightward.h"
#include "CommandMoveBackward.h"
#include "CommandMoveStop.h"
#include "Event.h"
#include "EventCommon.h"
#include "Utility.h"
#include "ProcessorCommon.h"
#include "Body.h"


ProcessorMove::ProcessorMove() :
Processor(ProcessMove)
{

}


ProcessorMove::~ProcessorMove()
{

}


bool ProcessorMove::Receive(const Event& event, Body& body)
{
	if(true == Update(event, body))
	{
		return true;
	}

	Command* runningCommand = nullptr;

	// �Լ� ������ ���ε��ؼ� ����� ��... 
	switch(event.GetType())
	{
	case EventMoveForward:
		{
			runningCommand = new CommandMoveForward(event);
			runningCommand->Initialize(body);

			Add(runningCommand);			
			break;
		}
	case EventMoveLeftward:
		{
			runningCommand = new CommandMoveLeftward(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}
	case EventMoveRightward:
		{
			runningCommand = new CommandMoveRightward(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}
	case EventMoveBackward:
		{
			runningCommand = new CommandMoveBackward(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}
	case EventMoveStop:
		{
			runningCommand = new CommandMoveStop(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}		
	}

	return nullptr != runningCommand;
}


bool ProcessorMove::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	const bool result = Processor::Run(frameEvent, body);

	// LUJ, �� �̻� ������ �̵� ����� ���ٴ� ��
	if(false == result)
	{
		body.Stop();
	}

	return result;
}