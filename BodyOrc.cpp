#include <StdAfx.h>
#include "BodyOrc.h"
#include "Application.h"
#include "Utility.h"
#include "CommonDefine.h"
#include "Tooltip.h"
#include "ProcessorMove.h"
#include "ProcessorAction.h"


#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define RUN_SPEED 17.0f           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second


BodyOrc::BodyOrc() :
mVerticalVelocity(0)
{
	SetMaxSpeed(20);

	mAttachedEntityContainer.insert(
		std::make_pair("Handle.L", nullptr));
	mAttachedEntityContainer.insert(
		std::make_pair("Handle.R", nullptr));
	mAttachedEntityContainer.insert(
		std::make_pair("Sheath.L", nullptr));
	mAttachedEntityContainer.insert(
		std::make_pair("Sheath.R", nullptr));

	AddProcessor(new ProcessorMove);
	AddProcessor(new ProcessorAction);
}


void BodyOrc::SetupBody(const Ogre::String& parentNodeName, const char* meshName, Common::QueryType queryType)
{
	Body::SetupBody(parentNodeName, meshName, queryType);

	GetTooltip().AddCaption(
		Ogre::ColourValue::White,
		Ogre::ColourValue(0.3, 0.5, 0.3),
		_T("나는 조선의 오크다"));

	setupAnimations();

	const Ogre::Vector3& position = GetSceneNode()->getPosition();

	GetSceneNode()->setPosition(
		position.x,
		CHAR_HEIGHT,
		position.z);

	// LUJ, 칼을 칼집에 붙이고, 효과도 생성한다
	{
		Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();

		//// create swords and attach to sheath
		mSword1 = sceneManager.createEntity("SinbadSword1", "Sword.mesh");
		mSword1->setQueryFlags(Common::QueryItem);
		mSword2 = sceneManager.createEntity("SinbadSword2", "Sword.mesh");
		mSword2->setQueryFlags(Common::QueryItem);
		
		mEntity->attachObjectToBone("Sheath.L", mSword1);
		mEntity->attachObjectToBone("Sheath.R", mSword2);

		//// create a couple of ribbon trails for the swords, just for fun
		Ogre::NameValuePairList params;
		params["numberOfChains"] = "2";
		params["maxElements"] = "80";
		mSwordTrail = (Ogre::RibbonTrail*)sceneManager.createMovableObject("RibbonTrail", &params);
		mSwordTrail->setQueryFlags(Common::QueryNone);
		//mSwordTrail->setMaterialName("Examples/LightOgre::RibbonTrail");
		mSwordTrail->setTrailLength(20);
		mSwordTrail->setVisible(false);
		mSceneNode->attachObject(mSwordTrail);

		for (int i = 0; i < 2; i++)
		{
			mSwordTrail->setInitialColour(i, 1, 0.8, 0);
			mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
			mSwordTrail->setWidthChange(i, 1);
			mSwordTrail->setInitialWidth(i, 0.5);
		}
	}
}


BodyOrc::~BodyOrc()
{
}


void BodyOrc::setupAnimations()
{
	Ogre::Entity* const bodyEntity = GetEntity();

	// this is very important due to the nature of the exported animations
	bodyEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

	Ogre::String animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		AnimationType animationType = AnimationType(i);

		mAnims[animationType] = bodyEntity->getAnimationState(animNames[i]);
		mAnims[animationType]->setLoop(true);
		mFadingIn[animationType] = false;
		mFadingOut[animationType] = false;
	}

	// start off in the idle state (top and bottom together)
	SetBaseAnimation(ANIM_IDLE_BASE);
	SetTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

	mSwordsDrawn = false;
}


