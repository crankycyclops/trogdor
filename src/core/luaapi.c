
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/event.h"
#include "include/room.h"
#include "include/object.h"
#include "include/state.h"
#include "include/data.h"


/* allows scripting engine to output strings */
static int l_outputString(lua_State *L);

/* allows scrcipting engine to output error messages */
static int l_outputError(lua_State *L);

/* change the player's location */
static int l_setLocation(lua_State *L);

/******************************************************************************/

void initLuaApi(lua_State *L) {

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, SUPPRESS_ACTION);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, ALLOW_ACTION);
   lua_setglobal(L, "ALLOW_ACTION");

   /* register C-Lua API functions */
   lua_register(L, "outputString", &l_outputString);
   lua_register(L, "outputError", &l_outputError);
   lua_register(L, "setLocation", &l_setLocation);

   return;
}

/******************************************************************************/

static int l_outputString(lua_State *L) {

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

static int l_outputError(lua_State *L) {

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

/******************************************************************************/

static int l_setLocation(lua_State *L) {

   Player     *player;
   Room       *newLocation;
   const char *playerName;
   const char *roomName;

   int n = lua_gettop(L);

   /* script must provide player name and room name as arguments */
   if (n < 2) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "must pass player and room names to setLocation");
      return 2;
   }

   playerName = lua_tostring(L, 1);
   roomName = lua_tostring(L, 2);

   player = g_hash_table_lookup(g_players, playerName);
   if (NULL == player) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "player doesn't exist");
      return 2;
   }

   newLocation = g_hash_table_lookup(rooms, roomName);
   if (NULL == newLocation) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "room doesn't exist");
      return 2;
   }

   setLocation(player, newLocation, 0);
   lua_pushboolean(L, 1);
   return 1;
}

