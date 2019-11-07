#include <StdAfx.h>
#include "SoulController.h"
#include "Body.h"
#include "Utility.h"
#include "Application.h"
#include "DebugManager.h"
#include "CommonDefine.h"
#include "EventCommon.h"
#include "Cursor.h"
#include "Event.h"


SoulController::SoulController(Ogre::SceneManager& sceneManager, Ogre::SceneNode& sceneNode) :
mBody(nullptr),
mCamera(nullptr),
// mPlayerController(nullptr),
mCursor(new Cursor(sceneManager, sceneNode, Ogre::Vector2(2, 3.5))),
mRotatingDirection(0)
{
}


SoulController::~SoulController()
{
	// Utility::SafeDelete(mPlayerController);
}


void SoulController::keyPressed(const OIS::KeyEvent& evt)
{
	if(nullptr == mBody)
	{
		return;
	}

	mCursor->keyPressed(evt);

	switch(evt.key)
	{
	case OIS::KC_W:
		{
			Event event(EventMoveForward);
			mBody->Receive(event);
			break;
		}		
	case OIS::KC_S:
		{
			Event event(EventMoveBackward);
			mBody->Receive(event);
			break;
		}
	case OIS::KC_A:
		{
			Event event(EventMoveLeftward);
			mBody->Receive(event);
			break;
		}
	case OIS::KC_D:
		{
			Event event(EventMoveRightward);
			mBody->Receive(event);
			break;
		}
	case OIS::KC_Z:
		{
			mCameraYaw = Ogre::Degree(100);
			break;
		}
	case OIS::KC_X:
		{
			mCameraYaw = Ogre::Degree(-100);
			break;
		}
	}

	//if (evt.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	//{
	//	// take swords out (or put them back, since it's the same animation but reversed)
	//	SetTopAnimation(ANIM_DRAW_SWORDS, true);
	//	mTimer = 0;
	//}
	//else if (evt.key == OIS::KC_E && !mSwordsDrawn)
	//{
	//	if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
	//	{
	//		// start dancing
	//		SetBaseAnimation(ANIM_DANCE, true);
	//		SetTopAnimation(ANIM_NONE);
	//		// disable hand animation because the dance controls hands
	//		mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
	//	}
	//	else if (mBaseAnimID == ANIM_DANCE)
	//	{
	//		// stop dancing
	//		SetBaseAnimation(ANIM_IDLE_BASE);
	//		SetTopAnimation(ANIM_IDLE_TOP);
	//		// re-enable hand animation
	//		mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
	//	}
	//}

	////// keep track of the player's intended direction
	////else if (evt.key == OIS::KC_W) mKeyDirection.z = -1;
	////else if (evt.key == OIS::KC_A) mKeyDirection.x = -1;
	////else if (evt.key == OIS::KC_S) mKeyDirection.z = 1;
	////else if (evt.key == OIS::KC_D) mKeyDirection.x = 1;

	//else if (evt.key == OIS::KC_W)
	//{
	//	mKeyDirection = mSceneNode->getOrientation().zAxis();
	//}
	//else if (evt.key == OIS::KC_A)
	//{
	//	mKeyDirection = mSceneNode->getOrientation().xAxis();
	//}
	//else if (evt.key == OIS::KC_S)
	//{
	//	mKeyDirection = mSceneNode->getOrientation().zAxis() * -1;
	//}
	//else if (evt.key == OIS::KC_D)
	//{
	//	mKeyDirection = mSceneNode->getOrientation().xAxis() * -1;
	//}

	//else if (evt.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	//{
	//	// jump if on ground
	//	SetBaseAnimation(ANIM_JUMP_START, true);
	//	SetTopAnimation(ANIM_NONE);
	//	mTimer = 0;
	//}

	//if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	//{
	//	// start running if not already moving and the player wants to move
	//	SetBaseAnimation(ANIM_RUN_BASE, true);
	//	if (mTopAnimID == ANIM_IDLE_TOP) SetTopAnimation(ANIM_RUN_TOP, true);
	//}
}

