#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/event.h"


/* initializes a Lua state with our C-to-Lua API */
void initLua(lua_State *L);

/* loads a Lua script from a file */
void loadScript(lua_State *L, char *filename);

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

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, SUPPRESS_ACTION);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, ALLOW_ACTION);
   lua_setglobal(L, "ALLOW_ACTION");

   return;
}

/******************************************************************************/

void loadScript(lua_State *L, char *filename) {

   int status;

   if (status = luaL_loadfile(L, filename)) {

      switch (status) {

         case LUA_ERRFILE:
            fprintf(stderr, "error: could not open %s\n", filename);
            g_scriptError = 1;
            break;

         case LUA_ERRSYNTAX:
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            g_scriptError = 1;
            break;

         case LUA_ERRMEM:
            PRINT_OUT_OF_MEMORY_ERROR;

         default:
            break;
      }
   }
}

/******************************************************************************/

/* TODO: this could probably be a macro instead... */
void primeLua(lua_State *L) {

   if (lua_pcall(L, 0, 0, 0)) {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/
