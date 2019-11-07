#pragma once

#include "State.h"

namespace State
{
	class NetworkPlay : public State
	{
	public:
		NetworkPlay(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager);
		~NetworkPlay(void);

	private:		
		virtual void enter();
		virtual void pause();
		virtual void resume();
		virtual void exit();
		virtual bool keyPressed(const OIS::KeyEvent&);

		void CreateScene();

		Ogre::SceneNode* mSceneNode;
	};
}