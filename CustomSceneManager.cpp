#include <StdAfx.h>
#include "CustomSceneManager.h"


/// STATIC OCCLUSION QUERY LIST
typedef std::list< RenderableOcclusionPair* > RenderableOcclusionPairContainer;
RenderableOcclusionPairContainer g_RenderableOcclusionPairList;

Ogre::Renderable* g_InspectRenderable = nullptr;
unsigned int g_PixelCount = 0;


void CustomSceneManagerFactory::initMetaData(void) const
{
	mMetaData.typeName = FACTORY_TYPE_NAME;
	mMetaData.description = "The custom scene manager";
	mMetaData.sceneTypeMask = Ogre::ST_GENERIC;
	mMetaData.worldGeometrySupported = false;
}

const Ogre::String CustomSceneManagerFactory::FACTORY_TYPE_NAME = "CustomSceneManager";

Ogre::SceneManager* CustomSceneManagerFactory::createInstance(const Ogre::String& instanceName)
{
	return new CustomSceneManager(instanceName);
}

void CustomSceneManagerFactory::destroyInstance(Ogre::SceneManager* instance)
{
	delete instance;
}

CustomSceneManager::CustomSceneManager(const Ogre::String& name) :
Ogre::SceneManager(name)
{
}

CustomSceneManager::~CustomSceneManager()
{
}

const Ogre::String& CustomSceneManager::getTypeName(void) const
{
	return CustomSceneManagerFactory::FACTORY_TYPE_NAME;
}

void CustomSceneManager::renderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass, 
	bool lightScissoringClipping, bool doLightIteration, const Ogre::LightList* manualLightList)
{
	RenderableOcclusionPair* currentRenderableOcclusionPair = nullptr;

	for(auto iterator = g_RenderableOcclusionPairList.begin();
		g_RenderableOcclusionPairList.end() != iterator;
		++iterator)
	{
		RenderableOcclusionPair* renderableOcclusionPair = (*iterator);

		if(renderableOcclusionPair->GetRenderable() == rend)
		{
			currentRenderableOcclusionPair = renderableOcclusionPair;

			currentRenderableOcclusionPair->BeginOcclusionQuery();
			break;
		}
	}

	Ogre::SceneManager::renderSingleObject(rend, pass, lightScissoringClipping, doLightIteration, manualLightList);

	if(nullptr != currentRenderableOcclusionPair)
	{
		currentRenderableOcclusionPair->EndOcclusionQuery();
	}
}

RenderableOcclusionPair::RenderableOcclusionPair(Ogre::Renderable* _renderable) :
m_Renderable(_renderable),
m_PixelCount(0),
m_PixelState(RenderableOcclusionPair::READY_FOR_RENDER)
{
	///
	/// CREATE HardwareOcclusionQuery
	///
	m_HardwareOcclusionQuery = Ogre::Root::getSingleton().getRenderSystem()->createHardwareOcclusionQuery();
	assert (m_HardwareOcclusionQuery);
}

RenderableOcclusionPair::~RenderableOcclusionPair()
{
	/// DELETE HardwareOcclusionQuery
	Ogre::Root::getSingleton().getRenderSystem()->destroyHardwareOcclusionQuery(m_HardwareOcclusionQuery);
	m_HardwareOcclusionQuery = nullptr;
}

///
/// Returns the pixel count, and changes the state
/// to continue the next hardware query
///
unsigned int RenderableOcclusionPair::GetPixelCount()
{
	switch (m_PixelState)
	{
	case RenderableOcclusionPair::READY_FOR_ACCESS :
		{
			/// GET THE PIXEL COUNT
			m_HardwareOcclusionQuery->pullOcclusionQuery(&m_PixelCount);

			/// CHANGE THE STATE
			m_PixelState = RenderableOcclusionPair::READY_FOR_RENDER;
		}
	}

	return m_PixelCount;
}

///
/// Returns the renderable to query
///
Ogre::Renderable* RenderableOcclusionPair::GetRenderable()
{
	return m_Renderable;
}

///
/// Start the query, if the state
/// is ready to render.
///
void RenderableOcclusionPair::BeginOcclusionQuery()
{
	switch (m_PixelState)
	{
	case RenderableOcclusionPair::READY_FOR_RENDER :
		{
			/// START THE QUERY
			m_HardwareOcclusionQuery->beginOcclusionQuery();

			/// CHANGE THE STATE
			m_PixelState = RenderableOcclusionPair::QUERY_STARTED;
		}
	}
}

///
/// Finish the query, if the state
/// is ready to render. Changes
/// the state to wait for access
///
void RenderableOcclusionPair::EndOcclusionQuery()
{
	switch (m_PixelState)
	{
	case RenderableOcclusionPair::QUERY_STARTED :
		{
			/// START THE QUERY
			m_HardwareOcclusionQuery->endOcclusionQuery();

			/// CHANGE THE STATE
			m_PixelState = RenderableOcclusionPair::READY_FOR_ACCESS;
		}
	}
}