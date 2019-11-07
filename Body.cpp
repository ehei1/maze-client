#include <StdAfx.h>
#include "Body.h"
#include "Utility.h"
#include "Application.h"
#include "DebugManager.h"
#include "Tooltip.h"
#include "CommonDefine.h"
#include "Processor.h"
#include "Soul.h"
#include "ToolTip.h"


Body::Body() :
mEntity(nullptr),
mSceneNode(nullptr),
mCurrentSpeed(0),
mMaxSpeed(0),
mTooltip(new Tooltip),
mIndex(0),
mRaySceneQuery(new Ogre::DefaultRaySceneQuery(&Application::GetSingleton().GetSceneManager())),
mBaseAnimationType(ANIM_IDLE_BASE),
mTopAnimationType(ANIM_IDLE_TOP),
mSoul(nullptr),
mHeight(0),
mIsShadowMode(false)
// mNewtonBody(nullptr)
{}


Body::~Body()
{
	// Utility::SafeDelete(mNewtonBody);

	Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();

	if(nullptr != mEntity)
	{
		sceneManager.destroyEntity(mEntity);
	}

	if(nullptr != mSceneNode &&
		true == sceneManager.hasSceneNode(mSceneNode->getName()))
	{
		sceneManager.destroySceneNode(mSceneNode);
	}

	while(false == mProcessContainer.empty())
	{
		Processor* processor = mProcessContainer.begin()->second;
		mProcessContainer.erase(processor->GetType());

		Utility::SafeDelete(processor);
	}
}


void Body::SetupBody(const Ogre::String& parentNodeName, const char* meshName, Common::QueryType queryType)
{
	Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();

	if(false == sceneManager.hasSceneNode(parentNodeName))
	{
		return;
	}

	Ogre::SceneNode* const parentSceneNode = sceneManager.getSceneNode(parentNodeName);
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	mEntity = sceneManager.createEntity(
		Ogre::String("soul/entity/") + meshName + uniqueIndex,
		meshName);
	mEntity->setCastShadows(true);
	mEntity->setListener(this);
	mEntity->setQueryFlags(queryType);

	mSceneNode = parentSceneNode->createChildSceneNode();
	mSceneNode->attachObject(mEntity);

	SetHeight(mEntity->getBoundingRadius());

	// LUJ, 벽 뒤에 갔을 때 다른 재질을 덮어쓴다. 이를 복구하기 위해 재질을 저장해놓는다
	for(unsigned int i = 0; i < mEntity->getNumSubEntities(); ++i)
	{
		const Ogre::SubEntity* const subEntity = mEntity->getSubEntity(i);

		mSubEntityMaterialContainer.push_back(SubEntityMaterial(i, subEntity->getMaterialName()));
	}
}


void Body::Move(const Ogre::Vector3& direction, Ogre::Real distance)
{
	Utility::OutputDebug(_T("Body::Move()\n"));

	if(ANIM_IDLE_BASE == GetBaseAnimation())
	{
		SetBaseAnimation(ANIM_RUN_BASE, true);

		if(ANIM_IDLE_TOP == GetTopAnimation())
		{
			SetTopAnimation(ANIM_RUN_TOP, true);
		}
	}

	SetCurrentSpeed(mMaxSpeed / (true == IsBackwardMoving() ? 2 : 1));

	Application::GetSingleton().GetDebugManager().DrawLine(
		DebugManager::LinePosition(mSceneNode->getPosition(), mSceneNode->getPosition() + direction * distance),
		Ogre::ColourValue::White);
}


void Body::Move()
{
	if(ANIM_IDLE_BASE == GetBaseAnimation())
	{
		SetBaseAnimation(ANIM_RUN_BASE, true);

		if(ANIM_IDLE_TOP == GetTopAnimation())
		{
			SetTopAnimation(ANIM_RUN_TOP, true);
		}
	}
}


void Body::Stop()
{
	Utility::OutputDebug(_T("Body::Stop()\n"));

	mCurrentSpeed = 0;

	if(ANIM_RUN_BASE == GetBaseAnimation())
	{
		SetBaseAnimation(ANIM_IDLE_BASE, true);

		if(ANIM_RUN_TOP == GetTopAnimation())
		{
			SetTopAnimation(ANIM_IDLE_TOP);
		}
	}
}


