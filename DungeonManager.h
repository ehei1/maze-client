#pragma once


namespace Dungeon
{
	class Parser;
	class Blueprint;


	struct WallProperty
	{
		Ogre::String mMeshFileName;
		Ogre::String mMaterialName;
	};

	struct DoorProperty
	{
		enum Type
		{
			TypeHinged,
			TypeSliding,
			TypeUpAndOver,
		}
		mType;
		
		Ogre::Real mOpeningSpeed;

		struct Door
		{
			Ogre::Vector3 mPosition;
			Ogre::Vector3 mScale;
			Ogre::Vector3 mAxis;
			Ogre::Vector3 mSize;

			Ogre::String mMeshFileName;
			Ogre::String mMaterialName;
		};
		typedef std::list< Door > DoorContainer;
		DoorContainer mDoorContainer;

		struct Frame
		{
			Ogre::Vector3 mPosition;
			Ogre::Vector3 mScale;
			Ogre::Vector3 mSize;

			Ogre::String mMeshFileName;
			Ogre::String mMaterialName;
		};
		typedef std::list< Frame > FrameContainer;
		FrameContainer mFrameContainer;
	};

	struct TileProperty
	{
		Ogre::String mName;
		Ogre::Real mWidth;
		Ogre::Real mHeight;

		WallProperty mWall;
		DoorProperty mDoor;
	};


	class Manager
	{
	public:		
		Manager();
		virtual ~Manager();

		void Release();

		void RandomizeBlueprint(int width, int height);
		const POINT GetStartPoint() const;

		void Save(const char*) const;
		bool Load(const char*);

		void Build(Ogre::SceneNode&, Ogre::SceneManager&);

		void LoadTileProperty(const char* filePath);


	private:
		TileProperty mTileProperty;

		std::auto_ptr< Parser > mParser;
		std::auto_ptr< Blueprint > mBlueprint;

		std::list< Ogre::String > mDoorContainer;

		OgreNewt::Body* mFloorBody;
	};
}
