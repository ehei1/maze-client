#pragma once


class Application;


namespace State
{
	class State :
		public Ogre::FrameListener
	{
	public:
		State(const Ogre::String& name, Application&, Ogre::SceneManager&);
		virtual ~State();	

		const Ogre::String& GetName() const { return mName; }

		virtual void enter() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual void exit() = 0;

		virtual bool keyPressed(const OIS::KeyEvent&) { return true; }
		virtual bool keyReleased(const OIS::KeyEvent&) { return true; }
		virtual bool mouseMoved(const OIS::MouseEvent&) { return true; }
		virtual bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) { return true; }
		virtual bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) { return true; }


	protected:
		Ogre::SceneManager& mSceneManager;
		Application& mApplication;
		const Ogre::String mName;
	};
}