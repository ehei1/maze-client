#include <StdAfx.h>
#include "MainMenuState.h"

namespace State
{
	MainMenuState::MainMenuState(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager) :
	State(name, application, sceneManager),
	mSceneNode(nullptr)
	{}

	MainMenuState::~MainMenuState()
	{
	}

	void MainMenuState::enter()
	{
		CEGUI::System::getSingleton().executeScriptFile(
			"mainMenu.enter.lua");
	}

	void MainMenuState::pause()
	{
		mSceneManager.getRootSceneNode()->removeChild(
			mSceneNode);
	}

	void MainMenuState::resume()
	{
		mSceneManager.getRootSceneNode()->addChild(
			mSceneNode);
	}

	void MainMenuState::exit()
	{
		CEGUI::System::getSingleton().executeScriptFile("mainMenu.exit.lua");
	}


	bool MainMenuState::keyPressed(const OIS::KeyEvent& event)
	{
		return State::keyPressed(
			event);
	}
}