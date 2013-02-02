
#define EVENT_C

#include <stdlib.h>
#include <glib.h>

#include "include/lua.h"
#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/state.h"
#include "include/player.h"
#include "include/event.h"
#include "include/player.h"
#include "include/data.h"


/* constructor for the event handler */
void initEventHandler();

/* destructor for the event handler */
void destroyEventHandler();

/* Binds an event handler written in C to an event.  The order in which event
   handlers are added is the order in which event handlers are executed. */
unsigned long addEventHandler(const char *event, EventFunction function);

/* Binds an event handler written in Lua to an event. */
unsigned long addLuaEventHandler(const char *event, const char *function,
lua_State *L);

/* Unbinds an existing event handler from a specific event.  Takes as input
   the event the handler is bound to and the event handler's id (returned by
   addEventHandler and addLuaEventHandler.) */
void removeEventHandler(const char *event, unsigned long id);

/* Triggers an event.  numArgs should be set to the number of EventArgument
   parameters that are passed when the event is triggered. */
int event(const char *event, int numArgs, ...);

/* Functions that create EventArgument structs for various types.  EventArgument
   parameters that are passed to an event handler are freed by the event
   handler, so you don't have to worry about memory management.  It's good
   coding practice to always use these functions when passing arguments, and to
   never free the return value yourself (which will probably cause nasty
   bugs!) */ 
EventArgument *eventArgNumber(double value);
EventArgument *eventArgString(const char *value);
EventArgument *eventArgRoom(Room *value);
EventArgument *eventArgPlayer(Player *value);
EventArgument *eventArgCreature(Creature *value);
EventArgument *eventArgObject(Object *value);

/******************************************************************************/

/* allocates memory for an event handler argument */
static EventArgument *makeEventArg();

/******************************************************************************/

/* lua state containing functions called by global events */
lua_State *globalL = NULL;

/* hash table that maps event names to event handlers */
static GHashTable *eventHandlers = NULL;

/* each event handler is assigned a unique id */
static unsigned long nextId = 0;

/******************************************************************************/

void initEventHandler() {

   eventHandlers = g_hash_table_new(g_str_hash, g_str_equal);
   return;
}

/******************************************************************************/

void destroyEventHandler() {

   GList *events = g_hash_table_get_values(eventHandlers);
   GList *next = events;

   while (next != NULL) {
      g_list_free(next->data);
      next = next->next;
   }

   g_list_free(events);
   g_hash_table_destroy(eventHandlers);

   if (NULL != globalL) {
      lua_close(globalL);
   }

   return;
}

/******************************************************************************/

unsigned long addEventHandler(const char *event, EventFunction function) {

   // TODO
   return nextId++;
}

/******************************************************************************/

unsigned long addLuaEventHandler(const char *event, const char *function,
lua_State *L) {

   // TODO
   return nextId++;
}

/******************************************************************************/

void removeEventHandler(const char *event, unsigned long id) {

   // TODO
   return;
}

/******************************************************************************/

int event(const char *event, int numArgs, ...) {

   int i;
   va_list args;

   // TODO

   /* free memory allocated for EventArguments */
   va_start(args, numArgs);
   for (i = 0; i < numArgs; i++) {
      free(va_arg(args, EventArgument *));
   }

   return TRUE;
}

/******************************************************************************/

EventArgument *eventArgNumber(double value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_number;
   arg->value.number = value;

   return arg;
}

/******************************************************************************/

EventArgument *eventArgString(const char *value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_string;
   arg->value.string = value;

   return arg;
}

/******************************************************************************/

EventArgument *eventArgRoom(Room *value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_room;
   arg->value.room = value;

   return arg;
}

/******************************************************************************/

EventArgument *eventArgPlayer(Player *value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_player;
   arg->value.player = value;

   return arg;
}

/******************************************************************************/

EventArgument *eventArgCreature(Creature *value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_creature;
   arg->value.creature = value;

   return arg;
}

/******************************************************************************/

EventArgument *eventArgObject(Object *value) {

   EventArgument *arg = makeEventArg();

   arg->type = scriptval_object;
   arg->value.object = value;

   return arg;
}

/******************************************************************************/

static EventArgument *makeEventArg() {

   EventArgument *arg;

   arg = malloc(sizeof(EventArgument));
   if (NULL == arg) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   return arg;
}