void SoulController::keyReleased(const OIS::KeyEvent& evt)
{
	if(nullptr == mBody)
	{
		return;
	}

	mCursor->keyReleased(evt);

	switch(evt.key)
	{
	case OIS::KC_W:
		{
			Event event(EventMoveStop);
			event.SetValue("eventType", EventMoveForward);

			mBody->Receive(event);
			break;
		}
	case OIS::KC_S:
		{
			Event event(EventMoveStop);
			event.SetValue("eventType", EventMoveBackward);

			mBody->Receive(event);
			break;
		}
	case OIS::KC_A:
		{
			Event event(EventMoveStop);
			event.SetValue("eventType", EventMoveLeftward);

			mBody->Receive(event);
			break;
		}
	case OIS::KC_D:
		{
			Event event(EventMoveStop);
			event.SetValue("eventType", EventMoveRightward);

			mBody->Receive(event);
			break;
		}
	case OIS::KC_Z:
	case OIS::KC_X:
		{
			mCameraYaw = Ogre::Radian();
			break;
		}
	}
}

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
void SoulController::mouseMoved(const OIS::MultiTouchEvent& evt)
{
	// update camera goal based on mouse movement
	updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel, -0.0005f * evt.state.Z.rel);
}

void SoulController::mousePressed(const OIS::MultiTouchEvent& evt)
{
	if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// if swords are out, and character's not doing something weird, then SLICE!
		SetTopAnimation(ANIM_SLICE_VERTICAL, true);
		mTimer = 0;
	}
}
#else
void SoulController::mouseMoved(const OIS::MouseEvent& mouseEvent)
{
	mCursor->mouseMoved(mouseEvent, mBody->GetSceneNode()->getPosition());

	Ogre::Vector3 lookingDirection = mCursor->GetMouseCursorPosition() - mBody->GetSceneNode()->getPosition();
	lookingDirection.y = 0;
	lookingDirection.normalise();

	// LUJ, 커서가 바라보는 방면으로 몸체의 방향을 옮긴다
	{
<<<<<<< .mine
		mBody->GetNewtonBody()->setOmega(Ogre::Vector3(0.5, 100, 0));
=======
		Event event(EventChangeDirection);
		event.SetValue("direction", lookingDirection);
>>>>>>> .r59

<<<<<<< .mine
		//Ogre::Quaternion quaternion;
		//quaternion.FromAxes(&lookingDirection);

		//Event event(EventChangeDirection);
		//event.SetValue("direction", lookingDirection);

		//mBody->Receive(event);

		//mBody->GetNewtonBody()->attachNode(mBody->GetSceneNode());


		//mBody->GetNewtonBody()->setPositionOrientation(
		//	mBody->GetSceneNode()->getPosition(),
		//	quaternion);
=======
		mBody->Receive(event);
>>>>>>> .r59
	}

	// LUJ, 카메라가 항상 시선 쪽을 향하도록 한다. 이동 중에도 작동하면 몹시 산만하므로, 정지 중에만 기능시킨다
	if(false == mBody->IsStop())
	{
		mRotatingDirection = 0;
	}
	// LUJ, 이동 중인 경우 카메라를 즉시 전방으로 변경한다
	else
	{
		Utility::OutputDebug(L"direction changing...\n");

		Ogre::Vector3 cameraVector = mBody->GetSceneNode()->getPosition() - mCamera->getPosition();
		cameraVector.y = 0;
		cameraVector.normalise();

		// http://blog.naver.com/clampkor/30026793407
		Ogre::Radian radian = Ogre::Math::ATan2(
			lookingDirection.x * cameraVector.z - cameraVector.x * lookingDirection.z,
			cameraVector.x * lookingDirection.x + cameraVector.z * lookingDirection.z);

		if(0 != radian.valueDegrees())
		{
			mRotatingDirection = (0 > radian.valueDegrees() ? 1 : -1);
		}
	}
}

