#pragma once


#include "Soul.h"


class Body;
class Cursor;


class SoulController : public Soul
{
public:	
	SoulController(Ogre::SceneManager&, Ogre::SceneNode&);
	virtual ~SoulController();

	void SetBody(Body*);
	inline Body* GetBody() { return mBody; }

	inline void SetCamera(Ogre::Camera* camera) { mCamera = camera; }

	bool frameRendering(const Ogre::FrameEvent&);
	void keyPressed(const OIS::KeyEvent&);
	void keyReleased(const OIS::KeyEvent&);

#if OGRE_PLATFORM == OGRE_PLATFORM_IPHONE
	void injectMouseMove(const OIS::MultiTouchEvent&);
	void injectMouseDown(const OIS::MultiTouchEvent&);
#else
	void mouseMoved(const OIS::MouseEvent&);
	void mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
	void mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);
#endif

	const Cursor& GetCursor();


private:
	Body* mBody;
	Ogre::Camera* mCamera;
	Ogre::Radian mCameraYaw;

	std::auto_ptr< Cursor > mCursor;

	Ogre::Real mRotatingDirection;
};