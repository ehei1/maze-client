#include <StdAfx.h>
#include "DungeonParser.h"
#include "DungeonManager.h"


namespace Dungeon
{
	Parser::Parser()
	{
	}


	Parser::~Parser()
	{
	}


	void Parser::LoadProperty(const char* fileName, TileProperty& tileProperty) const
	{
		TiXmlDocument document(fileName);

		if(false == document.LoadFile())
		{
			return;
		}

		tileProperty.mDoor.mDoorContainer.clear();
		tileProperty.mDoor.mFrameContainer.clear();

		struct ParserByKeyword
		{
			const char* mKeyword;
			ParsingFunction mFunction;
		};

		const ParserByKeyword parserByKeyword[] = 
		{
			{"global", Parser::ParseGlobal},
			{"wall", Parser::ParseTileWall},
			{"door", Parser::ParseTileDoor},
		};

		const TiXmlNode* rootNode = document.FirstChild("tile");
		
		for(size_t i = 0; i < _countof(parserByKeyword); ++i)
		{
			const ParserByKeyword& parser = parserByKeyword[i];
				
			if(const TiXmlNode* const childNode = rootNode->FirstChild(parser.mKeyword))
			{
				(*parser.mFunction)(*childNode, tileProperty);
			}
		}
	}


	void Parser::ParseGlobal(const TiXmlNode& rootNode, TileProperty& tileProperty)
	{
		if(const TiXmlNode* const childNode = rootNode.FirstChild("name"))
		{
			const TiXmlElement* const element = childNode->ToElement();

			tileProperty.mName = element->Attribute("value");
		}

		if(const TiXmlNode* const childNode = rootNode.FirstChild("size"))
		{
			const TiXmlElement* const element = childNode->ToElement();

			tileProperty.mWidth = Ogre::Real(atof(element->Attribute("width")));
			tileProperty.mHeight = Ogre::Real(atof(element->Attribute("height")));
		}
	}


	void Parser::ParseTileWall(const TiXmlNode& rootNode, TileProperty& tileProperty)
	{
		WallProperty& property = tileProperty.mWall;

		if(const TiXmlNode* const childNode = rootNode.FirstChild("mesh"))
		{
			const TiXmlElement* const element = childNode->ToElement();

			property.mMeshFileName = element->Attribute("file");
			property.mMaterialName = element->Attribute("material");
		}
	}


	void Parser::ParseTileDoor(const TiXmlNode& rootNode, TileProperty& tileProperty)
	{
		DoorProperty& property = tileProperty.mDoor;

		{
			const TiXmlElement* const element = rootNode.ToElement();

			property.mType = Dungeon::DoorProperty::Type(atoi(element->Attribute("type")));
			property.mOpeningSpeed = Ogre::Real(atof(element->Attribute("openingSpeed")));
		}

		for(const TiXmlNode* parentNode = rootNode.FirstChild();
			nullptr != parentNode;
			parentNode = parentNode->NextSibling())
		{
			if(0 == stricmp("door", parentNode->Value()))
			{
				property.mDoorContainer.push_back(DoorProperty::Door());
				DoorProperty::Door& door = property.mDoorContainer.back();

				if(const TiXmlNode* const meshNode = parentNode->FirstChild("mesh"))
				{
					const TiXmlElement* const element = meshNode->ToElement();

					door.mMeshFileName = element->Attribute("file");
					door.mMaterialName = element->Attribute("material");
				}

				if(const TiXmlNode* const positionNode = parentNode->FirstChild("position"))
				{
					const TiXmlElement* const element = positionNode->ToElement();

					door.mPosition.x = Ogre::Real(atof(element->Attribute("x")));
					door.mPosition.y = Ogre::Real(atof(element->Attribute("y")));
					door.mPosition.z = Ogre::Real(atof(element->Attribute("z")));
				}

				if(const TiXmlNode* const scaleNode = parentNode->FirstChild("scale"))
				{
					const TiXmlElement* const element = scaleNode->ToElement();

					door.mScale.x = Ogre::Real(atof(element->Attribute("x")));
					door.mScale.y = Ogre::Real(atof(element->Attribute("y")));
					door.mScale.z = Ogre::Real(atof(element->Attribute("z")));
				}

				if(const TiXmlNode* const axisNode = parentNode->FirstChild("axis"))
				{
					const TiXmlElement* const element = axisNode->ToElement();

					door.mAxis.x = Ogre::Real(atof(element->Attribute("x")));
					door.mAxis.y = Ogre::Real(atof(element->Attribute("y")));
					door.mAxis.z = Ogre::Real(atof(element->Attribute("z")));
				}

				if(const TiXmlNode* const scaleNode = parentNode->FirstChild("size"))
				{
					const TiXmlElement* const element = scaleNode->ToElement();

					door.mSize.x = Ogre::Real(atof(element->Attribute("x")));
					door.mSize.y = Ogre::Real(atof(element->Attribute("y")));
					door.mSize.z = Ogre::Real(atof(element->Attribute("z")));
				}
			}
			else if(0 == stricmp("frame", parentNode->Value()))
			{
				property.mFrameContainer.push_back(DoorProperty::Frame());
				DoorProperty::Frame& frame = property.mFrameContainer.back();

				if(const TiXmlNode* const meshNode = parentNode->FirstChild("mesh"))
				{
					const TiXmlElement* const element = meshNode->ToElement();

					frame.mMeshFileName = element->Attribute("file");
					frame.mMaterialName = element->Attribute("material");
				}

				if(const TiXmlNode* const positionNode = parentNode->FirstChild("position"))
				{
					const TiXmlElement* const element = positionNode->ToElement();

					frame.mPosition.x = Ogre::Real(atof(element->Attribute("x")));
					frame.mPosition.y = Ogre::Real(atof(element->Attribute("y")));
					frame.mPosition.z = Ogre::Real(atof(element->Attribute("z")));
				}

				if(const TiXmlNode* const scaleNode = parentNode->FirstChild("scale"))
				{
					const TiXmlElement* const element = scaleNode->ToElement();

					frame.mScale.x = Ogre::Real(atof(element->Attribute("x")));
					frame.mScale.y = Ogre::Real(atof(element->Attribute("y")));
					frame.mScale.z = Ogre::Real(atof(element->Attribute("z")));
				}

				if(const TiXmlNode* const scaleNode = parentNode->FirstChild("size"))
				{
					const TiXmlElement* const element = scaleNode->ToElement();

					frame.mSize.x = Ogre::Real(atof(element->Attribute("x")));
					frame.mSize.y = Ogre::Real(atof(element->Attribute("y")));
					frame.mSize.z = Ogre::Real(atof(element->Attribute("z")));
				}
			}
		}
	}
}
