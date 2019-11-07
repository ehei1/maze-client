#include "StdAfx.h"
#include "CommandMoveStop.h"
#include "Event.h"
#include "Body.h"
#include "Utility.h"
#include "Processor.h"
#include "ProcessorCommon.h"
#include "Command.h"
#include "EventCommon.h"


CommandMoveStop::CommandMoveStop(const Event& event) :
Command(event)
{
	SetValue(event);
}


CommandMoveStop::~CommandMoveStop(void)
{

}


void CommandMoveStop::SetValue(const Event& event)
{
	event.GetValue(mStoppingEventType, "eventType");
}


void CommandMoveStop::Initialize(Body& body)
{
	
}


void CommandMoveStop::Release(Body& body)
{
	Utility::OutputDebug(L"CommandMoveStop::Release()\n");
}


bool CommandMoveStop::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	if(Processor* const processor = body.GetProcessor(ProcessMove))
	{
		processor->Remove(mStoppingEventType, body);

		/*
		문제는 절묘한 오류가 발생한다는 점.... 어떤 점

		이동 커맨드가 1개일 때는 릴리즈 과정으로 중지해야하고...
		이동 커맨드가 2개이상 일때는 필요한 동작만 릴리즈 없이 중지시켜야한다...흠...
		*/
	}
	
	return false;
}