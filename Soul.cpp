#include "StdAfx.h"
#include "Soul.h"
#include "Event.h"
#include "Body.h"


Soul::Soul(void) :
mIndex(0)
{
}


Soul::~Soul(void)
{
}


void Soul::AddEvent(Event* event)
{
	/*
	TODO: 루아 스크립트에 event를 보낸다. 스크립트는 이를 바탕으로 메시지를 Soul을 통해 생성하고, 바디에 전송하도록 한다
	*/

	if(nullptr != mBody)
	{
		mBody->Receive(*event);
	}
}


void Soul::Run(const Body&, const Ogre::FrameEvent&)
{
	// 정기적으로 상황을 처리한다... (
}