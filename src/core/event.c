
#define EVENT_C

#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/state.h"
#include "include/player.h"
#include "include/event.h"
#include "include/player.h"
#include "include/data.h"


/* initialize the event handler (MUST be called before first use) */
void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
void registerEvent(char *name, int (*event)(Player *, void *));

/* call an event */
int event(Player *player, char *name, void *data);


/******************\
 * List of events *
\******************/

/* called before a room is displayed - takes as input the room (type Room *) */
static int beforeRoomDisplay(Player *player, void *data);

/* called after a room is displayed - takes as input the room (type Room *) */
static int afterRoomDisplay(Player *player, void *data);

/* called before an object is taken - takes as input the object (type Object *) */
static int beforeTakeObject(Player *player, void *data);

/* called after an object is taken - takes as input the object (type Object *) */
static int afterTakeObject(Player *player, void *data);

/* called before an object is dropped - takes as input the object (type Object *) */
static int beforeDropObject(Player *player, void *data);

/* called after an object is dropped - takes as input the object (type Object *) */
static int afterDropObject(Player *player, void *data);

/* called before an object is described - takes as input the object (type Object *) */
static int beforeDisplayObject(Player *player, void *data);

/* called after an object is described - takes as input the object (type Object *) */
static int afterDisplayObject(Player *player, void *data);

/* triggered when the user tries to take an object, but their inventory doesn't
   have enough free weight - data = object we're trying to pick up
   (type Object *) */
static int takeObjectTooHeavy(Player *player, void *data);

/* triggered when the user tries to take an untakeable object
   - data = object we're trying to pick up (type Object *) */
static int takeObjectUntakeable(Player *player, void *data);

/* triggered when the user tries to drop an undroppable object
   - data = object we're trying to pick up (type Object *) */
static int dropObjectUndroppable(Player *player, void *data);

/* called before setting a new location - data = room we're setting (type Room *) */
static int beforeSetLocation(Player *player, void *data);

/* called after setting a new location - data = room we're setting (type Room *) */
static int afterSetLocation(Player *player, void *data);

/* utilized by individual event handlers and returns one of:
   SUPPRESS_ACTION - don't allow the action immediately following the event
   ALLOW_ACTION    - allow the action following the event to continue */
static int callLuaEventHandler(lua_State *L, char *function, const char *player,
const char *name, int before);


/* maintains a mapping of event names to functions */
static GHashTable *events = NULL;


/******************************************************************************/

void initEvent() {

   events = g_hash_table_new(g_str_hash, g_str_equal);

   registerEvent("beforeRoomDisplay",     &beforeRoomDisplay);
   registerEvent("afterRoomDisplay",      &afterRoomDisplay);
   registerEvent("beforeSetLocation",     &beforeSetLocation);
   registerEvent("afterSetLocation",      &afterSetLocation);
   registerEvent("beforeTakeObject",      &beforeTakeObject);
   registerEvent("afterTakeObject",       &afterTakeObject);
   registerEvent("beforeDropObject",      &beforeDropObject);
   registerEvent("afterDropObject",       &afterDropObject);
   registerEvent("beforeDisplayObject",   &beforeDisplayObject);
   registerEvent("afterDisplayObject",    &afterDisplayObject);
   registerEvent("takeObjectTooHeavy",    &takeObjectTooHeavy);
   registerEvent("takeObjectUntakeable",  &takeObjectUntakeable);
   registerEvent("dropObjectUndroppable", &dropObjectUndroppable);
}

/******************************************************************************/

void registerEvent(char *name, int (*event)(Player *, void *)) {

   g_hash_table_insert(events, name, event);
}

/******************************************************************************/

int event(Player *player, char *name, void *data) {

   int (*event)(Player *, void *) = g_hash_table_lookup(events, name);

   if (NULL != event) {
      return event(player, data);
   }

   else {
      g_outputError("WARNING: event %s hasn't been registered.  This is a "
         "bug.\n", name);
      return ALLOW_ACTION;
   }
}

/******************************************************************************/

static int beforeRoomDisplay(Player *player, void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterRoomDisplay(Player *player, void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeSetLocation(Player *player, void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterSetLocation(Player *player, void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeTakeObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeTakeObject", dstrview(player->name),
      dstrview(object->name), 1);
}

/******************************************************************************/

static int afterTakeObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterTakeObject", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDropObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeDropObject", dstrview(player->name),
      dstrview(object->name), 1);
}

/******************************************************************************/

static int afterDropObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDropObject", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDisplayObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeDisplayObject", dstrview(player->name),
      dstrview(object->name), 1);
}

/******************************************************************************/

static int afterDisplayObject(Player *player, void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDisplayObject", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int takeObjectTooHeavy(Player *player, void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDropObject", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int takeObjectUntakeable(Player *player, void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "takeObjectUntakeable", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int dropObjectUndroppable(Player *player, void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "dropObjectUndroppable", dstrview(player->name),
      dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int callLuaEventHandler(lua_State *L, char *function, const char *player,
const char *thing, int before) {

   if (NULL != L) {

      lua_getglobal(L, function);

      /* only call function if it exists */
      if (lua_isfunction(L, lua_gettop(L))) {

         lua_pushstring(L, player);
         lua_pushstring(L, thing);

         if (lua_pcall(L, 2, 1, 0)) {
            g_outputError("Script error: %s\n", lua_tostring(L, -1));
         }

         else {

            /* only events triggered BEFORE actions can stop an action */
            if (before) {

               if (!lua_isboolean(L, -1)) {
                  g_outputError("Script error: %s must return a boolean!\n",
                     function);
               }

               /* script can return SUPPRESS_ACTION or ALLOW_ACTION */
               else {
                  int allowAction = lua_toboolean(L, -1);
                  lua_pop(L, 1);
                  return allowAction;
               }
            }
         }
      }
   }

   /* no script, so nothing to do ;) */
   return ALLOW_ACTION;
}

