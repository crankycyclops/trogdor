
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

/* binds an event handler to a global event */
unsigned long addGlobalEventHandler(const char *event, const char *function,
lua_State *L);

/* binds an event handler to a player-specific event */
unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, EntityType type, const char *function, lua_State *L);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
extern int event(const char *name, Player *player, void *entity,
EntityType entityType, int numArgs, ...);

static unsigned long nextEventId = 0;
static GHashTable *globalEvents;

/******************************************************************************/

void initEvents() {

   globalEvents = g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

void destroyEvents() {

   GList *events = g_hash_table_get_values(globalEvents);
   GList *next = events;

   while (next != NULL) {
      g_list_destroy(next->data);
      next = next->next;
   }

   g_list_destroy(events);
   g_hash_table_destroy(globalEvents);
   globalEvents = NULL;
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

   /* use next available ID and increment it */
   EventHandler *newHandler = createEventHandler(nextEventId++);
   newHandler->function = function;
   newHandler->L = L;

   GList *handlerList = g_hash_table_lookup(globalEvents, (char *)event);
   handlerList = g_list_append(handlerList, newHandler);
   g_hash_table_insert(handlerList, (char *)event, handlerList);

   return;
}

/******************************************************************************/

unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L) {

}

/******************************************************************************/

unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, EntityType type, const char *function, lua_State *L) {

}

/******************************************************************************/

int event(const char *name, Player *player, void *entity, EntityType entityType,
int numArgs, ...) {

}