void BodyOrc::updateBody(Ogre::Real deltaTime)
{
	Ogre::SceneNode* const bodySceneNode = GetSceneNode();

	//if (mKeyDirection != Ogre::Vector3::ZERO && GetBaseAnimation() != ANIM_DANCE)
	{
		//// calculate actually goal direction in world based on player's key directions
		//mGoalPosition.z += mKeyDirection.z;
		//mGoalPosition.x += mKeyDirection.x;
		//mGoalPosition.y = 0;
		//mGoalPosition.normalise();

		//Ogre::Quaternion toGoal = bodySceneNode->getOrientation().zAxis().getRotationTo(mGoalPosition);

		//// calculate how much the character has to turn to face goal direction
		//Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();
		//// this is how much the character CAN turn this frame
		//Ogre::Real yawAtSpeed = yawToGoal / Ogre::Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		//// reduce "turnability" if we're in midair
		//if (GetBaseAnimation() == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

		//// turn as much as we can, but not more than we need to
		//if (yawToGoal < 0) yawToGoal = std::min<Ogre::Real>(0, std::max<Ogre::Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Ogre::Real>(yawToGoal, yawAtSpeed, 0);
		//else if (yawToGoal > 0) yawToGoal = std::max<Ogre::Real>(0, std::min<Ogre::Real>(yawToGoal, yawAtSpeed)); //yawToGoal = Math::Clamp<Ogre::Real>(yawToGoal, 0, yawAtSpeed);

		//bodySceneNode->yaw(Ogre::Degree(yawToGoal));

		//Ogre::Vector3 previousPosition = bodySceneNode->getPosition();

		//// move in current body direction (not the goal direction)
		//bodySceneNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[GetBaseAnimation()]->getWeight(),
		//	Ogre::Node::TS_LOCAL);

		//Ogre::Vector3 movedPosition = bodySceneNode->getPosition() - previousPosition;

		//{
		//	Ogre::Camera* const camera = Application::GetSingleton().GetCameraMan()->getCamera();

		//	camera->setPosition(
		//		camera->getPosition() + movedPosition);
		//}
	}

	if (GetBaseAnimation() == ANIM_JUMP_LOOP)
	{
		// if we're jumping, add a vertical offset too, and apply gravity
		bodySceneNode->translate(0, mVerticalVelocity * deltaTime, 0, Ogre::Node::TS_LOCAL);
		mVerticalVelocity -= GRAVITY * deltaTime;

		Ogre::Vector3 pos = bodySceneNode->getPosition();
		if (pos.y <= CHAR_HEIGHT)
		{
			// if we've hit the ground, change to landing state
			pos.y = CHAR_HEIGHT;
			bodySceneNode->setPosition(pos);
			SetBaseAnimation(ANIM_JUMP_END, true);
			mTimer = 0;
		}
	}
}

