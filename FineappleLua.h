#pragma once


/*
LUJ, You may write exporting function for lua in this file.
	tolua.pkg has helpful text
*/
namespace Fineapple // tolua_export
{ // tolua_export
	class Lua // tolua_export
	{ // tolua_export
	public:
		// tolua_begin
		static Lua& GetSingleton();

		void SaveDungeon() const;
		void LoadDungeon() const;
		void RandomizeDungeon(int row, int column) const;

		void SetFog(
			int fogMode,
			float red,
			float green,
			float blue,
			float density,
			float linearStart,
			float linearEnd) const;

		void PutToConsole(const char* text) const;

		void LoadTileProperty(const char* filePath) const;

		bool ConnectServer( const char* ip, const char* port );

		// tolua_end
	private:
		Lua();
		virtual ~Lua();
	}; // tolua_export
} // tolua_export
