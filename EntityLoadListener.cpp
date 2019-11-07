#include <StdAfx.h>
#include "EntityLoadListener.h"
#include "Application.h"
#include "Utility.h"

EntityLoadListener::EntityLoadListener(const Parameter& parameter) :
mParameter(parameter),
mResourceGroupName("Mesh")
{
	mIsDestroy = false;
	mIsReady = true;
}

EntityLoadListener::~EntityLoadListener()
{

}

Ogre::BackgroundProcessTicket EntityLoadListener::Load()
{
	if(false == mIsReady)
	{
		return 0;
	}

	mIsReady = false;

	return Ogre::ResourceBackgroundQueue::getSingleton().load(
		Ogre::MeshManager::getSingleton().getResourceType(),
		mParameter.mMeshName,
		mResourceGroupName,
		false,
		nullptr,
		nullptr,
		this);
}

void EntityLoadListener::operationFailed(Ogre::BackgroundProcessTicket, const Ogre::BackgroundProcessResult&)
{
	Utility::AutoFlagger< tbb::atomic< bool >, bool > autoFlagger(
		mIsDestroy);
	autoFlagger.Reserve(true);
}

void EntityLoadListener::operationCompleted(Ogre::BackgroundProcessTicket ticket, const Ogre::BackgroundProcessResult& result)
{
	Utility::AutoFlagger< tbb::atomic< bool >, bool > autoFlagger(
		mIsDestroy);
	autoFlagger.Reserve(true);

	Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();
	Ogre::SceneNode* const parentSceneNode = sceneManager.getSceneNode(mParameter.mParentNodeName);

	if(nullptr == parentSceneNode)
	{
		return;
	}

	Ogre::Entity* entity = sceneManager.createEntity(
		mParameter.mEntityName,
		mParameter.mMeshName,
		mResourceGroupName);
	entity->setCastShadows(true);
	entity->setListener(mParameter.mListener);

	// 텍스처도 동적으로 읽어들인다. 다만, 텍스처를 백그라운드로 읽어들이는 건 고민이 필요하다
	for(unsigned int i = 0; i < entity->getNumSubEntities(); ++i)
	{
		Ogre::SubEntity* const subEntity = entity->getSubEntity(i);
		Ogre::SubMesh* const subMesh = subEntity->getSubMesh();
		const Ogre::ResourcePtr resourcePointer = Ogre::MaterialManager::getSingleton().getByName(
			subMesh->getMaterialName(),
			mResourceGroupName);

		if(nullptr == resourcePointer.get())
		{
			LoadMaterial(
				mParameter.mMaterialName);

			Ogre::MaterialPtr materialPointer = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(
				subMesh->getMaterialName(),
				mResourceGroupName);
			subEntity->setMaterial(materialPointer);
		}
	}

	Ogre::SceneNode* sceneNode = parentSceneNode->createChildSceneNode(
		mParameter.mNodeName);
	sceneNode->attachObject(entity);
	sceneNode->setPosition(mParameter.mPosition);
	sceneNode->setScale(mParameter.mScale);
}

void EntityLoadListener::LoadMaterial(const Ogre::String& fileName) const
{
	// Reloading materials and parsing material scripts  
	// http://www.ogre3d.org/tikiwiki/Reloading+materials+and+parsing+material+scripts&structure=Cookbook

	// TODO: check it opened already
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(
		fileName,
		mResourceGroupName);

	if(false == stream.isNull())
	{
		try
		{
			Ogre::MaterialManager::getSingleton().parseScript(stream, mResourceGroupName);
		}
		catch (Ogre::Exception& e)
		{
			Ogre::StringUtil::StrStreamType msg;
			msg << "Exception: FILE: " << __FILE__ << " LINE: " << __LINE__ << " DESC: " << e.getFullDescription() << std::endl;
			Ogre::LogManager::getSingleton().logMessage(msg.str());
		}
	}

	stream->close();
}