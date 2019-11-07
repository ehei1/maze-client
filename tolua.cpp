#include <StdAfx.h>

/*
** Lua binding: toLua
** Generated automatically by tolua++-1.0.92 on 05/04/11 17:22:24.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
int tolua_toLua_open (lua_State* tolua_S);

#include "FineappleLua.h"
#include "Application.h"
#include "ConnectMenuState.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Application");
 tolua_usertype(tolua_S,"CShinIOCP");
 tolua_usertype(tolua_S,"Fineapple::Lua");
}

/* method: GetSingleton of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_GetSingleton00
static int tolua_toLua_Fineapple_Lua_GetSingleton00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Fineapple::Lua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Fineapple::Lua& tolua_ret = (Fineapple::Lua&)  Fineapple::Lua::GetSingleton();
 tolua_pushusertype(tolua_S,(void*)&tolua_ret,"Fineapple::Lua");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSingleton'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SaveDungeon of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_SaveDungeon00
static int tolua_toLua_Fineapple_Lua_SaveDungeon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SaveDungeon'",NULL);
#endif
 {
  self->SaveDungeon();
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SaveDungeon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LoadDungeon of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_LoadDungeon00
static int tolua_toLua_Fineapple_Lua_LoadDungeon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LoadDungeon'",NULL);
#endif
 {
  self->LoadDungeon();
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LoadDungeon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RandomizeDungeon of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_RandomizeDungeon00
static int tolua_toLua_Fineapple_Lua_RandomizeDungeon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
  int row = ((int)  tolua_tonumber(tolua_S,2,0));
  int column = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RandomizeDungeon'",NULL);
#endif
 {
  self->RandomizeDungeon(row,column);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RandomizeDungeon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetFog of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_SetFog00
static int tolua_toLua_Fineapple_Lua_SetFog00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
  int fogMode = ((int)  tolua_tonumber(tolua_S,2,0));
  float red = ((float)  tolua_tonumber(tolua_S,3,0));
  float green = ((float)  tolua_tonumber(tolua_S,4,0));
  float blue = ((float)  tolua_tonumber(tolua_S,5,0));
  float density = ((float)  tolua_tonumber(tolua_S,6,0));
  float linearStart = ((float)  tolua_tonumber(tolua_S,7,0));
  float linearEnd = ((float)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetFog'",NULL);
#endif
 {
  self->SetFog(fogMode,red,green,blue,density,linearStart,linearEnd);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetFog'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PutToConsole of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_PutToConsole00
static int tolua_toLua_Fineapple_Lua_PutToConsole00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
  const char* text = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PutToConsole'",NULL);
#endif
 {
  self->PutToConsole(text);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PutToConsole'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LoadTileProperty of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_LoadTileProperty00
static int tolua_toLua_Fineapple_Lua_LoadTileProperty00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Fineapple::Lua",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Fineapple::Lua* self = (const Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
  const char* filePath = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LoadTileProperty'",NULL);
#endif
 {
  self->LoadTileProperty(filePath);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LoadTileProperty'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ConnectServer of class  Fineapple::Lua */
#ifndef TOLUA_DISABLE_tolua_toLua_Fineapple_Lua_ConnectServer00
static int tolua_toLua_Fineapple_Lua_ConnectServer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Fineapple::Lua",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isstring(tolua_S,3,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Fineapple::Lua* self = (Fineapple::Lua*)  tolua_tousertype(tolua_S,1,0);
  const char* ip = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* port = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ConnectServer'",NULL);
#endif
 {
  bool tolua_ret = (bool)  self->ConnectServer(ip,port);
 tolua_pushboolean(tolua_S,(bool)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConnectServer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSingleton of class  Application */
#ifndef TOLUA_DISABLE_tolua_toLua_Application_GetSingleton00
static int tolua_toLua_Application_GetSingleton00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertable(tolua_S,1,"Application",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
 {
  Application& tolua_ret = (Application&)  Application::GetSingleton();
 tolua_pushusertype(tolua_S,(void*)&tolua_ret,"Application");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSingleton'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Quit of class  Application */
#ifndef TOLUA_DISABLE_tolua_toLua_Application_Quit00
static int tolua_toLua_Application_Quit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Application",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Application* self = (Application*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Quit'",NULL);
#endif
 {
  bool tolua_ret = (bool)  self->Quit();
 tolua_pushboolean(tolua_S,(bool)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Quit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SelectState of class  Application */
#ifndef TOLUA_DISABLE_tolua_toLua_Application_SelectState00
static int tolua_toLua_Application_SelectState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Application",0,&tolua_err) ||
 !tolua_isstring(tolua_S,2,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Application* self = (Application*)  tolua_tousertype(tolua_S,1,0);
  const char* tolua_var_1 = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SelectState'",NULL);
#endif
 {
  bool tolua_ret = (bool)  self->SelectState(tolua_var_1);
 tolua_pushboolean(tolua_S,(bool)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SelectState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNetwork of class  Application */
#ifndef TOLUA_DISABLE_tolua_toLua_Application_GetNetwork00
static int tolua_toLua_Application_GetNetwork00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Application",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Application* self = (Application*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNetwork'",NULL);
#endif
 {
  CShinIOCP& tolua_ret = (CShinIOCP&)  self->GetNetwork();
 tolua_pushusertype(tolua_S,(void*)&tolua_ret,"CShinIOCP");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNetwork'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
int tolua_toLua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_module(tolua_S,"Fineapple",0);
 tolua_beginmodule(tolua_S,"Fineapple");
  tolua_cclass(tolua_S,"Lua","Fineapple::Lua","",NULL);
  tolua_beginmodule(tolua_S,"Lua");
   tolua_function(tolua_S,"GetSingleton",tolua_toLua_Fineapple_Lua_GetSingleton00);
   tolua_function(tolua_S,"SaveDungeon",tolua_toLua_Fineapple_Lua_SaveDungeon00);
   tolua_function(tolua_S,"LoadDungeon",tolua_toLua_Fineapple_Lua_LoadDungeon00);
   tolua_function(tolua_S,"RandomizeDungeon",tolua_toLua_Fineapple_Lua_RandomizeDungeon00);
   tolua_function(tolua_S,"SetFog",tolua_toLua_Fineapple_Lua_SetFog00);
   tolua_function(tolua_S,"PutToConsole",tolua_toLua_Fineapple_Lua_PutToConsole00);
   tolua_function(tolua_S,"LoadTileProperty",tolua_toLua_Fineapple_Lua_LoadTileProperty00);
   tolua_function(tolua_S,"ConnectServer",tolua_toLua_Fineapple_Lua_ConnectServer00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Application","Application","",NULL);
 tolua_beginmodule(tolua_S,"Application");
  tolua_function(tolua_S,"GetSingleton",tolua_toLua_Application_GetSingleton00);
  tolua_function(tolua_S,"Quit",tolua_toLua_Application_Quit00);
  tolua_function(tolua_S,"SelectState",tolua_toLua_Application_SelectState00);
  tolua_function(tolua_S,"GetNetwork",tolua_toLua_Application_GetNetwork00);
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 int luaopen_toLua (lua_State* tolua_S) {
 return tolua_toLua_open(tolua_S);
};
#endif

