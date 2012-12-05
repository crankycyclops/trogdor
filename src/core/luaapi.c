
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/event.h"
#include "include/data.h"


/* allows scripting engine to output strings */
static int outputString(lua_State *L);

/* allows scrcipting engine to output error messages */
static int outputError(lua_State *L);

/******************************************************************************/

void initLuaApi(lua_State *L) {

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, SUPPRESS_ACTION);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, ALLOW_ACTION);
   lua_setglobal(L, "ALLOW_ACTION");

   /* register C-Lua API functions */
   lua_register(L, "outputString", &outputString);
   lua_register(L, "outputError", &outputError);

   return;
}

/******************************************************************************/

static int outputString(lua_State *L) {

   const char *string;
   int n = lua_gettop(L);

   /* not passing in any arguments results in outputtinga new line */
   if (0 == n) {
      g_outputString("\n");
      return 0;
   }

   string = lua_tostring(L, 1);
   g_outputString(string);
   return 0;
}

/******************************************************************************/

static int outputError(lua_State *L) {

   const char *string;
   int n = lua_gettop(L);

   /* not passing in any arguments results in outputting a new line */
   if (0 == n) {
      g_outputError("\n");
      return 0;
   }

   string = lua_tostring(L, 1);
   g_outputError(string);
   return 0;
}

