#include <StdAfx.h>
#include "Tooltip.h"
#include "Application.h"
#include "Utility.h"


Tooltip::Tooltip() :
mOverlayContainer(nullptr),
mOverlay(nullptr),
mIsVisible(false)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	mOverlayContainer = static_cast< Ogre::OverlayContainer* >(
		overlayManager.createOverlayElement("Panel", "PanelName" + uniqueIndex));
	mOverlayContainer->setMetricsMode(Ogre::GMM_PIXELS);
	mOverlayContainer->setDimensions(1, 1);
	mOverlayContainer->setWidth(100);

	mOverlay = overlayManager.create("OverlayName" + uniqueIndex);
}


Tooltip::Tooltip(Tooltip& tooltip)
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	mOverlayContainer = static_cast< Ogre::OverlayContainer* >(
		tooltip.mOverlayContainer->clone("PanelName" + uniqueIndex));

	mOverlay = overlayManager.create("OverlayName" + uniqueIndex);
}


Tooltip::~Tooltip()
{
	Ogre::OverlayManager::getSingleton().destroy(mOverlay);
}


void Tooltip::AddCaption(
	const Ogre::ColourValue& topColour,
	const Ogre::ColourValue& bottomColour,
	LPCTSTR caption, ...)
{
	TCHAR buffer[MAX_PATH] = {0};
	va_list variableArgument;
	va_start(variableArgument, caption);

	_vsntprintf_s(
		buffer,
		_countof(buffer),
		_TRUNCATE,
		caption,
		variableArgument);

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	Ogre::TextAreaOverlayElement* const textArea = static_cast< Ogre::TextAreaOverlayElement* >(
		overlayManager.createOverlayElement("TextArea", "TextAreaName" + uniqueIndex));
	textArea->setMetricsMode(Ogre::GMM_PIXELS);
	textArea->setPosition(0, 0);
	textArea->setDimensions(1, 1);
	textArea->setCaption(Ogre::DisplayString(buffer));
	textArea->setCharHeight(20);
	textArea->setFontName("FontTooltip");
	textArea->setColourBottom(bottomColour);
	textArea->setColourTop(topColour);
	textArea->setAlignment(Ogre::TextAreaOverlayElement::Center);

	mOverlayContainer->addChild(textArea);
}


void Tooltip::SetPosition(const Ogre::Vector3& position)
{
	Ogre::Vector2 projectedPosition;
	Ogre::Camera* const camera = Application::GetSingleton().GetCameraMan()->getCamera();
	assert(nullptr != camera);

	if(false == Utility::ProjectPosition(
		*camera,
		position,
		projectedPosition))
	{
		mOverlay->hide();
		return;
	}

	mOverlay->show();
	mOverlayContainer->setPosition(
		projectedPosition.x,
		projectedPosition.y);
}


void Tooltip::SetVisible(bool isVisible)
{
	mIsVisible = isVisible;

	if(false == isVisible)
	{
		mOverlay->remove2D(mOverlayContainer);
	}
	else if(nullptr == mOverlay->getChild(mOverlayContainer->getName()))
	{
		mOverlay->add2D(mOverlayContainer);
	}
}