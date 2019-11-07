#pragma once


namespace Dungeon
{
	struct TileProperty;

	class Parser
	{
	public:		
		Parser();
		virtual ~Parser();

		void LoadProperty(const char* fileName, TileProperty&) const;


	private:
		typedef void (*ParsingFunction)(const TiXmlNode&, TileProperty&);
		static void ParseGlobal(const TiXmlNode&, TileProperty&);
		static void ParseTileWall(const TiXmlNode&, TileProperty&);
		static void ParseTileDoor(const TiXmlNode&, TileProperty&);
	};
}
