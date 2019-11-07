#pragma once


#define NUM_ANIMS 13


class Tooltip;
class Soul;
class SoulMessage;
class Event;
class Processor;

namespace Common { enum QueryType; }
enum ProcessType;


class Body :
	public Ogre::MovableObject::Listener
{
	friend class BodyFactory;


protected:
	enum AnimationType
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};


public:
	virtual ~Body();

	virtual void SetupBody(const Ogre::String& parentNodeName, const char* meshName, Common::QueryType);

	virtual void Move(const Ogre::Vector3& direction, Ogre::Real distance);
	virtual void Move();

	inline virtual bool IsStop() const { return 0 == mCurrentSpeed; }

	virtual void Stop();
	inline bool IsBackwardMoving() const { return mIsBackwardMoving; }
	void SetBackwardMoving(bool isBackward);

	Tooltip& GetTooltip();

	inline Ogre::SceneNode* GetSceneNode() { return mSceneNode; }
	inline const Ogre::SceneNode* GetSceneNode() const { return mSceneNode; }

	inline Ogre::Entity* GetEntity() { return mEntity; }
	inline const Ogre::Entity* GetEntity() const { return mEntity; }

	inline const Ogre::String& GetName() const { return mName; }
	inline void SetName(const Ogre::String& name) { mName = name; }

	inline Ogre::Real GetCurrentSpeed() const { return mCurrentSpeed; }
	inline void SetCurrentSpeed(Ogre::Real speed) { mCurrentSpeed = speed; }
	inline void SetMaxSpeed(Ogre::Real speed) { mMaxSpeed = speed; }
	inline Ogre::Real GetMaxSpeed() const { return mMaxSpeed; }

	void SetSoul(Soul* soul) { mSoul = soul; }
	Soul* GetSoul() { return mSoul; }

	Ogre::Real GetDistance(const Ogre::Vector3& direction) const;
	Ogre::Real GetAnimationLength(const Ogre::String&) const;

	inline int GetIndex() const { return mIndex; }

	void Receive(const Event&);
	Processor* GetProcessor(ProcessType);

	virtual void UseLeftHand() {}
	virtual void UseRightHand() {}

	// LUJ, 장애물로 인해 시야에서 보이고/안 보일 때 호출된다.
	void TurnOnShadow();
	void TurnOffShadow();

	// inline void SetNewtonBody(OgreNewt::Body* body) { mNewtonBody = body; }
	// inline OgreNewt::Body* GetNewtonBody() { return mNewtonBody; }


protected:
	Body();

	void SetBaseAnimation(AnimationType id, bool reset = false);
	inline AnimationType GetBaseAnimation() const { return mBaseAnimationType; }

	void SetTopAnimation(AnimationType id, bool reset = false);
	inline AnimationType GetTopAnimation() const { return mTopAnimationType; }

	virtual void frameRendering(const Ogre::FrameEvent&);

	inline void SetHeight(Ogre::Real height) { mHeight = height; }

	void AddProcessor(Processor*);


protected:
	std::map< AnimationType, Ogre::AnimationState* > mAnims;
	std::map< AnimationType, bool > mFadingIn;
	std::map< AnimationType, bool > mFadingOut;

	Ogre::SceneNode* mSceneNode;
	Ogre::Entity* mEntity;
	
	bool mIsShadowMode;

	typedef std::pair< int, Ogre::String > SubEntityMaterial;
	typedef std::list < SubEntityMaterial > SubEntityMaterialContainer;
	SubEntityMaterialContainer mSubEntityMaterialContainer;


private:
	Ogre::String mName;
	int mIndex;

	std::auto_ptr< Tooltip > mTooltip;

	// current base (full- or lower-body) animation
	AnimationType mBaseAnimationType;
	// current top (upper-body) animation
	AnimationType mTopAnimationType;
	
	Ogre::Real mCurrentSpeed;
	Ogre::Real mMaxSpeed;
	Ogre::Real mHeight;

	bool mIsBackwardMoving;

	Soul* mSoul;

	std::auto_ptr< Ogre::RaySceneQuery > mRaySceneQuery;

	// LUJ, 이벤트 수신해야하는 프로세서실
	typedef std::map< ProcessType, Processor* > ProcessorContainer;
	ProcessorContainer mProcessContainer;

	// LUJ, 실행 중인 프로세서. Run()을 호출해야하는 프로세서
	typedef std::set< Processor* > RunningProcessorContainer;
	RunningProcessorContainer mRunningProcessContainer;

	// OgreNewt::Body* mNewtonBody;
};