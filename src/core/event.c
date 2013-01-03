
#define EVENT_C

#include <stdlib.h>
#include <glib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/state.h"
#include "include/player.h"
#include "include/event.h"
#include "include/player.h"
#include "include/data.h"


/* initializes global event handlers */
void initGlobalEvents();

/* frees memory associated with the global event handlers */
void destroyGlobalEvents();

/* allocates memory for a new event handlers list */
GHashTable *createEventsList();

/* called by destroyEvents() to destroy a hash table of event-handler maps */
void destroyEventsList(GHashTable *table);

/* binds an event handler to a global event */
void addGlobalEventHandler(const char *event, const char *function);

/* binds an event handler to a player-specific event */
void addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
void addEntityEventHandler(const char *event, void *entity, enum EntityType type,
const char *function, lua_State *L);

/* removes a global event handler */
void removeGlobalEventHandler(const char *event, const char *function);

/* removes a player-specific event handler */
void removePlayerEventHandler(Player *player, const char *event,
const char *function);

/* removes an entity-specific event handler */
void removeEntityEventHandler(enum EntityType entityType, void *entity,
const char *event, const char *function);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
int event(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, ...);

/******************************************************************************/

/* creates an event handler object */
static EventHandler *createEventHandler();

/* frees an event handler object */
static void destroyEventHandler(EventHandler *handler);

/* does the actual work of registering an event handler */
static void addEventHandler(GHashTable *eventsTable, const char *event,
const char *function, lua_State *L);

/* does the actual work of removing an event handler */
static void removeEventHandler(GHashTable *table, const char *event,
const char *function);

/* called by event: execute global handlers for an event */
static int globalEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args);

/* called by event: execute player-specific handlers for an event */
static int playerEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args);

/* called by event: execute entity-specific handlers for an event */
static int entityEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args);

/* called by global, player and entityEvent: executes event handlers associated
   with an event.  entityName is the name of the entity or player that owns the
   event handler (or NULL for global events.) */
static int executeEvent(GList *handlers, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args);

/* passes an EventArgument to a script function */
static void eventPassArgument(lua_State *L, EventArgument arg);

/******************************************************************************/

/* lua state containing functions called by global events */
lua_State *globalL = NULL;

/******************************************************************************/

static GHashTable *globalEvents;

/******************************************************************************/

void initGlobalEvents() {

   globalEvents   = g_hash_table_new(g_str_hash, g_str_equal);
   return;
}

/******************************************************************************/

void destroyGlobalEvents() {

   destroyEventsList(globalEvents);

   if (NULL != globalL) {
      lua_close(globalL);
   }

   return;
}

/******************************************************************************/

GHashTable *createEventsList() {

   return g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

void destroyEventsList(GHashTable *table) {

   GList *events = g_hash_table_get_values(table);
   GList *next = events;

   while (next != NULL) {
      g_list_free(next->data);
      next = next->next;
   }

   g_list_free(events);
   g_hash_table_destroy(table);
   return;
}

/******************************************************************************/

static EventHandler *createEventHandler() {

   EventHandler *newHandler = malloc(sizeof(EventHandler));

   if (NULL == newHandler) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   newHandler->function = NULL;
   newHandler->L = NULL;

   return newHandler;
}

/******************************************************************************/

static void destroyEventHandler(EventHandler *handler) {

   free(handler);
}

/******************************************************************************/

void addGlobalEventHandler(const char *event, const char *function) {

   addEventHandler(globalEvents, event, function, globalL);
}

/******************************************************************************/

void addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L) {

   addEventHandler(player->events, event, function, L);
}

/******************************************************************************/