void SoulController::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	switch(id)
	{
	case OIS::MB_Left:
		{
			Event event(EventUseHand);
			event.SetValue("hand", HandLeft);

			mBody->Receive(event);
			break;
		}
	case OIS::MB_Right:
		{
			Event event(EventUseHand);
			event.SetValue("hand", HandRight);

			mBody->Receive(event);
			break;
		}
	}
}


void SoulController::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{

}


#endif


bool SoulController::frameRendering(const Ogre::FrameEvent& frameEvent)
{
	if(nullptr == mBody)
	{
		return false;
	}

	if(false == mBody->IsStop())
	{
		mCamera->setPosition(mBody->GetSceneNode()->getPosition() - mCamera->getAutoTrackOffset());
	}

	if(Ogre::Radian() != mCameraYaw)
	{
		Ogre::Vector3 cameraPosition = mCamera->getPosition();

		Utility::RotateXZ(
			cameraPosition.x,
			cameraPosition.z,
			mCameraYaw * frameEvent.timeSinceLastFrame,
			mBody->GetSceneNode()->getPosition(),
			cameraPosition);

		mCamera->setAutoTracking(
			true,
			mBody->GetSceneNode(),
			mBody->GetSceneNode()->getPosition() - cameraPosition);
		mCamera->setPosition(cameraPosition);
	}

	if(0 != mRotatingDirection)
	{
		Ogre::Vector3 cameraVector = mCamera->getPosition() - mBody->GetSceneNode()->getPosition();
		cameraVector.y = 0;
		cameraVector.normalise();

		Ogre::Vector3 soulDirection = Utility::GetDirection(*(mBody->GetSceneNode()));
		Ogre::Radian radian = Ogre::Degree(180) - Ogre::Math::ACos(soulDirection.x * cameraVector.x + soulDirection.z * cameraVector.z);

		if(1 > abs(radian.valueDegrees()))
		{
			mRotatingDirection = 0;
		}
		else
		{
			radian = std::max(Ogre::Radian(radian.valueRadians() / 1.5f), Ogre::Radian(Ogre::Degree(10)));

			Ogre::Vector3 cameraPosition = mCamera->getPosition();

			Utility::RotateXZ(
				cameraPosition.x,
				cameraPosition.z,
				radian * frameEvent.timeSinceLastFrame * mRotatingDirection,
				mBody->GetSceneNode()->getPosition(),
				cameraPosition);

			mCamera->setAutoTracking(
				true,
				mBody->GetSceneNode(),
				mBody->GetSceneNode()->getPosition() - cameraPosition);
			mCamera->setPosition(cameraPosition);
		}
	}

	mCursor->frameRendering(frameEvent);

	return true;
}


const Cursor& SoulController::GetCursor()
{
	return *mCursor;
}


void SoulController::SetBody(Body* body)
{
	mBody = body;

//	OgreNewt::World* const newtonWorld = Application::GetSingleton().GetNewtonWorld();
//
//	OgreNewt::ConvexCollisionPtr col = OgreNewt::ConvexCollisionPtr(
//		new OgreNewt::CollisionPrimitives::Ellipsoid(newtonWorld, body->GetSceneNode()->getScale() * 0.5f, 0 ));
//	OgreNewt::Body* bod = new OgreNewt::Body(newtonWorld, col);
//	Ogre::Vector3 inertia, offset;
//	col->calculateInertialMatrix(inertia, offset);
//#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
//delete col;
//#endif
//	bod->attachNode(body->GetSceneNode());
//	bod->setMassMatrix(50, 50 * inertia);
//	bod->setCenterOfMass(offset);
//	bod->setStandardForceCallback();
//
//	bod->setPositionOrientation(
//		body->GetSceneNode()->getPosition(),
//		body->GetSceneNode()->getOrientation());
//	//bod->setPositionOrientation(Ogre::Vector3(0.0f, 1.0f, 0.0f), Ogre::Quaternion::IDENTITY);
//	// set teh play to alway be actibe
//	bod->setAutoSleep(0);
//
//	body->SetNewtonBody(bod);
//
//	Utility::SafeDelete(mPlayerController);
//
//	mPlayerController = new OgreNewt::PlayerController(bod, 0.4f);
}