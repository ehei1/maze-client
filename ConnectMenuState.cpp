#include "StdAfx.h"
#include "ConnectMenuState.h"
#include "Application.h"

namespace State
{
	ConnectMenuState::ConnectMenuState(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager) :
		State(name, application, sceneManager),
		mSceneNode(nullptr)
	{
	}

	ConnectMenuState::~ConnectMenuState(void)
	{
	}

	void ConnectMenuState::enter()
	{
		mSceneNode = (Ogre::SceneNode*)mSceneManager.getRootSceneNode()->createChildSceneNode(
			GetName() + "/node/root");

		CEGUI::System::getSingleton().executeScriptFile( "connectMenu.enter.lua", "lua_scripts" );
	}

	void ConnectMenuState::pause()
	{
		mSceneManager.getRootSceneNode()->removeChild( mSceneNode );
	}
	void ConnectMenuState::resume()
	{
		mSceneManager.getRootSceneNode()->addChild( mSceneNode );
	}

	void ConnectMenuState::exit()
	{
		mSceneManager.destroySceneNode( mSceneNode );

		CEGUI::System::getSingleton().executeScriptFile( "connectMenu.exit.lua" );
	}

	bool ConnectMenuState::keyPressed(const OIS::KeyEvent& event)
	{
		return State::keyPressed( event );
	}
}