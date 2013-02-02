#ifndef LUA_H
#define LUA_H


#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct {
   char       *function;
   lua_State  *L;
} LuaFunction;


#endif

