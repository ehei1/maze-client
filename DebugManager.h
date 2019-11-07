#pragma once


class DebugManager :
	public Ogre::FrameListener
{
	friend class Application;


public:
	virtual ~DebugManager();

	virtual bool frameRenderingQueued(const Ogre::FrameEvent&);

	typedef std::pair< Ogre::Vector3, Ogre::Vector3 > LinePosition;
	typedef std::list< LinePosition > LinePositionContainer;
	void DrawLine(const LinePosition&, const Ogre::ColourValue&);
	void DrawLine(const LinePositionContainer&, const Ogre::ColourValue&);

	

private:
	DebugManager();

	void Initialize();
	void Release();



private:
	typedef std::pair< LONGLONG, Ogre::MovableObject* > DebugObject;
	std::list< DebugObject > mDebugObjectContainer;

	Ogre::SceneNode* mRootSceneNode;

	Ogre::TextAreaOverlayElement* mDebugOverlayElement;
};