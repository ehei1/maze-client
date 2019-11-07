#pragma once


class Body;


class BodyFactory
{
	friend class Application;


public:
	enum Type
	{
		TypeNone,
		TypeWall,
		TypeDoor,
		TypeOrc,
		TypeSword,
		TypeBox,
	};

public:
	virtual ~BodyFactory();

	void Release();	
	Body* AddBody(Type, const Ogre::String& parentNodeName, const Ogre::String& bodyName);

	Body* GetBody(const Ogre::String&);
	Body* GetBody(int bodyIndex);

	void RemoveBody(const Ogre::String& name);
	void frameRendering(const Ogre::FrameEvent&);


private:
	BodyFactory();

	// change to shared_ptr or auto_ptr
	typedef std::map< Ogre::String, Body* > BodyContainer;
	BodyContainer mBodyContainer;

	typedef std::map< int, Body* > IndexedBodyContainer;
	IndexedBodyContainer mIndexedBodyContainer;
};