void Body::SetBaseAnimation(AnimationType id, bool reset)
{
	Utility::OutputDebug(_T("Body::SetBaseAnimation()\n"));

	if (mBaseAnimationType >= 0 && mBaseAnimationType < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimationType] = false;
		mFadingOut[mBaseAnimationType] = true;
	}

	mBaseAnimationType = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void Body::SetTopAnimation(AnimationType id, bool reset)
{
	if (mTopAnimationType >= 0 && mTopAnimationType < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimationType] = false;
		mFadingOut[mTopAnimationType] = true;
	}

	mTopAnimationType = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}


Tooltip& Body::GetTooltip()
{
	return *mTooltip;
}


void Body::frameRendering(const Ogre::FrameEvent& frameEvent)
{
	if(nullptr != mSoul)
	{
		mSoul->Run(*this, frameEvent);
	}

	if(true == GetTooltip().IsVisible())
	{
		GetTooltip().SetPosition(
			mSceneNode->getPosition() + Ogre::Vector3(0, mHeight * 2, 0));
	}

	for(auto iterator = mRunningProcessContainer.begin();
		mRunningProcessContainer.end() != iterator;)
	{
		Processor* const processor = *iterator;

		if(false == processor->Run(frameEvent, *this))
		{
			mRunningProcessContainer.erase(iterator++);
			continue;
		}

		++iterator;
	}
}


void Body::SetBackwardMoving(bool isBackward)
{
	mIsBackwardMoving = isBackward;

	SetCurrentSpeed(mMaxSpeed / 2);
}


Ogre::Real Body::GetAnimationLength(const Ogre::String& name) const
{
	if(const Ogre::AnimationState* const animationState = mEntity->getAnimationState(name))
	{
		return animationState->getLength();
	}

	return 0;
}


void Body::Receive(const Event& event)
{
	for(auto iterator = mProcessContainer.begin();
		mProcessContainer.end() != iterator;
		++iterator)
	{
		Processor* const processor = iterator->second;

		if(true == processor->Receive(event, *this))
		{
			mRunningProcessContainer.insert(processor);
		}
	}
}


Ogre::Real Body::GetDistance(const Ogre::Vector3& direction) const
{
	const Ogre::Ray ray(mSceneNode->getPosition(), direction);

	mRaySceneQuery->setRay(ray);
	mRaySceneQuery->setSortByDistance(true);
	mRaySceneQuery->setQueryMask(Common::QueryWall);

	const Ogre::RaySceneQueryResult& raySceneQueryResult = mRaySceneQuery->execute();

	for(auto iterator = raySceneQueryResult.begin();
		raySceneQueryResult.end() != iterator;
		++iterator)
	{
		const Ogre::RaySceneQueryResultEntry& raySceneQueryResultEntry = *iterator;
		Ogre::MovableObject* const collidedMovableObject = raySceneQueryResultEntry.movable;

		if(collidedMovableObject == mEntity)
		{
			continue;
		}

		const bool isCollided = collidedMovableObject->getWorldBoundingBox().intersects(mEntity->getWorldBoundingSphere());

		if(true == isCollided)
		{
			return 0;
		}

		auto collidedResult = ray.intersects(collidedMovableObject->getWorldBoundingBox());

		if(true == collidedResult.first)
		{
			printf("collided: %s \n", collidedMovableObject->getName().c_str());

			return collidedResult.second;
		}
	}

	const Ogre::Real defaultDistance = 100.0f;

	return defaultDistance;
}


void Body::AddProcessor(Processor* processor)
{
	assert(mProcessContainer.end() == mProcessContainer.find(processor->GetType()));

	mProcessContainer.insert(ProcessorContainer::value_type(processor->GetType(), processor));
}


Processor* Body::GetProcessor(ProcessType type)
{
	auto iterator = mProcessContainer.find(type);

	return mProcessContainer.end() == iterator ? nullptr : iterator->second;
}


void Body::TurnOnShadow()
{
	if(false == mIsShadowMode)
	{
		mIsShadowMode = true;

		mEntity->setMaterialName("no_depth_check_glow");
	}	
}


void Body::TurnOffShadow()
{
	if(true == mIsShadowMode)
	{
		mIsShadowMode = false;

		for(SubEntityMaterialContainer::const_iterator iterator = mSubEntityMaterialContainer.begin();
			mSubEntityMaterialContainer.end() != iterator;
			++iterator)
		{
			const SubEntityMaterial& subEntityMaterial = *iterator;
			const int index = subEntityMaterial.first;
			const Ogre::String& materialName = subEntityMaterial.second;

			Ogre::SubEntity* const subEntity = mEntity->getSubEntity(index);
			subEntity->setMaterialName(materialName);
		}
	}
}