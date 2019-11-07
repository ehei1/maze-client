#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinNT.h>
#include <tchar.h>
#include <memory>
#include <stack>
#include <list>
#include <vector>
#include <limits>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <queue>

#pragma warning(push)
#pragma warning (disable:4244)
#pragma warning (disable:4512)
#pragma warning (disable:4127)

#include <tbb3.0 update 5/include/tbb/tbb.h>
#include <tbb3.0 update 5/include/tbb/tbbmalloc_proxy.h>
#include <Ogre 1.7.2/Dependencies/include/OIS/OISEvents.h>
#include <Ogre 1.7.2/Dependencies/include/OIS/OISInputManager.h>
#include <Ogre 1.7.2/Dependencies/include/OIS/OISKeyboard.h>
#include <Ogre 1.7.2/Dependencies/include/OIS/OISMouse.h>
#include <Ogre 1.7.2/source/OgreMain/include/Ogre.h>
#include <Ogre 1.7.2/source/OgreMain/include/OgreTextAreaOverlayElement.h>
#include <Ogre 1.7.2/source/Samples/Common/include/SdkCameraMan.h>
#include <OgreNewt 2/inc/OgreNewt.h>
#include <OgreNewt 2/inc/OgreNewt_BasicFrameListener.h>

#include <CEGUI-0.7.5/cegui/include/CEGUI.h>
#include <CEGUI-0.7.5/cegui/include/ScriptingModules/LuaScriptModule/CEGUILua.h>
#include <CEGUI-0.7.5/cegui/include/RendererModules/Ogre/CEGUIOgreRenderer.h>

extern "C" {
#include <CEGUI-0.7.5/dependencies\include\lua.h>
#include <CEGUI-0.7.5/dependencies\include\lauxlib.h>
#include <CEGUI-0.7.5/dependencies\include\lualib.h>
};

#include "tinyxml.h"

#include "Network/ShinCommon.h"

#pragma warning(pop)