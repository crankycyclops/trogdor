
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


/* outputs a string */
static int l_outputString(lua_State *L);

/* outputs an error message */
static int l_outputError(lua_State *L);

/* change the player's location */
static int l_setLocation(lua_State *L);

/* retrieves a room struct in the form of a Lua table */
static int l_getRoom(lua_State *L);

/* creates a room */
static int l_createRoom(lua_State *L);

/******************************************************************************/

void initLuaApi(lua_State *L) {

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, SUPPRESS_ACTION);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, ALLOW_ACTION);
   lua_setglobal(L, "ALLOW_ACTION");

   /* register C-Lua API functions */
   lua_register(L, "outputString", &l_outputString);
   lua_register(L, "outputError",  &l_outputError);
   lua_register(L, "setLocation",  &l_setLocation);
   lua_register(L, "getRoom",      &l_getRoom);
   lua_register(L, "createRoom",   &l_createRoom);

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

   newLocation = g_hash_table_lookup(g_rooms, roomName);
   if (NULL == newLocation) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "room doesn't exist");
      return 2;
   }

   setLocation(player, newLocation, 0);
   lua_pushboolean(L, 1);
   return 1;
}

/******************************************************************************/

static int l_getRoom(lua_State *L) {

   Room *room;
   const char *roomName;
   int n = lua_gettop(L);

   /* script must provide room name */
   if (n < 1) {
      lua_pushboolean(L, 0);
      return 1;
   }

   roomName = lua_tostring(L, 1);
   room = g_hash_table_lookup(g_rooms, roomName);

   /* specified room doesn't exist */
   if (NULL == room) {
      lua_pushboolean(L, 0);
      return 1;
   }

   lua_newtable(L);

   lua_pushstring(L, "title");
   lua_pushstring(L, dstrview(room->title));
   lua_settable(L, -3);

   lua_pushstring(L, "description");
   lua_pushstring(L, dstrview(room->description));
   lua_settable(L, -3);

   lua_pushstring(L, "north");
   if (NULL == room->north) {
      lua_pushboolean(L, 0);
   } else {
      lua_pushstring(L, dstrview(room->north->name));
   }

   lua_settable(L, -3);

   lua_pushstring(L, "south");
   if (NULL == room->south) {
      lua_pushboolean(L, 0);
   } else {
      lua_pushstring(L, dstrview(room->south->name));
   }

   lua_settable(L, -3);

   lua_pushstring(L, "east");
   if (NULL == room->east) {
      lua_pushboolean(L, 0);
   } else {
      lua_pushstring(L, dstrview(room->east->name));
   }

   lua_settable(L, -3);

   lua_pushstring(L, "west");
   if (NULL == room->west) {
      lua_pushboolean(L, 0);
   } else {
      lua_pushstring(L, dstrview(room->west->name));
   }

   lua_settable(L, -3);
   return 1;
}

/******************************************************************************/

static int l_createRoom(lua_State *L) {

   Room *newroom;

   char *name        = NULL;
   char *title       = NULL;
   char *description = NULL;

   Room *north = NULL;
   Room *south = NULL;
   Room *east  = NULL;
   Room *west  = NULL;

   int n = lua_gettop(L);

   /* script must provide a name and a table of room properties */
   if (n < 2) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "Must provide room name and table of its properties");
      return 2;
   }

   name = (char *)lua_tostring(L, 1);

   /* make sure room doesn't already exist */
   if (g_hash_table_lookup(g_rooms, name)) {
      lua_pushboolean(L, 0);
      lua_pushstring(L, "Room already exists!");
      return 2;
   }

   lua_pushnil(L);  /* first key (popped by lua_next) */

   while (lua_next(L, 2) != 0) {

      const char *key = lua_tostring(L, -2);
      const char *value = lua_tostring(L, -1);

      if (0 == strcmp("title", key)) {
         title = (char *)value;
      }

      else if (0 == strcmp("description", key)) {
         description = (char *)value;
      }

      else if (0 == strcmp("north", key)) {
         north = g_hash_table_lookup(g_rooms, value);
         if (NULL == north) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Room to the north doesn't exist!");
            return 2;
         }
      }

      else if (0 == strcmp("south", key)) {
         south = g_hash_table_lookup(g_rooms, value);
         if (NULL == south) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Room to the south doesn't exist!");
            return 2;
         }
      }

      else if (0 == strcmp("east", key)) {
         east = g_hash_table_lookup(g_rooms, value);
         if (NULL == east) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Room to the east doesn't exist!");
            return 2;
         }
      }

      else if (0 == strcmp("west", key)) {
         west = g_hash_table_lookup(g_rooms, value);
         if (NULL == west) {
            lua_pushboolean(L, 0);
            lua_pushstring(L, "Room to the west doesn't exist!");
            return 2;
         }
      }

      else {
         lua_pop(L, 1);
         lua_pushboolean(L, 0);
         lua_pushstring(L, "Invalid property!");
         return 2;
      }

      /* removes 'value'; keeps 'key' for next iteration */
      lua_pop(L, 1);
   }

   if (NULL == title) {
         lua_pushboolean(L, 0);
         lua_pushstring(L, "title property is required");
         return 2;
   }

   else if (NULL == description) {
         lua_pushboolean(L, 0);
         lua_pushstring(L, "description property is required");
         return 2;
   }

   newroom = createRoom();

   cstrtodstr(newroom->name, name);
   cstrtodstr(newroom->title, title);
   cstrtodstr(newroom->description, description);   

   newroom->north = north;
   newroom->south = south;
   newroom->east = east;
   newroom->west = west;

   g_hash_table_insert(g_rooms, name, newroom);
   lua_pushboolean(L, 1);
   return 1;
}

