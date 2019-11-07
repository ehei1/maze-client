#pragma once

class EntityLoadListener : public Ogre::ResourceBackgroundQueue::Listener
{
public:
	struct Parameter
	{
		Ogre::String mMeshName;
		Ogre::String mMaterialName;
		Ogre::String mEntityName;
		Ogre::String mNodeName;
		Ogre::String mParentNodeName;

		Ogre::Vector3 mPosition;
		Ogre::Vector3 mScale;
		Ogre::MovableObject::Listener* mListener;

		Parameter() :
		mListener(nullptr)
		{}
	};
	EntityLoadListener(const Parameter&);
	virtual ~EntityLoadListener();
	virtual void operationCompleted(Ogre::BackgroundProcessTicket, const Ogre::BackgroundProcessResult&);
	virtual void operationFailed(Ogre::BackgroundProcessTicket, const Ogre::BackgroundProcessResult&);
	bool IsDestroy() const { return mIsDestroy; }
	Ogre::BackgroundProcessTicket Load();

private:
	void LoadMaterial(const Ogre::String& fileName) const;

private:
	const Ogre::String mResourceGroupName;
	const Parameter mParameter;
	tbb::atomic< bool > mIsDestroy;
	tbb::atomic< bool > mIsReady;
};