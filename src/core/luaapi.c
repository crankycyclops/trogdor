
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/event.h"

void initLuaApi(lua_State *L) {

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, SUPPRESS_ACTION);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, ALLOW_ACTION);
   lua_setglobal(L, "ALLOW_ACTION");

   return;
}

