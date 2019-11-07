#include <StdAfx.h>
#include "State.h"

namespace State
{
	State::State(const Ogre::String& name, Application& application, Ogre::SceneManager& sceneManager) :
	mSceneManager(sceneManager),
	mApplication(application),
	mName(name)
	{}


	State::~State()
	{}
}