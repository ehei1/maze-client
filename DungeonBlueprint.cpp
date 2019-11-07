#include <StdAfx.h>
#include "DungeonBlueprint.h"


namespace Dungeon
{
	Blueprint::Blueprint(void) :
	chanceRoom(50),
	chanceCorridor(50),
	mTileSize(1)
	{
		ZeroMemory(&mProperty, sizeof(mProperty));
		ZeroMemory(&mStartPosition, sizeof(mStartPosition));
	}


	Blueprint::~Blueprint(void)
	{
	}


	void Blueprint::SetTileType(int x, int y, TileType cellType)
	{
		mTileContainer[Tile(x, y)].mType = cellType;
	}


	Blueprint::TileType Blueprint::GetTileType(int x, int y) const
	{
		auto iterator = mTileContainer.find(Tile(x, y));

		if(mTileContainer.end() != iterator)
		{
			return iterator->second.mType;
		}

		return TileBoundary;
	}


	int Blueprint::GetValueRandomly(int min, int max) const
	{
		return min + rand() % (max - min);
	}


	bool Blueprint::BuildCorridor(int corridorX, int corridorY, int length, Side direction)
	{
		auto IsInvalid = [this](const POINT& begin, const POINT& end) -> bool
		{
			for(int y = begin.y; y <= end.y; ++y)
			{
				for(int x = begin.x; x <= end.x; ++x)
				{
					if(TileNone != this->GetTileType(x, y))
					{
						return true;
					}
				}
			}

			return false;
		};

		const int minimumLength = 2;
		
		for(int len = GetValueRandomly(minimumLength, length);
			minimumLength <= len;
			--len)
		{
			POINT begin = {0};
			POINT end = {0};

			switch(direction)
			{
			case SideNorth:
				{
					begin.x = corridorX;
					begin.y = corridorY - len;

					end.x = corridorX;
					end.y = corridorY;
					break;
				}
			case SideEast:
				{
					begin.x = corridorX;
					begin.y = corridorY;

					end.x = corridorX + len;
					end.y = corridorY;
					break;
				}
			case SideSouth:
				{
					begin.x = corridorX;
					begin.y = corridorY;

					end.x = corridorX;
					end.y = corridorY + len;
					break;
				}
			case SideWest:
				{
					begin.x = corridorX - len;
					begin.y = corridorY;

					end.x = corridorX;
					end.y = corridorY;
					break;
				}
			}

			if(true == IsInvalid(begin, end))
			{
				continue;
			}

			for(int y = begin.y; y <= end.y; ++y)
			{
				for(int x = begin.x; x <= end.x; ++x)
				{
					SetTileType(x, y, TileCorridor);
				}
			}

			return true;
		}

		return false;
	}


	bool Blueprint::BuildRoom(int roomX, int roomY, int xlength, int ylength, Blueprint::Side direction)
	{
		auto IsInvalid = [this](const POINT& begin, const POINT& end) -> bool
		{
			for(int y = begin.y; y <= end.y; ++y)
			{
				for(int x = begin.x; x <= end.x; ++x)
				{
					if(TileNone != this->GetTileType(x, y))
					{
						return true;
					}
				}
			}

			return false;
		};

		const int minimumRoomSize = 1;
		SIZE roomSize = {
			 GetValueRandomly(minimumRoomSize + 1, xlength),
			 GetValueRandomly(minimumRoomSize + 1, ylength),
		};

		for(;
			roomSize.cx > minimumRoomSize || roomSize.cy > minimumRoomSize;
			minimumRoomSize < roomSize.cx ? --roomSize.cx : roomSize.cx,
			minimumRoomSize < roomSize.cy ? --roomSize.cy : roomSize.cy)
		{
			POINT begin = {0};
			POINT end = {0};

			switch(direction)
			{
			case SideNorth:
				{
					begin.x = roomX - roomSize.cx / 2 - 1;
					begin.y = roomY - roomSize.cy - 2;

					end.x = roomX + roomSize.cx / 2 + 1;
					end.y = roomY;
					break;
				}
			case SideEast:
				{
					begin.x = roomX;
					begin.y = roomY - roomSize.cy / 2 - 1;

					end.x = roomX + roomSize.cx + 2;
					end.y = roomY + roomSize.cy / 2 + 1;
					break;
				}
			case SideSouth:
				{
					begin.x = roomX - roomSize.cx / 2 - 1;
					begin.y = roomY;

					end.x = roomX + roomSize.cx / 2 + 1;
					end.y = roomY + roomSize.cy + 2;
					break;
				}
			case SideWest:
				{
					begin.x = roomX - roomSize.cx - 2;
					begin.y = roomY - roomSize.cy / 2 - 1;

					end.x = roomX;
					end.y = roomY + roomSize.cy / 2 + 1;
					break;
				}
			}

			if(true == IsInvalid(begin, end))
			{
				continue;
			}

			for(int y = begin.y; y <= end.y; ++y)
			{
				SetTileType(begin.x, y, TileWall);
				SetTileType(end.x, y, TileWall);
			}

			for(int x = begin.x; x <= end.x; ++x)
			{
				SetTileType(x, begin.y, TileWall);
				SetTileType(x, end.y, TileWall);
			}

			for(int y = begin.y + 1; y <= end.y - 1; ++y)
			{
				for(int x = begin.x + 1; x <= end.x - 1; ++x)
				{
					SetTileType(x, y, TileFloor);
				}
			}

			return true;
		}

		return false;
	}


