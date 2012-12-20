
#define EVENT_C

#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/state.h"
#include "include/player.h"
#include "include/event.h"
#include "include/player.h"
#include "include/data.h"


/* initializes the global event handler */
void initEvents();

/* destructor for the global event handler */
void destroyEvents();

/* called by destroyEvents() to destroy a hash table of event-handler maps */
static static void destroyEventsList(GHashTable *table);

/* binds an event handler to a global event */
unsigned long addGlobalEventHandler(const char *event, const char *function,
lua_State *L);

/* binds an event handler to a player-specific event */
unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, EntityType type, const char *function, lua_State *L);

/* does the actual work of registering an event handler */
static void addEventHandler(GHashTable *eventsTable, unsigned long id,
const char *event, const char *function, lua_State *L);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
int event(const char *name, Player *player, void *entity,
EntityType entityType, int numArgs, ...);

/* called by event: execute global handlers for an event */
static int globalEvent(const char *name, va_list args, int numArgs);

/* called by event: execute player-specific handlers for an event */
static int playerEvent(const char *name, Player *player, va_list args,
int numArgs);

/* called by event: execute entity-specific handlers for an event */
static int entityEvent(const char *name, Player *player, void *entity,
EntityType entityType, va_list args, int numArgs);

/* called by global, player and entityEvent: executes an event handler */
static int executeEvent(EventHandler handler);


static unsigned long nextGlobalEventId = 0;
static unsigned long nextPlayerEventId = 0;
static unsigned long nextEntityEventId = 0;

static GHashTable *globalEvents;
static GHashTable *playerEvents;

/* entity-specific events */
static GHashTable *roomEvents;
static GHashTable *objectEvents;
static GHashTable *creatureEvents;

/******************************************************************************/

void initEvents() {

   globalEvents   = g_hash_table_new(g_str_hash, g_str_equal);
   playerEvents   = g_hash_table_new(g_str_hash, g_str_equal);
   roomEvents     = g_hash_table_new(g_str_hash, g_str_equal);
   objectEvents   = g_hash_table_new(g_str_hash, g_str_equal);
   creatureEvents = g_hash_table_new(g_str_hash, g_str_equal);

   return;
}

/******************************************************************************/

void destroyEvents() {

   destroyEventsList(globalEvents);
   destroyEventsList(playerEvents);
   destroyEventsList(roomEvents);
   destroyEventsList(objectEvents);
   destroyEventsList(creatureEvents);

   return;
}

/******************************************************************************/

static void destroyEventsList(GHashTable *table) {

   GList *events = g_hash_table_get_values(table);
   GList *next = events;

   while (next != NULL) {
      g_list_destroy(next->data);
      next = next->next;
   }

   g_list_destroy(events);
   g_hash_table_destroy(table);
   return;
}

/******************************************************************************/

EventHandler *createEventHandler(unsigned long id) {

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

void destroyEventHandler(EventHandler *handler) {

   free(handler);
}

/******************************************************************************/

unsigned long addGlobalEventHandler(const char *event, const char *function,
lua_State *L) {

   unsigned long id = nextGlobalEventId++;
   addEventHandler(globalEvents, id, event, function, L);
   return id;
}

/******************************************************************************/

unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L) {

   unsigned long id = nextPlayerEventId++;
   addEventHandler(playerEvents, id, event, function, L);
   return id;
}

/******************************************************************************/

unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, EntityType type, const char *function, lua_State *L) {

   unsigned long id = nextEntityEventId++;

   switch (type) {

      case room:
         addEventHandler(roomEvents, id, event, function, L);
         break;

      case object:
         addEventHandler(objectEvents, id, event, function, L);
         break;

      case creature:
         addEventHandler(creatureEvents, id, event, function, L);
         break;

      case player:
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

int event(const char *name, Player *player, void *entity, EntityType entityType,
int numArgs, ...) {

   va_list args;
   va_start(args, numArgs);

   /* execute global event handlers */
   if (!globalEvent(name, args, numArgs)) {
      return FALSE;
   }

   /* execute player-specific event handlers */
   else if (!playerEvent(name, player, args, numArgs)) {
      return FALSE;
   }

   /* finally, execute entity-specific event handlers */
   else if (!entityEvent(name, player, entity, entityType, args, numArgs)) {
      return FALSE;
   }

   /* return true if we chose to execute all three and want to allow the action
      that triggered the event */
   else {
      return TRUE;
   }
}

/******************************************************************************/

static int globalEvent(const char *name, va_list args, int numArgs) {

   // TODO
   return TRUE;
}

/******************************************************************************/

static int playerEvent(const char *name, Player *player, va_list args,
int numArgs) {

   // TODO
   return TRUE;
}

/******************************************************************************/

static int entityEvent(const char *name, Player *player, void *entity,
EntityType entityType, va_list args, int numArgs) {

   // TODO
   return TRUE;
}

/******************************************************************************/

static int executeEvent(EventHandler handler) {

}

