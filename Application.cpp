/*
-----------------------------------------------------------------------------
Filename:    Application.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _ 
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|                                 
	  Ogre 1.7.x Application Wizard for VC9 (January 2010)
	  http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/
#include <StdAfx.h>
#include "Application.h"
#include "MainMenuState.h"
#include "ConnectMenuState.h"
#include "NetworkPlay.h"
#include "SinglePlayState.h"
#include "tolua.h"
#include "Utility.h"
#include "BodyFactory.h"
#include "DebugManager.h"
#include "StencilQueueListener.h"
#include "CustomSceneManager.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "res/resource.h"
#endif

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}

//-------------------------------------------------------------------------------------
Application::Application(void) :
	mRoot(nullptr),
	mCamera(nullptr),
	mCameraMan(nullptr),
	mSceneManager(nullptr),
	mWindow(nullptr),
	mInputManager(nullptr),
	mMouse(nullptr),
	mKeyboard(nullptr),
	mCurrentState(nullptr),
	mBodyFactory(new BodyFactory),
	mDebugManager(new DebugManager),
	mNewtonWorld(new OgreNewt::World),
	mServerConnectIdx(0)
{}
	
//-------------------------------------------------------------------------------------
Application::~Application(void)
{
	mRoot->removeFrameListener(mDebugManager.get());

	std::for_each(
		mStateContainer.begin(),
		mStateContainer.end(),
		[](StateContainer::value_type& value) { Utility::SafeDelete(value.second); } );

	if ( ! mRemoveListenerQueue.empty() )
	{
		if(auto listener = mRemoveListenerQueue.front())
		{
			Utility::SafeDelete(listener);

			mRemoveListenerQueue.pop();
		}
	}

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);

	Utility::SafeDelete(
		mCameraMan);
	Utility::SafeDelete(
		mRoot);
}

Application& Application::GetSingleton()
{
	static Application instance;

	return instance;
}

//-------------------------------------------------------------------------------------
bool Application::configure(void)
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if(mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Application Render Window");

		// Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		HWND hwnd;
		mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
		LONG iconID   = (LONG)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON));
		SetClassLong(hwnd, GCL_HICON, iconID);
#endif
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------------------------------
void Application::chooseSceneManager(void)
{
	// LUJ, It supports Hardware Occlusion Query
	mRoot->addSceneManagerFactory(new CustomSceneManagerFactory);

	mSceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC, "CustomSceneManager");
}
//-------------------------------------------------------------------------------------
void Application::createCamera(void)
{
	// Create the camera
	mCamera = mSceneManager->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,300,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,100,50));
	mCamera->setNearClipDistance(5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
	mCameraMan->setStyle(OgreBites::CS_ORBIT);
}
//-------------------------------------------------------------------------------------
void Application::createFrameListener(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	// insert the following lines right before calling mInputSystem = OIS::InputManager::createInputSystem(paramList);
#ifdef _DEBUG
	// http://www.ogre3d.org/tikiwiki/Using+OIS#Non-exclusive_input
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#endif
	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	//tell OIS about the window's dimensions
	{
		unsigned int width, height, depth;
		int top, left;
		mWindow->getMetrics(width, height, depth, left, top);
		const OIS::MouseState &ms = mMouse->getMouseState();
		ms.width = width;
		ms.height = height;
	}

	mRoot->addFrameListener(this);
	mRoot->addFrameListener(mDebugManager.get());

	//// create newton listener
	//{
	//	OgreNewt::BasicFrameListener* basicFrameListener = new OgreNewt::BasicFrameListener(
	//		mWindow,
	//		mNewtonWorld->get(),
	//		100);
	//	mRoot->addFrameListener(
	//		basicFrameListener);
	//}
	
	{
		// LUJ, 게임 중에 제거하려면 포인터를 저장해둔다.
		mSceneManager->addRenderQueueListener(new StencilQueueListener);
	}

	delete mNewtonWorld;
	mNewtonWorld = nullptr;
}
//-------------------------------------------------------------------------------------
void Application::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void Application::createViewports(void)
{
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void Application::setupResources(void)
{
	// Load resource paths from config file
	Ogre::ConfigFile cf;
#ifdef _DEBUG
	cf.load("resources_d.cfg");
#else
	cf.load("resources.cfg");
#endif
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	while(true == seci.hasMoreElements())
	{
		const Ogre::String& secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		
		for(auto i = settings->begin(); i != settings->end(); ++i)
		{
			const Ogre::String& typeName = i->first;
			const Ogre::String& archName = i->second;

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void Application::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void Application::loadResources(void)
{
	//mTrayMgr->showLoadingBar(1, 1, 0);

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Default");

	//mTrayMgr->hideLoadingBar();
}
//-------------------------------------------------------------------------------------
void Application::go(void)
{
	if (!setup())
		return;

	SelectState("singlePlay");

	mRoot->startRendering();

	// clean up
	destroyScene();
}
//-------------------------------------------------------------------------------------
bool Application::setup(void)
{
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg");
#else
	mRoot = new Ogre::Root("plugins.cfg");
#endif
	setupResources();

	bool carryOn = configure();
	if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	loadResources();
	// Create any resource listeners (for loading screens)
	createResourceListener();
	createFrameListener();
	createState();

	// LUJ, intialize CEGUI
	{
		CEGUI::OgreRenderer::bootstrapSystem();

		/*
		// cannot found resource... omg

		// set directories
		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

		CEGUI::LuaScriptModule& scriptModule(
			CEGUI::LuaScriptModule::create(luaL_newstate()));
		luaL_openlibs(scriptModule.getLuaState());
		tolua_toLua_open(scriptModule.getLuaState());

		CEGUI::System::getSingleton().setScriptingModule(&scriptModule);
		CEGUI::System::getSingleton().executeScriptFile("application.lua");
		*/
	}

	mDebugManager->Initialize();

	return true;
};


bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;

	if(mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	mCameraMan->frameRenderingQueued(evt);
	CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);

	if ( ! mRemoveListenerQueue.empty() )
	{
		if(auto listener = mRemoveListenerQueue.front())
		{
			Utility::SafeDelete(listener);

			mRemoveListenerQueue.pop();
		}
	}

	return true;
}
//-------------------------------------------------------------------------------------
bool Application::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		Ogre::PolygonMode pm;

		switch (mCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			pm = Ogre::PM_POINTS;
			break;
		default:
			pm = Ogre::PM_SOLID;
			break;
		}

		mCamera->setPolygonMode(pm);
	}
	else if(arg.key == OIS::KC_F5)   // refresh all textures
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}

	//mCameraMan->injectKeyDown(arg);

	if(mCurrentState)
	{
		mCurrentState->keyPressed(arg);
	}

	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);
	return true;
}

bool Application::keyReleased(const OIS::KeyEvent &arg)
{
	mCameraMan->injectKeyUp(arg);

	if(mCurrentState)
	{
		mCurrentState->keyReleased(
			arg);
	}

	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	return true;
}

bool Application::mouseMoved(const OIS::MouseEvent &mouseEvent)
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	//sys.injectMouseMove(mouseEvent.state.X.rel, mouseEvent.state.Y.rel);
	sys.injectMousePosition(mouseEvent.state.X.abs, mouseEvent.state.Y.abs);

	// Scroll wheel.
	if (mouseEvent.state.Z.rel)
	{
		sys.injectMouseWheelChange(mouseEvent.state.Z.rel / 120.0f);
		
		Ogre::Vector3 movedPosition = mCamera->getDirection().normalisedCopy() * mouseEvent.state.Z.rel / 50.0f;
		mCamera->setPosition(mCamera->getPosition() + movedPosition);
	}

	//mCameraMan->injectMouseMove(mouseEvent);

	if(mCurrentState)
	{
		mCurrentState->mouseMoved(mouseEvent);
	}

	return true;
}

bool Application::mousePressed(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	if(true == Utility::IsMouseOnGui())
	{
		CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
		return true;
	}

	mCameraMan->injectMouseDown(mouseEvent, id);

	if(nullptr != mCurrentState)
	{
		mCurrentState->mousePressed(mouseEvent, id);
	}

	return true;
}

bool Application::mouseReleased(const OIS::MouseEvent &mouseEvent, OIS::MouseButtonID id)
{
	if(true == Utility::IsMouseOnGui())
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
		return true;
	}

	mCameraMan->injectMouseUp(mouseEvent, id);
	
	if(mCurrentState)
	{
		mCurrentState->mouseReleased(mouseEvent, id);
	}

    return true;
}

//Adjust mouse clipping area
void Application::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void Application::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if(rw == mWindow)
	{
		if(mInputManager)
		{
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}


void Application::createState()
{
	{
		State::State* const state = new State::MainMenuState(
			"mainMenu",
			*this,
			*mSceneManager);
		mStateContainer.insert(
			std::make_pair(state->GetName(), state));
	}

	{
		State::State* const state = new State::ConnectMenuState(
			"connectMenu",
			*this,
			*mSceneManager);
		mStateContainer.insert(
			std::make_pair(state->GetName(), state));
	}

	{
		State::State* const state = new State::NetworkPlay(
			"networkPlay",
			*this,
			*mSceneManager);
		mStateContainer.insert(
			std::make_pair(state->GetName(), state));
	}

	{
		State::State* const state = new State::SinglePlayState(
			"singlePlay",
			*this,
			*mSceneManager);
		mStateContainer.insert(
			std::make_pair(state->GetName(), state));
	}
}


bool Application::SelectState(const char* name)
{
	const StateContainer::iterator iterator = mStateContainer.find(
		name);

	if(mStateContainer.end() == iterator)
	{
		return false;
	}
	else if(nullptr != mCurrentState)
	{
		if(mCurrentState->GetName() == name)
		{
			return false;
		}

		mCurrentState->exit();
		mRoot->removeFrameListener(mCurrentState);
	}
	
	mCurrentState = iterator->second;
	mCurrentState->enter();
	mRoot->addFrameListener(mCurrentState);
	
	mDebugManager->Release();
	return true;
}


void Application::Remove(Ogre::MovableObject::Listener* listener)
{
	mRemoveListenerQueue.push(listener);
}


BodyFactory& Application::GetBodyFactory()
{
	return *mBodyFactory;
}


DebugManager& Application::GetDebugManager()
{
	return *mDebugManager;
}


const Ogre::RenderWindow* Application::GetRenderWindow() const
{
	return mWindow;
}


State::State* Application::GetState(const char* name)
{
	const StateContainer::iterator iterator = mStateContainer.find(name);

	return mStateContainer.end() == iterator ? nullptr : iterator->second;
}

