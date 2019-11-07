/*
포스트 프로세싱을 통해 외곽선을 만든다

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