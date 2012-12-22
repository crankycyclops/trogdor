
#define EVENT_C

#include <stdlib.h>
#include <glib.h>

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
unsigned long addGlobalEventHandler(const char *event, const char *function,
lua_State *L);

/* binds an event handler to a player-specific event */
unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, enum EntityType type, const char *function, lua_State *L);

/* removes a global event handler */
void removeGlobalEventHandler(const char *event, unsigned long id);

/* removes a player-specific event handler */
void removePlayerEventHandler(Player *player, const char *event,
unsigned long id);

/* removes an entity-specific event handler */
void removeEntityEventHandler(enum EntityType type, void *entity,
const char *event, unsigned long id);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
int event(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, ...);

/******************************************************************************/

/* creates an event handler object */
static EventHandler *createEventHandler(unsigned long id);

/* frees an event handler object */
static void destroyEventHandler(EventHandler *handler);

/* does the actual work of registering an event handler */
static void addEventHandler(GHashTable *eventsTable, unsigned long id,
const char *event, const char *function, lua_State *L);

/* does the actual work of removing an event handler */
static void removeEventHandler(GHashTable *table, const char *event,
unsigned long id);

/* called by event: execute global handlers for an event */
static int globalEvent(const char *name, int numArgs, va_list args);

/* called by event: execute player-specific handlers for an event */
static int playerEvent(const char *name, Player *player, int numArgs,
va_list args);

/* called by event: execute entity-specific handlers for an event */
static int entityEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args);

/* called by global, player and entityEvent: executes event handlers associated
   with an event.  entityName is the name of the entity or player that owns the
   event handler (or NULL for global events.) */
static int executeEvent(GList *handlers, const char *entityName, int numArgs,
va_list args);

/******************************************************************************/

static unsigned long nextEventId = 0;
static GHashTable *globalEvents;

/******************************************************************************/

void initGlobalEvents() {

   globalEvents   = g_hash_table_new(g_str_hash, g_str_equal);
   return;
}

/******************************************************************************/

void destroyGlobalEvents() {

   destroyEventsList(globalEvents);
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

static EventHandler *createEventHandler(unsigned long id) {

   EventHandler *newHandler = malloc(sizeof(EventHandler));

   if (NULL == newHandler) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   newHandler->id = id;
   newHandler->function = NULL;
   newHandler->L = NULL;

   return newHandler;
}

/******************************************************************************/

static void destroyEventHandler(EventHandler *handler) {

   free(handler);
}

/******************************************************************************/

unsigned long addGlobalEventHandler(const char *event, const char *function,
lua_State *L) {

   unsigned long id = nextEventId++;
   addEventHandler(globalEvents, id, event, function, L);
   return id;
}

/******************************************************************************/

unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L) {

   unsigned long id = player->nextEventId++;
   addEventHandler(player->events, id, event, function, L);
   return id;
}

/******************************************************************************/

unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, enum EntityType type, const char *function, lua_State *L) {

   unsigned long id;

   switch (type) {

      case entity_room:
         id = ((Room *)entity)->nextEventId++;
         addEventHandler(((Room *)entity)->events, id, event, function, L);
         break;

      case entity_object:
         id = ((Object*)entity)->nextEventId++;
         addEventHandler(((Object *)entity)->events, id, event, function, L);
         break;

      case entity_creature:
         id = ((Creature *)entity)->nextEventId++;
         addEventHandler(((Creature *)entity)->events, id, event, function, L);
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

   return id;
}

/******************************************************************************/

static void addEventHandler(GHashTable *eventsTable, unsigned long id,
const char *event, const char *function, lua_State *L) {

   GList        *handlerList;
   EventHandler *newHandler;

   newHandler = createEventHandler(id);
   newHandler->function = function;
   newHandler->L = L;

   handlerList = g_hash_table_lookup(eventsTable, (char *)event);
   handlerList = g_list_append(handlerList, newHandler);
   g_hash_table_insert(eventsTable, (char *)event, handlerList);

   return;
}

/******************************************************************************/

void removeGlobalEventHandler(const char *event, unsigned long id) {

   removeEventHandler(globalEvents, event, id);
   return;
}

/******************************************************************************/

void removePlayerEventHandler(Player *player, const char *event,
unsigned long id) {

   // TODO
   return;
}

/******************************************************************************/

void removeEntityEventHandler(enum EntityType type, void *entity,
const char *event, unsigned long id) {

   // TODO
   return;
}

/******************************************************************************/

static void removeEventHandler(GHashTable *table, const char *event,
unsigned long id) {

   // TODO
   return;
}

/******************************************************************************/

int event(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, ...) {

   va_list args;
   va_start(args, numArgs);

   /* execute global event handlers */
   if (!globalEvent(name, numArgs, args)) {
      return FALSE;
   }

   /* execute player-specific event handlers */
   else if (!playerEvent(name, player, numArgs, args)) {
      return FALSE;
   }

   /* finally, execute entity-specific event handlers */
   else if (!entityEvent(name, player, entity, entityType, numArgs, args)) {
      return FALSE;
   }

   /* return true if we chose to execute all three and want to allow the action
      that triggered the event */
   else {
      return TRUE;
   }
}

/******************************************************************************/

static int globalEvent(const char *name, int numArgs, va_list args) {

   GList *handlers = g_hash_table_lookup(globalEvents, name);
   return executeEvent(handlers, NULL, numArgs, args);
}

/******************************************************************************/

static int playerEvent(const char *name, Player *player, int numArgs,
va_list args) {

   GList *handlers = g_hash_table_lookup(player->events, name);
   return executeEvent(handlers, player->name, numArgs, args);
}

/******************************************************************************/

static int entityEvent(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, va_list args) {

   GList     *handlers;
   dstring_t  entityName;

   switch (entityType) {

      case entity_room:
         entityName = ((Room *)entity)->name;
         handlers = g_hash_table_lookup(((Room *)entity)->events, name);
         break;

      case entity_object:
         entityName = ((Object *)entity)->name;
         handlers = g_hash_table_lookup(((Object *)entity)->events, name);
         break;

      case entity_creature:
         entityName = ((Creature *)entity)->name;
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

   return executeEvent(handlers, dstrview(entityName), numArgs, args);
}

/******************************************************************************/

static int executeEvent(GList *handlers, const char *entityName, int numArgs,
va_list args) {

   // TODO: if no handlers, return true, else, execute while return true and return error or true
}