void addEntityEventHandler(const char *event, void *entity, enum EntityType type,
const char *function, lua_State *L) {

   switch (type) {

      case entity_room:
         addEventHandler(((Room *)entity)->events, event, function, L);
         break;

      case entity_object:
         addEventHandler(((Object *)entity)->events, event, function, L);
         break;

      case entity_creature:
         addEventHandler(((Creature *)entity)->events, event, function, L);
         break;

      case entity_player:
         g_outputError("ERROR: Use addPlayerEventHander to attach an event "
            "to a player. This is a bug.");
         break;

      default:
         g_outputError("ERROR: Invalid entity type passed to "
            "addEntityEventHandler. This is a bug.");
         break;
   }

   return;
}

/******************************************************************************/

static void addEventHandler(GHashTable *eventsTable, const char *event,
const char *function, lua_State *L) {

   GList        *handlerList;
   EventHandler *newHandler;

   newHandler = createEventHandler();
   newHandler->function = function;
   newHandler->L = L;

   /* events will be executed in LIFO order */
   handlerList = g_hash_table_lookup(eventsTable, (char *)event);
   handlerList = g_list_prepend(handlerList, newHandler);
   g_hash_table_insert(eventsTable, (char *)event, handlerList);

   return;
}

/******************************************************************************/

void removeGlobalEventHandler(const char *event, const char *function) {

   removeEventHandler(globalEvents, event, function);
   return;
}

/******************************************************************************/

void removePlayerEventHandler(Player *player, const char *event,
const char *function) {

   removeEventHandler(player->events, event, function);
}

/******************************************************************************/

void removeEntityEventHandler(enum EntityType entityType, void *entity,
const char *event, const char *function) {

   GHashTable *events;

   switch (entityType) {

      case entity_room:
         events = ((Room *)entity)->events;
         break;

      case entity_object:
         events = ((Object *)entity)->events;
         break;

      case entity_creature:
         events = ((Creature *)entity)->events;
         break;

      case entity_player:
         g_outputError("ERROR: Use removePlayerEventHander to remove an event "
            "handler from a player. This is a bug.");
         break;

      default:
         g_outputError("ERROR: Invalid entity type passed to "
            "removeEntityEventHandler. This is a bug.");
         break;
   }

   removeEventHandler(events, event, function);
   return;
}

/******************************************************************************/

static void removeEventHandler(GHashTable *table, const char *event,
const char *function) {

   GList *eventList = g_hash_table_lookup(table, event);

   if (NULL != eventList) {

      GList *next = eventList;

      while (next != NULL) {
         if (0 == strcmp(((EventHandler *)next->data)->function, function)) {
            eventList = g_list_remove_link(eventList, next);
            g_hash_table_insert(table, (char *)event, eventList);
            return;
         }
      }
   }

   return;
}

/******************************************************************************/

int event(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, ...) {

   /* whether or not to continue executing event handlers AND whether or not
      to allow the action that triggered the event to continue */
   int globalStatus;
   int playerStatus;
   int entityStatus;

   va_list args;
   va_start(args, numArgs);

   /* execute global event handlers */
   globalStatus = globalEvent(name, player, entity, entityType, numArgs, args);
   if (!(CONTINUE_HANDLERS & globalStatus)) {
      return CONTINUE_ACTION & globalStatus;
   }

   /* execute player-specific event handlers */
   playerStatus = playerEvent(name, player, entity, entityType, numArgs, args);
   if (!(CONTINUE_HANDLERS & playerStatus)) {
      return CONTINUE_ACTION & globalStatus && CONTINUE_ACTION & playerStatus;
   }

   /* finally, execute entity-specific event handlers */
   entityStatus = entityEvent(name, player, entity, entityType, numArgs, args);

   return CONTINUE_ACTION & globalStatus && CONTINUE_ACTION & playerStatus &&
      CONTINUE_ACTION & entityStatus;
}

/******************************************************************************/

static int globalEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args) {

   GList *handlers = g_hash_table_lookup(globalEvents, name);
   return executeEvent(handlers, player, entity, entityType, numArgs, args);
}

/******************************************************************************/

static int playerEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args) {

   GList *handlers = g_hash_table_lookup(player->events, name);
   return executeEvent(handlers, player, entity, entityType, numArgs, args);
}

