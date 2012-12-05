#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/event.h"
#include "include/data.h"
#include "include/luaapi.h"


/* initializes a Lua state with our C-to-Lua API */
void initLua(lua_State *L);

/* loads a Lua script from a file */
int loadScript(lua_State *L, char *filename);

/* primes Lua state so that all variables and functions get loaded */
void primeLua(lua_State *L);

/* This gets set to 1 if we had trouble parsing a script.  Using this will allow
   us to delay exiting due to the error until we've parsed and reported errors
   for all possibly bad scripts. */
int g_scriptError = 0;

/******************************************************************************/

void initLua(lua_State *L) {

   /* load standard library */
   luaL_openlibs(L);

   /* initialize Lua API */
   initLuaApi(L);

   return;
}

/******************************************************************************/

int loadScript(lua_State *L, char *filename) {

   int status;

   if (status = luaL_loadfile(L, filename)) {

      switch (status) {

         case LUA_ERRFILE:
            g_outputError("error: could not open %s\n", filename);
            g_scriptError = 1;
            return 0;

         case LUA_ERRSYNTAX:
            g_outputError("%s\n", lua_tostring(L, -1));
            g_scriptError = 1;
            return 0;

         case LUA_ERRMEM:
            PRINT_OUT_OF_MEMORY_ERROR;

         default:
            break;
      }
   }

   return 1;
}

/******************************************************************************/

/* TODO: this could probably be a macro instead... */
void primeLua(lua_State *L) {

   if (lua_pcall(L, 0, 0, 0)) {
      g_outputError("%s\n", lua_tostring(L, -1));
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

