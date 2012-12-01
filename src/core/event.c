
#define EVENT_C

#include <glib.h>

#include "include/object.h"
#include "include/room.h"
#include "include/state.h"
#include "include/event.h"


/* initialize the event handler (MUST be called before first use) */
void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
void registerEvent(char *name, int (*event)(void *));

/* call an event */
int event(char *name, void *data);


/******************\
 * List of events *
\******************/

/* called before a room is displayed - takes as input the room (type Room *) */
static int beforeRoomDisplay(void *data);

/* called after a room is displayed - takes as input the room (type Room *) */
static int afterRoomDisplay(void *data);

/* called before an object is taken - takes as input the object (type Object *) */
static int beforeTakeObject(void *data);

/* called after an object is taken - takes as input the object (type Object *) */
static int afterTakeObject(void *data);

/* called before an object is dropped - takes as input the object (type Object *) */
static int beforeDropObject(void *data);

/* called after an object is dropped - takes as input the object (type Object *) */
static int afterDropObject(void *data);

/* called before an object is described - takes as input the object (type Object *) */
static int beforeDisplayObject(void *data);

/* called after an object is described - takes as input the object (type Object *) */
static int afterDisplayObject(void *data);

/* triggered when the user tries to take an object, but their inventory doesn't
   have enough free weight - data = object we're trying to pick up
   (type Object *) */
static int takeObjectTooHeavy(void *data);

/* called before setting a new location - data = room we're setting (type Room *) */
static int beforeSetLocation(void *data);

/* called after setting a new location - data = room we're setting (type Room *) */
static int afterSetLocation(void *data);

/* utilized by individual event handlers and returns one of:
   SUPPRESS_ACTION - don't allow the action immediately following the event
   ALLOW_ACTION    - allow the action following the event to continue */
static int callLuaEventHandler(lua_State *L, char *function, const char *name,
int before);


/* maintains a mapping of event names to functions */
static GHashTable *events = NULL;


/******************************************************************************/

void initEvent() {

   events = g_hash_table_new(g_str_hash, g_str_equal);

   registerEvent("beforeRoomDisplay",   &beforeRoomDisplay);
   registerEvent("afterRoomDisplay",    &afterRoomDisplay);
   registerEvent("beforeSetLocation",   &beforeSetLocation);
   registerEvent("afterSetLocation",    &afterSetLocation);
   registerEvent("beforeTakeObject",    &beforeTakeObject);
   registerEvent("afterTakeObject",     &afterTakeObject);
   registerEvent("beforeDropObject",    &beforeDropObject);
   registerEvent("afterDropObject",     &afterDropObject);
   registerEvent("beforeDisplayObject", &beforeDisplayObject);
   registerEvent("afterDisplayObject",  &afterDisplayObject);
   registerEvent("takeObjectTooHeavy",  &takeObjectTooHeavy);
}

/******************************************************************************/

void registerEvent(char *name, int (*event)(void *)) {

   g_hash_table_insert(events, name, event);
}

/******************************************************************************/

int event(char *name, void *data) {

   int (*event)(void *) = g_hash_table_lookup(events, name);

   if (NULL != event) {
      return event(data);
   }

   else {
      fprintf(stderr, "WARNING: event %s hasn't been registered.  This is a "
         "bug.\n", name);
      return ALLOW_ACTION;
   }
}

/******************************************************************************/

static int beforeRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeSetLocation(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterSetLocation(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeTakeObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeTakeObject", dstrview(object->name), 1);
}

/******************************************************************************/

static int afterTakeObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterTakeObject", dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDropObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeDropObject", dstrview(object->name), 1);
}

/******************************************************************************/

static int afterDropObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDropObject", dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDisplayObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   return callLuaEventHandler(L, "beforeDisplayObject", dstrview(object->name), 1);
}

/******************************************************************************/

static int afterDisplayObject(void *data) {

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDisplayObject", dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int takeObjectTooHeavy(void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.

   Object *object = data;
   lua_State *L = object->lua;

   callLuaEventHandler(L, "afterDropObject", dstrview(object->name), 0);
   return ALLOW_ACTION;
}

/******************************************************************************/

static int callLuaEventHandler(lua_State *L, char *function, const char *name,
int before) {

   if (NULL != L) {

      lua_getglobal(L, function);

      /* only call function if it exists */
      if (lua_isfunction(L, lua_gettop(L))) {

         lua_pushstring(L, name);

         if (lua_pcall(L, 1, 1, 0)) {
            fprintf(stderr, "Script error: %s\n", lua_tostring(L, -1));
         }

         else {

            /* only events triggered BEFORE actions can stop an action */
            if (before) {

               if (!lua_isboolean(L, -1)) {
                  fprintf(stderr, "Script error: %s must return a boolean!\n",
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

