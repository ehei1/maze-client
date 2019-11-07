#pragma once

#include "State.h"

namespace State
{
	class ConnectMenuState :
		public State	
	{
	public:
		ConnectMenuState(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager);
		~ConnectMenuState(void);

		void ConnectServer( const char* ip, const char* port );

	private:		
		virtual void enter();
		virtual void pause();
		virtual void resume();
		virtual void exit();
		virtual bool keyPressed(const OIS::KeyEvent&);

		Ogre::SceneNode* mSceneNode;
	};
}