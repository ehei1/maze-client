#include <StdAfx.h>
#include "DebugManager.h"
#include "CommonDefine.h"
#include "Application.h"
#include "Utility.h"


DebugManager::DebugManager() :
mRootSceneNode(nullptr),
mDebugOverlayElement(nullptr)
{

}


DebugManager::~DebugManager()
{
}


void DebugManager::Initialize()
{
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	Ogre::OverlayContainer* const overlayContainer = static_cast< Ogre::OverlayContainer* >(
		overlayManager.createOverlayElement("Panel", "PanelName" + uniqueIndex));
	overlayContainer->setMetricsMode(Ogre::GMM_PIXELS);
	overlayContainer->setDimensions(1, 1);
	overlayContainer->setWidth(100);

	const Ogre::RenderWindow* const renderWindow = Application::GetSingleton().GetRenderWindow();
	const Ogre::Real fontHeight = 20;

	mDebugOverlayElement = static_cast< Ogre::TextAreaOverlayElement* >(
		overlayManager.createOverlayElement("TextArea", "TextAreaName" + uniqueIndex));
	mDebugOverlayElement->setMetricsMode(Ogre::GMM_PIXELS);
	mDebugOverlayElement->setDimensions(1, 1);
	mDebugOverlayElement->setCaption(Ogre::DisplayString(""));
	mDebugOverlayElement->setCharHeight(fontHeight);
	mDebugOverlayElement->setPosition(
		0,
		renderWindow->getHeight() - fontHeight);
	mDebugOverlayElement->setFontName("StarWars");
	mDebugOverlayElement->setColourBottom(Ogre::ColourValue(0.3, 0.5, 0.3));
	mDebugOverlayElement->setColourTop(Ogre::ColourValue(0.5, 0.7, 0.5));
	mDebugOverlayElement->setAlignment(Ogre::TextAreaOverlayElement::Left);

	overlayContainer->addChild(mDebugOverlayElement);

	Ogre::Overlay* const overlay = overlayManager.create("OverlayName" + uniqueIndex);
	overlay->add2D(overlayContainer);
	overlay->show();
}


void DebugManager::Release()
{
	if(nullptr != mRootSceneNode)
	{
		Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();
		sceneManager.destroySceneNode(mRootSceneNode);

		mRootSceneNode = nullptr;
	}

	mDebugObjectContainer.clear();
}


bool DebugManager::frameRenderingQueued(const Ogre::FrameEvent& frameEvent)
{
	if(false == mDebugObjectContainer.empty())
	{
		DebugObject& DebugObject = mDebugObjectContainer.front();

		if(GetTickCount() > DebugObject.first)
		{
			Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();
			Ogre::MovableObject* const debugObject = DebugObject.second;
			
			if(Ogre::SceneNode* const sceneNode = debugObject->getParentSceneNode())
			{
				sceneManager.destroySceneNode(sceneNode);
			}
			else
			{
				sceneManager.destroyMovableObject(debugObject);
			}

			mDebugObjectContainer.pop_front();
		}
	}

	{
		const Ogre::RenderWindow* const renderWindow = Application::GetSingleton().GetRenderWindow();
		const Ogre::RenderTarget::FrameStats& frameState = renderWindow->getStatistics();

		mDebugOverlayElement->setCaption(Ogre::StringConverter::toString(frameState.avgFPS) + " frame/second");
	}

	return Ogre::FrameListener::frameEnded(frameEvent);
}


void DebugManager::DrawLine(const LinePosition& linePosition, const Ogre::ColourValue& colorValue)
{
	LinePositionContainer linePositionContainer;
	linePositionContainer.push_back(linePosition);

	DrawLine(linePositionContainer, colorValue);
}

// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Line+3D
void DebugManager::DrawLine(const LinePositionContainer& linePositionContainer, const Ogre::ColourValue& colorValue)
{
	const Ogre::String materialName("debug/material/line");
	const Ogre::String groupName("Mesh");
	const Ogre::String uniqueIndex = Ogre::StringConverter::toString(Utility::GetUniqueIndex());

	Ogre::SceneManager& sceneManager = Application::GetSingleton().GetSceneManager();
	Ogre::ManualObject* const manualObject = sceneManager.createManualObject(
		"debug/manual/line" + uniqueIndex);
	manualObject->setQueryFlags(Common::QueryNone);

	if(nullptr == mRootSceneNode)
	{
		mRootSceneNode = sceneManager.getRootSceneNode()->createChildSceneNode("debug/node/root");
	}

	Ogre::SceneNode* const sceneNode = mRootSceneNode->createChildSceneNode();
	sceneNode->attachObject(manualObject);

	Ogre::MaterialManager::getSingleton().remove(materialName);
	Ogre::MaterialPtr materialPointer = Ogre::MaterialManager::getSingleton().create(
		materialName,
		groupName);
	materialPointer->setReceiveShadows(false);
	materialPointer->getTechnique(0)->setLightingEnabled(true);
	materialPointer->getTechnique(0)->getPass(0)->setDiffuse(colorValue);
	materialPointer->getTechnique(0)->getPass(0)->setAmbient(colorValue);
	materialPointer->getTechnique(0)->getPass(0)->setSelfIllumination(colorValue);

	manualObject->begin(materialName, Ogre::RenderOperation::OT_LINE_LIST, groupName);

	for(auto iterator = linePositionContainer.begin();
		linePositionContainer.end() != iterator;
		++iterator)
	{
		const LinePosition& linePosition = *iterator;
		manualObject->position(linePosition.first);
		manualObject->position(linePosition.second);
	}

	manualObject->end();

	{
		const ULONGLONG removingTime = 200;

		mDebugObjectContainer.push_back(
			DebugObject(GetTickCount() + removingTime, manualObject));
	}
}