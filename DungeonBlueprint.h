#pragma once


namespace Dungeon
{
	class Blueprint
	{
	public:		
		typedef std::list< RECT > BlockContainer;
#ifndef _AFX_ALL_WARNINGS
		typedef Ogre::IteratorWrapper< BlockContainer, BlockContainer::const_iterator, RECT > BlockIterator;
#endif
		enum TileType
		{
			TileNone,
			TileWall,
			TileBoundary,
			TileCorridor,
			TileFloor,
			TileDoor,
			TileUpStairs,
			TileDownStairs,
			TileChest,
			TileBlock,
		};

		enum Side
		{
			SideNone,
			SideWest,
			SideEast,
			SideNorth,
			SideSouth,
		};

		struct TileProperty
		{
			TileProperty() :
			mType(TileNone),
			mSide(SideNone)
			{}

			TileType mType;
			Side mSide;
		};

		typedef std::pair< int, int > Tile;
		typedef std::map< Tile, TileProperty > TileContainer;
#ifndef _AFX_ALL_WARNINGS
		typedef Ogre::ConstMapIterator< TileContainer > TileIterator;
#endif


	public:
		Blueprint(void);
		virtual ~Blueprint(void);

		bool Load(const char* fileName);
		bool Randomize(int width, int height);
		void PutToConsole();
		
		const POINT GetStartPoint() const;

#ifndef _AFX_ALL_WARNINGS
		BlockIterator GetBlockIterator() const;
		TileIterator GetTileIterator() const;
#endif
		inline void SetTileSize(int size) { mTileSize = size; }


#ifdef _AFX_ALL_WARNINGS
		// LUJ, MFC�� �� �׽�Ʈ Ŭ���̾�Ʈ���� ���� ǥ���� �� ���
		void DrawTile(CPaintDC&, const POINT&, size_t tileSize) const;
		void DrawBlock(CPaintDC&, const POINT&, size_t tileSize) const;
#endif


	private:
		void SetTileType(int x, int y, TileType);
		TileType GetTileType(int x, int y) const;
		int GetValueRandomly(int min, int max) const;
		Side GetSideRandomly() const;
		bool BuildCorridor(int x, int y, int length, Side);
		bool BuildRoom(int x, int y, int xlength, int ylength, Side);
		void BuildCollisionBlock();
		void BuildStartEndPoint();
		TileProperty& GetTileProperty(int x, int y);


	private:
		struct Property
		{
			int mWidth;
			int mHeight;
			// LUJ, ������ ��� ��쿡�� ���簢�� �ȿ� �����ȴ�. �Ʒ� ���� ���꿡 ���ȴ�
			int mSize;
		}
		mProperty;
			
		TileContainer mTileContainer;

		POINT mStartPosition;

		int chanceRoom;
		int chanceCorridor;
		int mTileSize;

		BlockContainer mBlockContainer;
	};
}
