#ifndef EVENT_H
#define EVENT_H

#include <glib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"


/* Bit flags we can set on the return value of an event handler.
   CONTINUE_HANDLERS - continue executing other event handlers
   CONTINUE_ACTION   - allow the action that triggered the event
*/
#define CONTINUE_HANDLERS 0x01
#define CONTINUE_ACTION   0x02


/* every event handler consists of a Lua state and a function to call */
typedef struct {
   unsigned long id;
   const char *function;
   lua_State  *L;
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


/* initializes global event handlers */
extern void initGlobalEvents();

/* frees memory associated with the global event handlers */
extern void destroyGlobalEvents();

/* allocates memory for a new event handlers list */
extern GHashTable *createEventsList();

/* called by destroyEvents() to destroy a hash table of event-handler maps */
extern void destroyEventsList(GHashTable *table);

/* binds an event handler to a global event */
extern unsigned long addGlobalEventHandler(const char *event,
const char *function);

/* binds an event handler to a player-specific event */
extern unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
extern unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, enum EntityType type, const char *function, lua_State *L);

/* removes a global event handler */
extern void removeGlobalEventHandler(const char *event, unsigned long id);

/* removes a player-specific event handler */
extern void removePlayerEventHandler(Player *player, const char *event,
unsigned long id);

/* removes an entity-specific event handler */
extern void removeEntityEventHandler(enum EntityType type, void *entity,
const char *event, unsigned long id);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
extern int event(const char *name, Player *player, void *entity,
enum EntityType entityType, int numArgs, ...);

#endif


#endif
