#include "StdAfx.h"
#include "ProcessorAction.h"
#include "CommandChangeDirection.h"
#include "CommandUseHand.h"
#include "Event.h"
#include "EventCommon.h"
#include "Utility.h"
#include "ProcessorCommon.h"


ProcessorAction::ProcessorAction() :
Processor(ProcessAction)
{

}


ProcessorAction::~ProcessorAction()
{

}


bool ProcessorAction::Receive(const Event& event, Body& body)
{
	if(true == Update(event, body))
	{
		return true;
	}

	Command* runningCommand = nullptr;

	// �Լ� ������ ���ε��ؼ� ����� ��... 
	switch(event.GetType())
	{
	case EventChangeDirection:
		{
			runningCommand = new CommandChangeDirection(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}
	case EventUseHand:
		{
			runningCommand = new CommandUseHand(event);
			runningCommand->Initialize(body);

			Add(runningCommand);
			break;
		}
	}

	return nullptr != runningCommand;
}