#pragma once


#include "State.h"


class SoulController;
class EntityLoadListener;

namespace Dungeon { class Blueprint; }
namespace Dungeon { class Manager; }


namespace State
{
	class SinglePlayState : public State
	{
	public:
		SinglePlayState(const Ogre::String& name, Application&, Ogre::SceneManager&);
		virtual ~SinglePlayState();

		void LoadDungeon(const char*);
		void RebuildDungeon();
		void RandomizeDungeon(int row, int column);

		Dungeon::Manager& GetDungeonManager();

		void SyncPlayer();


	private:
		virtual void enter();
		virtual void pause();
		virtual void resume();
		virtual void exit();
		virtual bool frameStarted(const Ogre::FrameEvent&);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent&);
		virtual bool keyPressed(const OIS::KeyEvent&);
		virtual bool keyReleased(const OIS::KeyEvent&);
		virtual bool mouseMoved(const OIS::MouseEvent&);
		virtual bool mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID);
		virtual bool mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID);

		void PutCreature();
		void CreateCollisionField();
		//void PutBox(const Ogre::Vector3&, Ogre::Real height);
		void PutField(
			const Ogre::Vector2& blockCount,
			Ogre::Real blockSize);


	private:
		Ogre::SceneNode* mSceneNode;
		Ogre::SceneNode* mDungeonNode;

		Ogre::ManualObject* mCursorObject;

		Ogre::AnimationState* mAnimationState;
		std::auto_ptr< SoulController > mSoulController;

		std::queue< EntityLoadListener* > mEntityListenerContainer;

		std::auto_ptr< Dungeon::Manager > mDungeonManager;
	};
}