void BodyOrc::updateAnimations(Ogre::Real deltaTime)
{
	Ogre::Real baseAnimSpeed = 1;
	Ogre::Real topAnimSpeed = 1;

	mTimer += deltaTime;

	if (GetTopAnimation() == ANIM_DRAW_SWORDS)
	{
		// flip the draw swords animation if we need to put it back
		topAnimSpeed = mSwordsDrawn ? -1.0f : 1.0f;

		// half-way through the animation is when the hand grasps the handles...
		if (mTimer >= mAnims[GetTopAnimation()]->getLength() / 2 &&
			mTimer - deltaTime < mAnims[GetTopAnimation()]->getLength() / 2)
		{
			// so transfer the swords from the sheaths to the hands
			mEntity->detachAllObjectsFromBone();
			mEntity->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
			mEntity->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
			// change the hand state to grab or let go
			mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

			// toggle sword trails
			if (mSwordsDrawn)
			{
				mSwordTrail->setVisible(false);
				mSwordTrail->removeNode(mSword1->getParentNode());
				mSwordTrail->removeNode(mSword2->getParentNode());
			}
			else
			{
				mSwordTrail->setVisible(true);
				mSwordTrail->addNode(mSword1->getParentNode());
				mSwordTrail->addNode(mSword2->getParentNode());
			}
		}

		if (mTimer >= mAnims[GetTopAnimation()]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (GetBaseAnimation() == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
			else
			{
				SetTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
			mSwordsDrawn = !mSwordsDrawn;
		}
	}
	else if (GetTopAnimation() == ANIM_SLICE_VERTICAL || GetTopAnimation() == ANIM_SLICE_HORIZONTAL)
	{
		if (mTimer >= mAnims[GetTopAnimation()]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (GetBaseAnimation() == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
			else
			{
				SetTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
		}

		// don't sway hips from side to side when slicing. that's just embarrasing.
		if (GetBaseAnimation() == ANIM_IDLE_BASE) baseAnimSpeed = 0;
	}
	else if (GetBaseAnimation() == ANIM_JUMP_START)
	{
		if (mTimer >= mAnims[GetBaseAnimation()]->getLength())
		{
			// takeoff animation finished, so time to leave the ground!
			SetBaseAnimation(ANIM_JUMP_LOOP, true);
			// apply a jump acceleration to the character
			mVerticalVelocity = JUMP_ACCEL;
		}
	}
	else if (GetBaseAnimation() == ANIM_JUMP_END)
	{
		if (mTimer >= mAnims[GetBaseAnimation()]->getLength())
		{
			// safely landed, so go back to running or idling
			if (mKeyDirection == Ogre::Vector3::ZERO)
			{
				SetBaseAnimation(ANIM_IDLE_BASE);
				SetTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				SetBaseAnimation(ANIM_RUN_BASE, true);
				SetTopAnimation(ANIM_RUN_TOP, true);
			}
		}
	}

	// increment the current base and top animation times
	if (GetBaseAnimation() != ANIM_NONE) mAnims[GetBaseAnimation()]->addTime(deltaTime * baseAnimSpeed);
	if (GetTopAnimation() != ANIM_NONE) mAnims[GetTopAnimation()]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void BodyOrc::fadeAnimations(Ogre::Real deltaTime)
{
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		AnimationType animationType = AnimationType(i);

		if (mFadingIn[animationType])
		{
			// slowly fade this animation in until it has full weight
			Ogre::Real newWeight = mAnims[animationType]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnims[animationType]->setWeight(Ogre::Math::Clamp<Ogre::Real>(newWeight, 0, 1));

			if (newWeight >= 1)
			{
				mFadingIn[animationType] = false;
			}
		}
		else if (mFadingOut[animationType])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Ogre::Real newWeight = mAnims[animationType]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[animationType]->setWeight(Ogre::Math::Clamp<Ogre::Real>(newWeight, 0, 1));

			if (newWeight <= 0)
			{
				mAnims[animationType]->setEnabled(false);
				mFadingOut[animationType] = false;
			}
		}
	}
}


void BodyOrc::frameRendering(const Ogre::FrameEvent& frameEvent)
{
	updateAnimations(frameEvent.timeSinceLastFrame);

	Body::frameRendering(frameEvent);
}


bool BodyOrc::Attach(Body& soul)
{
	for(auto iterator = mAttachedEntityContainer.begin();
		mAttachedEntityContainer.end() != iterator;
		++iterator)
	{
		const Ogre::String& boneName = iterator->first;
		Ogre::Entity*& attachedEntity = iterator->second;

		if(nullptr == attachedEntity)
		{
			attachedEntity = soul.GetEntity()->clone(soul.GetName() + boneName);
			attachedEntity->setQueryFlags(Common::QueryNone);

			GetEntity()->attachObjectToBone(
				boneName,
				attachedEntity);

			return true;
		}
	}

	return false;
}


void BodyOrc::UseLeftHand()
{
	/*
	top animation: 막 시작될 애니메이션
	base animation: 블렌딩될 수 있는 애니메이션. 백그라운드로 진행될 수 있는 애니메이션.

	잘 보면, 달리는 중에도 애니메이션을 표시할 수 있도록 상체는 주로 top, 하체는 주로 base ani를
	쓰고 있다.
	*/
	if(false == mSwordsDrawn)
	{
		if(ANIM_IDLE_TOP == GetTopAnimation() ||
			ANIM_RUN_TOP == GetTopAnimation())
		{
			SetTopAnimation(ANIM_DRAW_SWORDS, true);
			mTimer = 0;
		}

		OutputDebug(_T("drawing sword..\n"));
		return;
	}

	if(ANIM_IDLE_TOP == GetTopAnimation() ||
		ANIM_RUN_TOP == GetTopAnimation())
	{
		const AnimationType animationType[] =
		{
			ANIM_SLICE_HORIZONTAL,
			ANIM_SLICE_VERTICAL,
		};

		OutputDebug(_T("use hand\n"));

		const size_t randomIndex = rand() % _countof(animationType);

		SetTopAnimation(animationType[randomIndex], true);
		mTimer = 0;
	}
}


void BodyOrc::UseRightHand()
{
	// not yet...
}