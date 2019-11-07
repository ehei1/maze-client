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
		������ ������ ������ �߻��Ѵٴ� ��.... � ��

		�̵� Ŀ�ǵ尡 1���� ���� ������ �������� �����ؾ��ϰ�...
		�̵� Ŀ�ǵ尡 2���̻� �϶��� �ʿ��� ���۸� ������ ���� �������Ѿ��Ѵ�...��...
		*/
	}
	
	return false;
}