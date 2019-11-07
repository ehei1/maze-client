#include <StdAfx.h>
#include "FineappleLua.h"
#include "Application.h"
#include "SinglePlayState.h"
#include "DungeonManager.h"


namespace Fineapple
{
	Lua::Lua()
	{

	}


	Lua::~Lua()
	{

	}


	Lua& Lua::GetSingleton()
	{
		static Lua instance;

		return instance;
	}


	void Lua::SaveDungeon() const
	{
		auto singlePlayState = dynamic_cast< State::SinglePlayState* >(Application::GetSingleton().GetState("singlePlay"));

		if(nullptr != singlePlayState)
		{
			singlePlayState->GetDungeonManager().Save("media/dungeon/Blueprint/Blueprint.xml");
		}
	}


	void Lua::LoadDungeon() const
	{
		auto singlePlayState = dynamic_cast< State::SinglePlayState* >(Application::GetSingleton().GetState("singlePlay"));

		if(nullptr != singlePlayState)
		{
			singlePlayState->LoadDungeon("media/dungeon/Blueprint/Blueprint.xml");
			singlePlayState->SyncPlayer();
		}
	}


	void Lua::SetFog(
		int fogMode,
		float red,
		float green,
		float blue,
		float density,
		float linearStart,
		float linearEnd) const
	{
		Application::GetSingleton().GetSceneManager().setFog(
			Ogre::FogMode(fogMode),
			Ogre::ColourValue(red, green, blue),
			density,
			linearStart,
			linearEnd);
	}


	void Lua::PutToConsole(const char* text) const
	{
		printf(text);
		printf("\n");
	}


	void Lua::RandomizeDungeon(int row, int column) const
	{
		auto singlePlayState = dynamic_cast< State::SinglePlayState* >(Application::GetSingleton().GetState("singlePlay"));

		if(nullptr != singlePlayState)
		{
			singlePlayState->RandomizeDungeon(row, column);
			singlePlayState->SyncPlayer();
		}
	}


	void Lua::LoadTileProperty(const char* filePath) const
	{
		auto singlePlayState = dynamic_cast< State::SinglePlayState* >(Application::GetSingleton().GetState("singlePlay"));

		if(nullptr != singlePlayState)
		{
			singlePlayState->GetDungeonManager().LoadTileProperty(filePath);
			singlePlayState->RebuildDungeon();
			singlePlayState->SyncPlayer();
		}
	}

	bool Lua::ConnectServer( const char* ip, const char* portText )
	{
		if( Application::GetSingleton().GetNetwork().IsBegined() == false )
		{
			Application::GetSingleton().GetNetwork().Begin();
		}
		else if( Application::GetSingleton().GetNetwork().IsServer( Application::GetSingleton().GetServerConnectIdx() ) == true )
		{
			return false;
		}

		u_short port = _ttoi( MultiToWide(portText) );
		unsigned long connectionIdx = 0;
		if( Application::GetSingleton().GetNetwork().ConnectServer( MultiToWide(ip), port, connectionIdx ) == false )
		{
			return false;
		}

		Application::GetSingleton().SetServerConnectIdx( connectionIdx );
		Application::GetSingleton().SelectState( "networkPlay" );
		return true;
	}
}