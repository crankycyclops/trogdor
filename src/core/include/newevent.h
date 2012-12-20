#ifndef EVENT_H
#define EVENT_H


/* every event handler consists of a Lua state and a function to call */
typedef struct {
   unsigned long id;
   const char *function;
   lua_State  *L;
} EventHandler;

/* all events take a variable number of arguments that can be passed to each
   event handler (global, player and entity) */
typedef struct {

   /* variable name */
   const char *name;

   /* data type */
   enum {
      number,
      string,
      room,
      player,
      creature,
      object
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


/* initializes the global event handler */
extern void initEventHandler();

/* destructor for the global event handler */
extern void destroyEventHandler();

/* binds an event handler to a global event */
extern unsigned long addGlobalEventHandler(const char *event,
const char *function, lua_State *L);

/* binds an event handler to a player-specific event */
extern unsigned long addPlayerEventHandler(const char *event, Player *player,
const char *function, lua_State *L);

/* binds an event handler to an entity-specific event */
extern unsigned long addEntityEventHandler(const char *event, Player *player,
void *entity, EntityType type, const char *function, lua_State *L);

/* Triggers an event.  Accepts variable number of EventArgument parameters. */
extern int event(const char *name, Player *player, void *entity,
EntityType entityType, int numArgs, ...);

#endif


#endif

