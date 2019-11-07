#pragma once


#include "State.h"


namespace State
{
	class MainMenuState : public State
	{
	public:
		MainMenuState(const Ogre::String& name, Application&, Ogre::SceneManager&);
		virtual ~MainMenuState();


	private:		
		virtual void enter();
		virtual void pause();
		virtual void resume();
		virtual void exit();
		virtual bool keyPressed(const OIS::KeyEvent&);


	private:
		Ogre::SceneNode* mSceneNode;
	};
}