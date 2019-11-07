#include "StdAfx.h"
#include "Processor.h"
#include "Utility.h"
#include "Command.h"
#include "Event.h"


Processor::Processor(ProcessType type) :
mType(type)
{

}


Processor::~Processor()
{
	while(false == mRunningCommandContainer.empty())
	{
		Command* command = *(mRunningCommandContainer.begin());

		Utility::SafeDelete(command);

		mRunningCommandContainer.pop_front();
	}
}


bool Processor::Run(const Ogre::FrameEvent& frameEvent, Body& body)
{
	for(CommandContainer::iterator iterator = mRunningCommandContainer.begin();
		mRunningCommandContainer.end() != iterator;)
	{
		Command* command = *iterator;

		if(true == command->IsStop())
		{
			mRunningCommandContainer.erase(iterator++);

			Utility::SafeDelete(command);
			continue;
		}
		else if(false == command->Run(frameEvent, body))
		{
			mRunningCommandContainer.erase(iterator++);

			command->Release(body);
			Utility::SafeDelete(command);
			continue;
		}

		++iterator;
	}

	return false == mRunningCommandContainer.empty();
}


bool Processor::Update(const Event& event, Body& body)
{
	for(CommandContainer::const_iterator iterator = mRunningCommandContainer.begin();
		mRunningCommandContainer.end() != iterator;
		++iterator)
	{
		Command* const command = *iterator;

		if(event.GetType() == command->GetType())
		{
			command->SetValue(event);
			command->Initialize(body);
			return true;
		}
	}

	return false;
}


void Processor::Add(Command* command)
{
	mRunningCommandContainer.push_back(command);
}


void Processor::Remove(EventType eventType, Body& body)
{
	// 컨테이너 반복 중에 또 지워! 미쳤나... -_-; 커맨드를 스톱시켜라
	for(CommandContainer::const_iterator iterator = mRunningCommandContainer.begin();
		mRunningCommandContainer.end() != iterator;
		++iterator)
	{
		Command* command = *iterator;

		if(command->GetType() == eventType)
		{
			command->Stop();
		}
	}
}