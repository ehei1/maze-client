#include <StdAfx.h>
#include "BodyFactory.h"
#include "BodyOrc.h"
#include "BodySword.h"
#include "BodyDoor.h"
//#include "BodyBox.h"
#include "Utility.h"
#include "Application.h"
#include "Soul.h"
#include "CommonDefine.h"


BodyFactory::BodyFactory()
{
	
}


BodyFactory::~BodyFactory()
{

}


void BodyFactory::Release()
{
	std::for_each(
		mBodyContainer.begin(),
		mBodyContainer.end(),
		[](BodyContainer::value_type& value) { Utility::SafeDelete(value.second); } );
	mBodyContainer.clear();
	mIndexedBodyContainer.clear();
}


Body* BodyFactory::AddBody(Type type, const Ogre::String& parentNodeName, const Ogre::String& bodyName)
{
	Body* body = nullptr;

	switch(type)
	{
	case TypeOrc:
		{
			body = new BodyOrc;
			body->SetupBody(parentNodeName, "sinbad.mesh", Common::QueryCreature);
			body->SetSoul(new Soul);
			break;
		}
	case TypeSword:
		{
			body = new BodySword;
			body->SetupBody(parentNodeName, "sword.mesh", Common::QueryItem);
			break;
		}
	case TypeDoor:
		{
			body = new BodyDoor;
			break;
		}
	case TypeBox:
		{
			break;
			//body = new BodyBox;
			//body->SetupBody(parentNodeName, "box.mesh", Common::QueryBox);
			break;
		}
	default:
		{
			assert(0 && "undefined type");
			return nullptr;
		}
	}

	body->SetName(bodyName);

	mBodyContainer.insert(
		BodyContainer::value_type(bodyName, body));
	mIndexedBodyContainer.insert(
		IndexedBodyContainer::value_type(body->GetIndex(), body));

	return body;
}


void BodyFactory::RemoveBody(const Ogre::String& name)
{
	auto iterator = mBodyContainer.find(name);

	if(mBodyContainer.end() == iterator)
	{
		return;
	}

	Body* body = iterator->second;
	mBodyContainer.erase(iterator);

	Utility::SafeDelete(body);
}


void BodyFactory::frameRendering(const Ogre::FrameEvent& frameEvent)
{
	std::for_each(
		mBodyContainer.begin(),
		mBodyContainer.end(),
		[&frameEvent](BodyContainer::value_type& value) { value.second->frameRendering(frameEvent); } );
}


Body* BodyFactory::GetBody(const Ogre::String& name)
{
	auto iterator = mBodyContainer.find(name);

	return mBodyContainer.end() == iterator ? nullptr : iterator->second;
}


Body* BodyFactory::GetBody(int bodyIndex)
{
	auto iterator = mIndexedBodyContainer.find(bodyIndex);

	return mIndexedBodyContainer.end() == iterator ? nullptr : iterator->second;
}