#include <StdAfx.h>
#include "StencilQueueListener.h"
#include "CommonDefine.h"


void StencilQueueListener::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
{
	if (queueGroupId == RENDER_QUEUE_OUTLINE_GLOW_OBJECTS) // outline glow object 
	{ 
		Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem(); 

		rendersys->clearFrameBuffer(Ogre::FBT_STENCIL); 
		rendersys->setStencilCheckEnabled(true); 
		rendersys->setStencilBufferParams(Ogre::CMPF_ALWAYS_PASS,
			STENCIL_VALUE_FOR_OUTLINE_GLOW, STENCIL_FULL_MASK, 
			Ogre::SOP_KEEP,Ogre::SOP_KEEP,Ogre::SOP_REPLACE,false);       
	} 
	if (queueGroupId == RENDER_QUEUE_OUTLINE_GLOW_GLOWS)  // outline glow
	{ 
		Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem(); 
		rendersys->setStencilCheckEnabled(true); 
		rendersys->setStencilBufferParams(Ogre::CMPF_NOT_EQUAL,
			STENCIL_VALUE_FOR_OUTLINE_GLOW, STENCIL_FULL_MASK, 
			Ogre::SOP_KEEP,Ogre::SOP_KEEP,Ogre::SOP_REPLACE,false);       
	} 
	if (queueGroupId == RENDER_QUEUE_FULL_GLOW_ALPHA_GLOW)  // full glow - alpha glow
	{ 
		Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem(); 
		rendersys->setStencilCheckEnabled(true); 
		rendersys->setStencilBufferParams(Ogre::CMPF_ALWAYS_PASS,
			STENCIL_VALUE_FOR_FULL_GLOW,STENCIL_FULL_MASK, 
			Ogre::SOP_KEEP,Ogre::SOP_KEEP,Ogre::SOP_REPLACE,false);       
	} 

	if (queueGroupId == RENDER_QUEUE_FULL_GLOW_GLOW)  // full glow - glow
	{ 
		Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem(); 
		rendersys->setStencilCheckEnabled(true); 
		rendersys->setStencilBufferParams(Ogre::CMPF_EQUAL,
			STENCIL_VALUE_FOR_FULL_GLOW,STENCIL_FULL_MASK, 
			Ogre::SOP_KEEP,Ogre::SOP_KEEP,Ogre::SOP_ZERO,false);       
	}
}


void StencilQueueListener::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
	if(queueGroupId == LAST_STENCIL_OP_RENDER_QUEUE)
	{ 
		Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem(); 
		rendersys->setStencilCheckEnabled(false); 
		rendersys->setStencilBufferParams(); 
	} 
}