#include "StdAfx.h"
#include "NetworkPlay.h"
#include "Application.h"

namespace State
{
	NetworkPlay::NetworkPlay(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager) :
		State(name, application, sceneManager),
		mSceneNode(nullptr)
	{
	}

	NetworkPlay::~NetworkPlay(void)
	{
	}

	void NetworkPlay::enter()
	{
		mSceneNode = (Ogre::SceneNode*)mSceneManager.getRootSceneNode()->createChildSceneNode(
			GetName() + "/node/root");

		CEGUI::System::getSingleton().executeScriptFile( "networkPlay.enter.lua" );
	}

	void NetworkPlay::pause()
	{
		mSceneManager.getRootSceneNode()->removeChild( mSceneNode );
	}
	void NetworkPlay::resume()
	{
		mSceneManager.getRootSceneNode()->addChild( mSceneNode );
	}

	void NetworkPlay::exit()
	{
		Application::GetSingleton().GetNetwork().End();

		mSceneManager.destroySceneNode( mSceneNode );

		CEGUI::System::getSingleton().executeScriptFile( "networkPlay.exit.lua" );
	}

	bool NetworkPlay::keyPressed(const OIS::KeyEvent& event)
	{
		switch( event.key )
		{
		case OIS::KC_ESCAPE:
			{
				//CEGUI::Window* guiSheet = CEGUI::System::getSingleton().getGUISheet();
				//if( guiSheet == nullptr )
				//{
				//	break;
				//}

				//CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
				//if( winMgr.isWindowPresent( "exitWindow" ) == false )
				//{
				//	winMgr.loadWindowLayout( "ExitWindow.layout" );
				//}
				//CEGUI::Window* exitWindow = winMgr.getWindow( "exitWindow" );
				//if( exitWindow == nullptr )
				//{
				//	break;
				//}

				//bool isExitWindowActivated = (exitWindow == nullptr ? false : exitWindow->isActive());
				//if( isExitWindowActivated == true )
				//{
				//	exitWindow->deactivate();
				//	guiSheet->removeChildWindow( exitWindow );
				//}
				//else
				//{
				//	exitWindow->activate();
				//	guiSheet->addChildWindow( exitWindow );	
				//}
			}
			break;
		}

		return State::keyPressed( event );
	}

	void NetworkPlay::CreateScene()
	{
	}
}