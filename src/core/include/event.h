#ifndef EVENT_H
#define EVENT_H

#include <glib.h>

#include "lua.h"
#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"


/* Bit flags we can set on the return value of an event handler.
   CONTINUE_HANDLERS - continue executing other event handlers
   CONTINUE_ACTION   - allow the action that triggered the event
*/
#define EVENT_CONTINUE_HANDLERS 0x01
#define EVENT_ALLOW_ACTION      0x02


/* pointer to a function an event handler can call */
typedef int (*EventFunctionPtr)(int numArgs, ...);

/* every event handler will contain an EventFunction, which will contain either
   a pointer to a native C function or a lua state and function name. */
typedef union {
   LuaFunction       luaHandler;
   EventFunctionPtr  nativeHandler;
} EventFunction;

typedef struct {
   unsigned long id;
   enum {EVENT_HANDLER_LUA, EVENT_HANDLER_NATIVE} funcType;
   EventFunction func;
} EventHandler;

/* all events take a variable number of arguments that can be passed to each
   event handler (global, player and entity) */
typedef struct {

   /* data type */
   enum {
         scriptval_number,
         scriptval_string,
         scriptval_room,
         scriptval_player,
         scriptval_creature,
         scriptval_object
   } type;

   /* actual value */
   union {
      double      number;
      const char  *string;
      Room        *room;
      Player      *player;
      Creature    *creature;
      Object      *object;
   } value;

} EventArgument;


#ifndef EVENT_C

/* lua state containing functions called by global events */
extern lua_State *globalL;

/* constructor for the event handler */
extern void initEventHandler();

/* destructor for the event handler */
extern void destroyEventHandler();

/* Binds an event handler written in C to an event.  The order in which event
   handlers are added is the order in which event handlers are executed. */
extern unsigned long addEventHandler(const char *event,
EventHandlerList *handlerList, EventFunctionPtr function);

/* Binds an event handler written in Lua to an event. */
extern unsigned long addLuaEventHandler(const char *event,
EventHandlerList *handlerList, const char *function, lua_State *L);

/* Unbinds an existing event handler from a specific event.  Takes as input
   the event the handler is bound to and the event handler's id (returned by
   addEventHandler and addLuaEventHandler.) Returns TRUE if the handler exists
   and is removed and FALSE if it doesn't exist. */
extern int removeEventHandler(const char *event, EventHandlerList handlerList,
unsigned long id);

/* Binds a global event handler written in C to an event. */
extern unsigned long addGlobalEventHandler(const char *event, EventFunctionPtr function);

/* Binds a global event handler written in Lua to an event. */
extern unsigned long addGlobalLuaEventHandler(const char *event, const char *function,
lua_State *L);

/* Unbinds an existing global event handler.  Calls removeEventHandler. */
extern int removeGlobalEventHandler(const char *event, unsigned long id);

/* Sets an entity up to listen for events */
void addEventListener(EventHandlerList *handlers);

/* Triggers an event.  numArgs should be set to the number of EventArgument
   parameters that are passed when the event is triggered. */
extern int event(const char *event, int numArgs, ...);

/* Functions that create EventArgument structs for various types.  EventArgument
   parameters that are passed to an event handler are freed by the event
   handler, so you don't have to worry about memory management.  It's good
   coding practice to always use these functions when passing arguments, and to
   never free the return value yourself (which will probably cause nasty
   bugs!) */ 
extern EventArgument *eventArgNumber(double value);
extern EventArgument *eventArgString(const char *value);
extern EventArgument *eventArgRoom(Room *value);
extern EventArgument *eventArgPlayer(Player *value);
extern EventArgument *eventArgCreature(Creature *value);
extern EventArgument *eventArgObject(Object *value);

#endif


#endif
