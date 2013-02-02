
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
#include "include/data.h"
#include "include/eventlist.h"


/* constructor for the event handling mechanism */
void initEvents();

/* destructor for the event handling mechanism */
void destroyEvents();

/* Binds an event handler written in C to an event.  The order in which event
   handlers are added is the order in which event handlers are executed. */
unsigned long addEventHandler(const char *event, EventFunctionPtr function);

/* Binds an event handler written in Lua to an event. */
unsigned long addLuaEventHandler(const char *event, const char *function,
lua_State *L);

/* Unbinds an existing event handler from a specific event.  Takes as input
   the event the handler is bound to and the event handler's id (returned by
   addEventHandler and addLuaEventHandler.) Returns TRUE if the handler exists
   and is removed and FALSE if it doesn't exist. */
int removeEventHandler(const char *event, unsigned long id);

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

/* creates a new EventHandler struct */
static EventHandler *createEventHandler();

/* frees memory associated with EventHandler */
static void destroyEventHandler(EventHandler *handler);

/* executes an event handler */
static int executeEvent(EventHandler *handler, int numArgs, va_list args);

/* executes an event handler written in C.  Called by executeEvent */
static int executeNativeHandler(EventFunctionPtr function, int numArgs,
va_list args);

/* executes an event handler written in Lua.  Called by executeEvent. */
static int executeLuaHandler(const char *function, lua_State *L, int numArgs,
va_list args);

/* passes an argument to a Lua event handler */
static void eventPassArgument(lua_State *L, EventArgument *arg);

/******************************************************************************/

/* lua state containing functions called by global events */
lua_State *globalL = NULL;

/* structure that maps event names to event handlers */
static EventHandlerList *handlerList = NULL;

/******************************************************************************/

static EventHandler *createEventHandler() {

   EventHandler *newHandler = malloc(sizeof(EventHandler));

   if (NULL == newHandler) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   return newHandler;
}

/******************************************************************************/

static void destroyEventHandler(EventHandler *handler) {

   free(handler);
}

/******************************************************************************/

void initEvents() {

   handlerList = initEventHandlerList();
}

/******************************************************************************/

void destroyEvents() {

   destroyEventHandlerList(handlerList);

   if (NULL != globalL) {
      lua_close(globalL);
   }
}

/******************************************************************************/

unsigned long addEventHandler(const char *event, EventFunctionPtr function) {

   GList *handlers = g_hash_table_lookup(handlerList->eventHandlers, event);
   EventHandler *handler = createEventHandler();

   handler->id = handlerList->nextId;
   handler->funcType = EVENT_HANDLER_NATIVE;
   handler->func.nativeHandler = function;

   handlers = g_list_append(handlers, handler);
   g_hash_table_insert(handlerList->eventHandlers, (char *)event, handlers);

   return handlerList->nextId++;
}

/******************************************************************************/

unsigned long addLuaEventHandler(const char *event, const char *function,
lua_State *L) {

   GList *handlers = g_hash_table_lookup(handlerList->eventHandlers, event);
   EventHandler *handler = createEventHandler();

   handler->id = handlerList->nextId;
   handler->funcType = EVENT_HANDLER_LUA;
   handler->func.luaHandler.function = function;
   handler->func.luaHandler.L = L;

   handlers = g_list_append(handlers, handler);
   g_hash_table_insert(handlerList->eventHandlers, (char *)event, handlers);

   return handlerList->nextId++;
}

/******************************************************************************/

int removeEventHandler(const char *event, unsigned long id) {

   GList *handlers = g_hash_table_lookup(handlerList->eventHandlers, event);
   GList *nextHandler = handlers;

   while (NULL != nextHandler) {

      EventHandler *handler = (EventHandler *)nextHandler->data;

      if (id == handler->id) {
         free(handler);
         handlers = g_list_remove(handlers, nextHandler);
         g_hash_table_insert(handlerList->eventHandlers, (char *)event, handlers);
         return TRUE;
      }

      nextHandler = nextHandler->next;
   }

   return FALSE;
}

/******************************************************************************/

