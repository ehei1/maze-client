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

	// 함수 포인터 바인딩해서 사용할 것... 
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

	// LUJ, 더 이상 실행할 이동 명령이 없다는 뜻
	if(false == result)
	{
		body.Stop();
	}

	return result;
}