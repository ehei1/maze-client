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
	TODO: ��� ��ũ��Ʈ�� event�� ������. ��ũ��Ʈ�� �̸� �������� �޽����� Soul�� ���� �����ϰ�, �ٵ� �����ϵ��� �Ѵ�
	*/

	if(nullptr != mBody)
	{
		mBody->Receive(*event);
	}
}


void Soul::Run(const Body&, const Ogre::FrameEvent&)
{
	// ���������� ��Ȳ�� ó���Ѵ�... (
}