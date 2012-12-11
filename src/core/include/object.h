#ifndef OBJECT_H
#define OBJECT_H


#include <dstring.h>
#include <lua.h>


/* represents the state of a game object */
typedef struct objectState {

   GHashTable *seenByPlayers;    /* all players that have seen the object */
   GHashTable *takenByPlayers;   /* all players that have taken the object */
   GHashTable *droppedByPlayers; /* all players that have dropped the object */

   int seenByPlayer;    /* whether object has been seen by any player */
   int takenByPlayer;   /* whether object has been taken by any player */
   int droppedByPlayer; /* whether object has been dropped by any player */

} ObjectState;

/* Synonyms will be stored as keys in a hash table */
typedef struct object {

   dstring_t name;         /* name of the object */
   dstring_t description;  /* the user reads this when seen the first time */
   int weight;             /* object's weight */
   int takeable;           /* whether or not object can be taken */
   int droppable;          /* whether or not object can be dropped */

   ObjectState state;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* Lua state containing scripted functions */
   lua_State *lua;

} Object;


#ifndef OBJECT_C

#include "room.h"
#include "player.h"

/* allocates memory for a new game object */
extern Object *objectAlloc();

/* frees memory associated with an object */
extern void destroyObject(Object *object);

/* allows a player to describe an object */
extern void displayObject(Player *player, Object *object, int showLongDescription);

/* processes the posession of an object from the current room */
extern void takeObject(Player *player, Object *object);

/* let's a player drop the specified object into the current room */
extern void dropObject(Player *player, Object *object);

/* disambiguates in the case where a name refers to more than one object */
extern Object *clarifyObject(GList *objects, int objectCount);

#endif


#endif