/******************************************************************************/

static int entityEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args) {

   GList     *handlers;

   switch (entityType) {

      case entity_room:
         handlers = g_hash_table_lookup(((Room *)entity)->events, name);
         break;

      case entity_object:
         handlers = g_hash_table_lookup(((Object *)entity)->events, name);
         break;

      case entity_creature:
         handlers = g_hash_table_lookup(((Creature *)entity)->events, name);
         break;

      case entity_player:
         g_outputError("ERROR: Use playerEvent to execute a player-specific "
            "event. This is a bug.");
         break;

      default:
         g_outputError("ERROR: Invalid entity type passed to entityEvent. "
            "This is a bug.");
         break;
   }

   return executeEvent(handlers, player, entity, entityType, numArgs, args);
}

/******************************************************************************/

static int executeEvent(GList *handlers, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args) {

   GList *nextHandler = handlers;
   int status = CONTINUE_HANDLERS | CONTINUE_ACTION;

   while (NULL != nextHandler && CONTINUE_HANDLERS & status) {

      int i;
      EventHandler *handler = (EventHandler *)nextHandler->data;

      if (NULL != handler->L) {

         lua_getglobal(handler->L, handler->function);

         /* only call function if it exists */
         if (lua_isfunction(handler->L, lua_gettop(handler->L))) {

            char *entityName = "";

            if (NULL != entity) {

               switch (entityType) {

                  case entity_room:
                     entityName = (char *)dstrview(((Room *)entity)->name);
                     break;

                  case entity_object:
                     entityName = (char *)dstrview(((Object *)entity)->name);
                     break;

                  case entity_creature:
                     entityName = (char *)dstrview(((Creature *)entity)->name);
                     break;

                  case entity_player:
                  default:
                     g_outputError("entity passed to executeEvent must be of "
                        "type room, object or creature.  This is a bug.");
                     break;
               }
            }

            lua_pushstring(handler->L, NULL == player ? "" :
               (char *)dstrview(player->name));
            lua_pushstring(handler->L, entityName);

            /* pass any extra arguments that might've been specified */
            for (i = 0; i < numArgs; i++) {
               EventArgument arg = va_arg(args, EventArgument);
               eventPassArgument(handler->L, arg);
            }

            /* function should return two arguments, CONTINUE_HANDLERS and
               CONTINUE_ACTION */
            if (lua_pcall(handler->L, numArgs + 2, 2, 0)) {
               g_outputError("Script error: %s\n", lua_tostring(handler->L, -1));
            }

            else {

               if (!lua_isboolean(handler->L, -1) ||
               !lua_isboolean(handler->L, -2)) {
                  g_outputError("Script error: %s must return a boolean!\n",
                     handler->function);
               }

               status = lua_toboolean(handler->L, -2) ?
                  status | CONTINUE_HANDLERS : status & ~CONTINUE_HANDLERS;
               status = lua_toboolean(handler->L, -1) ?
                  status | CONTINUE_ACTION : status & ~CONTINUE_ACTION;
            }
         }
      }

      nextHandler = g_list_next(nextHandler);
   }

   return status;
}

/******************************************************************************/

static void eventPassArgument(lua_State *L, EventArgument arg) {

   switch (arg.type) {

      scriptval_number:
         lua_pushnumber(L, arg.value.number);
         break;

      scriptval_string:
         lua_pushstring(L, arg.value.string);
         break;

      scriptval_player:
         lua_pushstring(L, arg.value.player->name);
         break;

      scriptval_room:
         lua_pushstring(L, arg.value.room->name);
         break;

      scriptval_object:
         lua_pushstring(L, arg.value.object->name);
         break;

      scriptval_creature:
         lua_pushstring(L, arg.value.creature->name);
         break;

      default:
         g_outputString("Invalid type for EventArgument.  This is a bug.\n");
         break;
   }

   return;
}

