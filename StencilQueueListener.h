/*
����Ʈ ���μ����� ���� �ܰ����� �����

http://www.ogre3d.org/forums/viewtopic.php?p=194816
*/
#pragma once


#include <OgreRenderQueueListener.h>


class StencilQueueListener : public Ogre::RenderQueueListener
{
public:
	virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation);

	virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation);
};