int event(const char *event, int numArgs, ...) {

   int i;
   va_list args;

   GList *handlers = g_hash_table_lookup(handlerList->eventHandlers, event);
   GList *nextHandler = handlers;

   int allowAction = TRUE;

   /* do nothing if there are no handlers for this event */
   if (!handlers) {
      return TRUE;
   }

   while (NULL != nextHandler) {

      EventHandler *handler;
      int status;

      va_start(args, numArgs);
      handler = (EventHandler *)nextHandler->data;
      status = executeEvent(handler, numArgs, args);

      allowAction = EVENT_ALLOW_ACTION & status;

      /* handler may have overridden execution of any further handlers */
      if (!(EVENT_CONTINUE_HANDLERS & status)) {
         break;
      }

      nextHandler = nextHandler->next;
   }

   /* free memory allocated for EventArguments */
   va_start(args, numArgs);
   for (i = 0; i < numArgs; i++) {
      free(va_arg(args, EventArgument *));
   }

   return allowAction;
}

/******************************************************************************/

static int executeEvent(EventHandler *handler, int numArgs, va_list args) {

   if (EVENT_HANDLER_NATIVE == handler->funcType) {
      return executeNativeHandler(handler->func.nativeHandler, numArgs, args);
   }

   else {
      return executeLuaHandler(handler->func.luaHandler.function,
         handler->func.luaHandler.L, numArgs, args);
   }
}

/******************************************************************************/

static int executeNativeHandler(EventFunctionPtr function, int numArgs,
va_list args) {

   return function(numArgs, args);
}

/******************************************************************************/

static int executeLuaHandler(const char *function, lua_State *L, int numArgs,
va_list args) {

   int i;

   /* default return values */
   int status = EVENT_CONTINUE_HANDLERS | EVENT_ALLOW_ACTION;

   if (NULL != L) {

      lua_getglobal(L, function);

      /* only call function if it exists */
      if (lua_isfunction(L, lua_gettop(L))) {

         /* pass arguments */
         for (i = 0; i < numArgs; i++) {

            EventArgument *arg = va_arg(args, EventArgument *);
            eventPassArgument(L, arg);
         }

         /* function should return two arguments, EVENT_CONTINUE_HANDLERS and
            EVENT_ALLOW_ACTION */
         if (lua_pcall(L, numArgs, 2, 0)) {
            g_outputError("Script error: %s\n", lua_tostring(L, -1));
         }

         else {

            if (!lua_isboolean(L, -1) || !lua_isboolean(L, -2)) {
               g_outputError("Script error: %s must return booleans!\n", function);
            }

            status = lua_toboolean(L, -2) ? status | EVENT_CONTINUE_HANDLERS :
               status & ~EVENT_CONTINUE_HANDLERS;
            status = lua_toboolean(L, -1) ? status | EVENT_ALLOW_ACTION :
               status & ~EVENT_ALLOW_ACTION;
         }
      }
   }

   return status;
}

/******************************************************************************/

static void eventPassArgument(lua_State *L, EventArgument *arg) {

   switch (arg->type) {

      case scriptval_number:
         lua_pushnumber(L, arg->value.number);
         break;

      case scriptval_string:
         lua_pushstring(L, arg->value.string);
         break;

      case scriptval_player:

         if (NULL != arg->value.player) {
            lua_pushstring(L, arg->value.player->name);
         }

         else {
            lua_pushboolean(L, FALSE);
         }

         break;

      case scriptval_room:

         if (NULL != arg->value.room) {
            lua_pushstring(L, arg->value.room->name);
         }

         else {
            lua_pushboolean(L, FALSE);
         }

         break;

      case scriptval_object:

         if (NULL != arg->value.object) {
            lua_pushstring(L, arg->value.object->name);
         }

         else {
            lua_pushboolean(L, FALSE);
         }

         break;

      case scriptval_creature:

         if (NULL != arg->value.creature) {
            lua_pushstring(L, arg->value.creature->name);
         }

         else {
            lua_pushboolean(L, FALSE);
         }

         break;

      default:
         g_outputString("Invalid type for EventArgument.  This is a bug.\n");
         break;
   }

   return;
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

