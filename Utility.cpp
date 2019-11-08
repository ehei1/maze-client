#include <StdAfx.h>
#include "Utility.h"
#include "Application.h"

namespace Utility
{
	void OutputDebug(const wchar_t* text, ...)
	{
#ifdef _DEBUG
		TCHAR buffer[MAX_PATH] = {0};
		va_list listPointer;
		va_start(listPointer, text);
		_vsntprintf(
			buffer,
			_countof(buffer),
			text,
			listPointer);
		OutputDebugString(buffer);
#endif
	}

	void OutputDebug(const char* text, ...)
	{
#ifdef _DEBUG
		char buffer[MAX_PATH] = {0};
		va_list listPointer;
		va_start(listPointer, text);

		vsprintf(
			buffer,
			text,
			listPointer);
		OutputDebugStringA(buffer);
#endif
	}


	// http://www.ogre3d.org/forums/viewtopic.php?f=2&t=62609
	Ogre::Vector3 GetDirection(const Ogre::SceneNode& sceneNode)
	{
		const Ogre::Quaternion quaternion(sceneNode.getOrientation().getYaw(), Ogre::Vector3::UNIT_Y);
		const Ogre::Vector3 direction = quaternion * Ogre::Vector3::UNIT_Z;

		return direction.normalisedCopy();
	}


	// http://www.ogre3d.org/tikiwiki/Projecting+3D+position+and+size+to+2D&structure=Cookbook
	bool ProjectPosition(
		const Ogre::Camera& camera,
		const Ogre::Vector3& position,
		Ogre::Vector2& projectedPosition)
	{
		const Ogre::Vector3 eyeSpacePos = camera.getViewMatrix(true) * position;

		// z < 0 means in front of cam
		if(eyeSpacePos.z < 0)
		{
			const Ogre::RenderWindow* const renderWindow = Application::GetSingleton().GetRenderWindow();
			assert(nullptr != renderWindow);

			const Ogre::Real windowWidth = Ogre::Real(renderWindow->getWidth());
			const Ogre::Real windowHeight = Ogre::Real(renderWindow->getHeight());

			// calculate projected pos. range: -1 ~ 1
			const Ogre::Vector3 screenSpacePos = camera.getProjectionMatrix() * eyeSpacePos;
			projectedPosition.x = Ogre::Real(windowWidth / 2) * (1 + screenSpacePos.x);
			projectedPosition.y = Ogre::Real(windowHeight / 2) * (1 - screenSpacePos.y);

			const bool isInHeight = (0 < projectedPosition.y) && (windowHeight > projectedPosition.y);
			const bool isInWidth = (0 < projectedPosition.x) && (windowWidth > projectedPosition.x);

			return isInWidth && isInHeight;
		}
		else
		{
			projectedPosition.x = (-eyeSpacePos.x > 0) ? -1 : 1;
			projectedPosition.y = (-eyeSpacePos.y > 0) ? -1 : 1;

			return false;
		}
	}


	// http://scripter.egloos.com/1618411
	void RotateXZ(
		Ogre::Real& x,
		Ogre::Real& z,
		const Ogre::Radian& radian,
		const Ogre::Vector3& axis,
		const Ogre::Vector3& position)
	{
		const Ogre::Vector3 distance = position - axis;

		x = axis.x + distance.x * Ogre::Math::Cos(radian) - distance.z * Ogre::Math::Sin(radian);
		z = axis.z + distance.x * Ogre::Math::Sin(radian) + distance.z * Ogre::Math::Cos(radian);
	}


	// http://www.cegui.org.uk/phpBB2/viewtopic.php?f=10&t=5134&p=24543&hilit=Ogre+mouse+input#p24543
	bool IsMouseOnGui()
	{
		CEGUI::Window* const window = CEGUI::System::getSingleton().getWindowContainingMouse();

		if(nullptr == window)
		{
			return false;
		}

		return nullptr != window->getParent();
	}


	int GetUniqueIndex()
	{
		static int mUniqueIndex;

		return ++mUniqueIndex;
	}
}