	bool Blueprint::Randomize(int width, int height)
	{
		mTileContainer.clear();

		mProperty.mWidth = width;
		mProperty.mHeight = height;
		mProperty.mSize = (width > height ? width : height) + 2;

		// fill boundary
		for(int y = 0; y < mProperty.mSize; ++y)
		{
			for(int x = 0; x < mProperty.mSize; ++x)
			{
				SetTileType(x, y, TileBoundary);
			}
		}

		// remove empty cell
		for(int y = (mProperty.mSize - height) / 2; y < (mProperty.mSize + height) / 2; ++y)
		{
			for(int x = (mProperty.mSize - width) / 2; x < (mProperty.mSize + width) / 2; ++x)
			{
				SetTileType(x, y, TileNone);
			}
		}
 
		//start with making a room in the middle, which we can start building upon 
		BuildRoom(
			mProperty.mSize / 2,
			mProperty.mSize / 2,
			8,
			6,
			GetSideRandomly());

		int currentFeatures = 1; //+1 for the first room we just made
	
		{
			auto IsInvalidTile = [this](int centerX, int centerY) -> bool
			{
				for(int y = centerY - 1; y <= centerY + 1; ++y)
				{
					for(int x = centerX - 1; x <= centerX + 1; ++x)
					{
						if(TileDoor == this->GetTileType(x, y))
						{
							return true;
						}
					}
				}

				const TileType tileType = this->GetTileType(centerX, centerY);

				return (TileWall != tileType) && (TileCorridor != tileType);
			};

			int beginX = mProperty.mSize / 2;
			int beginY = mProperty.mSize / 2;
			int turnDirectionCount = 0;
			int movingCount = 0;
			int beginXMovingCount = 1;
			Side movingDirection = SideWest;

			// LUJ, 중앙에서부터 시계 방향으로 돌다가 외곽을 벗어나면 빠져나온다
			for(;;)
			{
				switch(movingDirection)
				{
				case SideWest:
					{
						--beginY;
						break;
					}
				case SideEast:
					{
						++beginY;
						break;
					}
				case SideNorth:
					{
						++beginX;
						break;
					}
				case SideSouth:
					{
						--beginX;
						break;
					}
				}

				if(beginXMovingCount <= ++movingCount)
				{
					movingCount = 0;

					switch(movingDirection)
					{
					case SideWest:
						{
							movingDirection = SideNorth;
							break;
						}
					case SideEast:
						{
							movingDirection = SideSouth;
							break;
						}
					case SideNorth:
						{
							movingDirection = SideEast;
							break;
						}
					case SideSouth:
						{
							movingDirection = SideWest;
							break;
						}
					}

					if(0 == ++turnDirectionCount % 2)
					{
						turnDirectionCount = 0;
						++beginXMovingCount;
					}
				}

				if(0 > beginX || mProperty.mSize < beginX ||
					0 > beginY || mProperty.mSize < beginY)
				{
					break;
				}

				if(true == IsInvalidTile(beginX, beginY))
				{
					continue;
				}

				auto GetValidPoint = [this](int x, int y, POINT& point) -> bool
				{
					const TileType tileType = GetTileType(x, y);

					if(TileFloor != tileType &&
						TileCorridor != tileType)
					{
						return false;
					}

					point.x += point.x - x;
					point.y += point.y - y;
					return true;
				};

				POINT checkPoint = {beginX, beginY};

				for(int i = -1; i <= 1; i += 2)
				{
					if(true == GetValidPoint(beginX + i, beginY, checkPoint))
					{
						break;
					}
					else if(true == GetValidPoint(beginX, beginY + i, checkPoint))
					{
						break;
					}
				}

				Side locatingSide = SideNone;

				// get direction
				{
					const POINT direction = {beginX - checkPoint.x, (beginY - checkPoint.y) * 2};

					switch(direction.x + direction.y)
					{
					case 1:
						{
							locatingSide = SideWest;
							break;
						}
					case 2:
						{
							locatingSide = SideNorth;
							break;
						}
					case -1:
						{
							locatingSide = SideEast;
							break;
						}
					case -2:
						{
							locatingSide = SideSouth;
							break;
						}
					}
				}

				if(SideNone == locatingSide)
				{
					continue;
				}

				//choose what to build now at our newlbeginY found place, and at what direction
				int feature = GetValueRandomly(0, 100);

				if(feature <= chanceRoom)
				{
					if(BuildRoom(checkPoint.x, checkPoint.y, 8, 6, locatingSide))
					{
						currentFeatures++; //add to our quota

						//then we mark the wall opening with a door 
						SetTileType(beginX, beginY, TileDoor);

						TileProperty& tileProperty = GetTileProperty(beginX, beginY);
						tileProperty.mSide = locatingSide;

						//clean up infront of the door so we can reach it 
						SetTileType(checkPoint.x, checkPoint.y, TileFloor);
					}
				}
				else if(feature >= chanceRoom)
				{
					if(BuildCorridor(checkPoint.x, checkPoint.y, 6, locatingSide))
					{
						//same thing here, add to the quota and a door
						currentFeatures++;

						SetTileType(beginX, beginY, TileDoor);

						TileProperty& tileProperty = GetTileProperty(beginX, beginY);
						tileProperty.mSide = locatingSide;
					}
				}
			}
		}

		BuildStartEndPoint();
		BuildCollisionBlock();
		return true;
	}


#ifdef _AFX_ALL_WARNINGS
	void Blueprint::DrawTile(CPaintDC& paintDc, const POINT& point, size_t tileSize) const
	{
		for(int y = 0; y < mProperty.mSize; ++y)
		{
			for(int x = 0; x < mProperty.mSize; ++x)
			{
				CRect rectangle;
				rectangle.left = x * tileSize + point.x;
				rectangle.top = y * tileSize + point.y;
				rectangle.right = rectangle.left + tileSize;
				rectangle.bottom = rectangle.top + tileSize;

				switch(GetTileType(x, y))
				{
				case TileBlock:
				case TileNone:
					{
						CBrush bkBrush(RGB(54, 47, 45));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileWall:
					{
						CBrush bkBrush(RGB(96, 57, 19));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileFloor:
					{
						CBrush bkBrush(RGB(166, 124, 82));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileBoundary:
					{
						CBrush bkBrush(RGB(115, 99, 87));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileCorridor:
					{
						CBrush bkBrush(RGB(123, 46, 0));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileDoor:
					{
						CBrush bkBrush(RGB(0, 118, 163));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileUpStairs:
					{
						CBrush bkBrush(RGB(100, 200, 100));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileDownStairs:
					{
						CBrush bkBrush(RGB(200, 100, 100));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				case TileChest:
					{
						CBrush bkBrush(RGB(255, 247, 153));

						CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
						paintDc.FillRect(
							rectangle,
							&bkBrush);

						paintDc.SelectObject(oldBlush);
						break;
					}
				}
			}
		}
	}

	
	void Blueprint::DrawBlock(CPaintDC& paintDc, const POINT& point, size_t tileSize) const
	{
		const COLORREF colorTable[] = 
		{
			RGB(100, 0, 80),
			RGB(100, 0, 60),
			RGB(100, 0, 40),
			RGB(100, 0, 20),
			RGB(100, 100, 0),
			RGB(100, 80, 0),
			RGB(100, 60, 0),
			RGB(100, 40, 0),
			RGB(100, 20, 0),
			RGB(100, 0, 0),
			RGB(0, 100, 0),
			RGB(0, 0, 100),
			RGB(0, 0, 0),
		};

		int colorIndex = 0;

		for(auto iterator = mBlockContainer.begin();
			mBlockContainer.end() != iterator;
			++iterator)
		{
			const RECT& block = *iterator;

			CRect rectangle;
			rectangle.left = block.left * tileSize + point.x;
			rectangle.top = block.top * tileSize + point.y;
			rectangle.right = rectangle.left + (1 + block.right - block.left) * tileSize;
			rectangle.bottom = rectangle.top + (1 + block.bottom - block.top) * tileSize;

			CBrush bkBrush(colorTable[colorIndex]);
			
			colorIndex = (++colorIndex < _countof(colorTable) ? colorIndex : 0); 

			CBrush* oldBlush = paintDc.SelectObject(&bkBrush);
			paintDc.FillRect(
				rectangle,
				&bkBrush);

			paintDc.SelectObject(oldBlush);
		}
	}
#endif


	void Blueprint::PutToConsole()
	{
		for(int y = 0; y < mProperty.mSize; ++y)
		{
			for(int x = 0; x < mProperty.mSize; ++x)
			{
				switch(GetTileType(x, y)){

				case TileNone:
					_tcprintf_s(L" ");
					break;
				case TileWall:
					_tcprintf_s(L"+");
					break;
				case TileFloor:
					_tcprintf_s(L".");	
					break;
				case TileBoundary:
					_tcprintf_s(L"O");
					break;
				case TileCorridor:
					_tcprintf_s(L"#");
					break;
				case TileDoor:
					_tcprintf_s(L"D");
					break;
				case TileUpStairs:
					_tcprintf_s(L"<");
					break;
				case TileDownStairs:
					_tcprintf_s(L">");
					break;
				case TileChest:
					_tcprintf_s(L"*");
					break;
				case TileBlock:
					_tcprintf_s(L"\\");
					break;
				};

			}

			_tcprintf_s(L"\n");
		}

		_tcprintf_s(L"\n");
	}


	Blueprint::Side Blueprint::GetSideRandomly() const
	{
		const Side side[] = {
			SideNorth,
			SideWest,
			SideSouth,
			SideEast,
		};

		return side[rand() % _countof(side)];
	}


	void Blueprint::BuildCollisionBlock()
	{
		mBlockContainer.clear();
		TileContainer tileContainer = mTileContainer;

		auto GetOptimizingTile = [&tileContainer](int x, int y) -> Blueprint::TileType
		{
			auto iterator = tileContainer.find(Blueprint::Tile(x, y));

			return tileContainer.end() == iterator ? Blueprint::TileNone : iterator->second.mType;
		};

		auto SetOptimizingTile = [&tileContainer](int x, int y, TileType tileType)
		{
			tileContainer[Blueprint::Tile(x, y)].mType = tileType;
		};

		// http://scor7910.tistory.com/55
		auto IsOptimizableTile = [](TileType type) -> bool { return Blueprint::TileBoundary == type || Blueprint::TileNone == type || Blueprint::TileWall == type; };

		for(int beginY = 0; beginY < mProperty.mSize; ++beginY)
		{
			for(int beginX = 0; beginX < mProperty.mSize; ++beginX)
			{
				if(false == IsOptimizableTile(GetOptimizingTile(beginX, beginY)))
				{
					continue;
				}

				RECT block = {beginX, beginY, beginX, beginY};

				// LUJ, 첫번째 라인이 어디까지 가는지 측정한다
				for(int x = block.left; x < mProperty.mSize; ++x, block.right = x - 1)
				{
					if(false == IsOptimizableTile(GetOptimizingTile(x, block.top)))
					{
						break;
					}

					// LUJ, 블록으로 확인된 줄은 재검사하지 않도록 표시한다
					SetOptimizingTile(x, block.top, TileBlock);
				}

				for(int y = block.bottom + 1; y < mProperty.mSize; ++y, block.bottom = y - 1)
				{
					int x = block.right + 1;

					while(block.left <= --x)
					{
						if(false == IsOptimizableTile(GetOptimizingTile(x, y)))
						{
							break;
						}
					}

					if(block.left <= x)
					{
						block.bottom = y - 1;
						break;
					}

					// LUJ, 블록으로 확인된 줄은 재검사하지 않도록 표시한다
					for(x = block.left; x <= block.right; ++x)
					{
						SetOptimizingTile(x, y, TileBlock);
					}
				}

				//PutToConsole();
				//cprintf("block: %d, %d ~ %d, %d (size: %d, %d)\n", block.left, block.top, block.right, block.bottom, block.right - block.left, block.bottom - block.top);
				//system("pause");

				mBlockContainer.push_back(block);
			}
		}

		_tcprintf_s(
			L"optimized: %d -> %d (%0.1f%%)\n",
			mTileContainer.size(), mBlockContainer.size(),
			float(mBlockContainer.size()) / float(mTileContainer.size()) * 100.0f);
	}


	Blueprint::TileProperty& Blueprint::GetTileProperty(int x, int y)
	{
		return mTileContainer[Tile(x, y)];
	}


	const POINT Blueprint::GetStartPoint() const
	{
		const POINT point = {
			mStartPosition.x * mTileSize,
			mStartPosition.y * mTileSize,
		};

		return point;
	}


	bool Blueprint::Load(const char* fileName)
	{
		TiXmlDocument document(fileName);

		if(false == document.LoadFile())
		{
			return false;
		}

		const TiXmlNode* const rootNode = document.FirstChild("blueprint");

		mTileContainer.clear();

		if(const TiXmlNode* const parentNode = rootNode->FirstChild("tile"))
		{
			for(const TiXmlNode* childNode = parentNode->FirstChild("data");
				nullptr != childNode;
				childNode = childNode->NextSibling())
			{
				const TiXmlElement* const element = childNode->ToElement();

				Tile tile;
				tile.first = atoi(element->Attribute("column"));
				tile.second = atoi(element->Attribute("row"));
				
				const TileType tileType = TileType(atoi(element->Attribute("type")));
				const Side sideType = Side(atoi(element->Attribute("side")));

				if(TileUpStairs == tileType)
				{
					mStartPosition.x = tile.first;
					mStartPosition.y = tile.second;
				}

				TileProperty& tileProperty = mTileContainer[tile];
				tileProperty.mType = tileType;
				tileProperty.mSide = sideType;
			}
		}

		mBlockContainer.clear();

		if(const TiXmlNode* const parentNode = rootNode->FirstChild("block"))
		{
			for(const TiXmlNode* childNode = parentNode->FirstChild("data");
				nullptr != childNode;
				childNode = childNode->NextSibling())
			{
				const TiXmlElement* const element = childNode->ToElement();

				RECT rect = {0};
				rect.left = atoi(element->Attribute("left"));
				rect.top = atoi(element->Attribute("top"));
				rect.right = atoi(element->Attribute("right"));
				rect.bottom = atoi(element->Attribute("bottom"));

				mBlockContainer.push_back(rect);
			}
		}

		if(true == mBlockContainer.empty())
		{
			BuildCollisionBlock();
		}

		return true;
	}


	void Blueprint::BuildStartEndPoint()
	{
		mStartPosition.x = mProperty.mSize / 2 + 1;
		mStartPosition.y = mProperty.mSize / 2 + 1;
	}


#ifndef _AFX_ALL_WARNINGS
	Blueprint::TileIterator Blueprint::GetTileIterator() const
	{
		return TileIterator(mTileContainer.begin(), mTileContainer.end());
	}


	Blueprint::BlockIterator Blueprint::GetBlockIterator() const
	{
		return BlockIterator(mBlockContainer.begin(), mBlockContainer.end());
	}
#endif
}
