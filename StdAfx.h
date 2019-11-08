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

#include <OgreSDK_vc10_v1-7-4/include/OIS/OISEvents.h>
#include <OgreSDK_vc10_v1-7-4/include/OIS/OISInputManager.h>
#include <OgreSDK_vc10_v1-7-4/include/OIS/OISKeyboard.h>
#include <OgreSDK_vc10_v1-7-4/include/OIS/OISMouse.h>
#include <OgreSDK_vc10_v1-7-4/include/OGRE/Ogre.h>
#include <OgreSDK_vc10_v1-7-4/include/OGRE/OgreTextAreaOverlayElement.h>
#include <OgreSDK_vc10_v1-7-4/Samples/Common/include/SdkCameraMan.h>
#include <OgreNewt 2/inc/OgreNewt.h>
#include <OgreNewt 2/inc/OgreNewt_BasicFrameListener.h>

#include <CEGUI-SDK-0.7.5-vc10/cegui/include/CEGUI.h>
#include <CEGUI-SDK-0.7.5-vc10/cegui/include/ScriptingModules/LuaScriptModule/CEGUILua.h>
#include <CEGUI-SDK-0.7.5-vc10/cegui/include/RendererModules/Ogre/CEGUIOgreRenderer.h>

extern "C" {
#include <CEGUI-SDK-0.7.5-vc10/dependencies\include\lua.h>
#include <CEGUI-SDK-0.7.5-vc10/dependencies\include\lauxlib.h>
#include <CEGUI-SDK-0.7.5-vc10/dependencies\include\lualib.h>
};

#include "tinyxml.h"

#include "Network/ShinCommon.h"

#pragma warning(pop)