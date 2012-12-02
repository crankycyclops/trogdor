#ifndef OBJECT_H
#define OBJECT_H


#include <dstring.h>
#include <lua.h>


/* represents the state of a game object */
typedef struct objectState {
   int seenByPlayer;    /* whether object has been seen by the player */
   int takenByPlayer;   /* whether object has been taken by the player */
   int droppedByPlayer; /* whether object has been dropped by the player */
} ObjectState;

/* Synonyms will be stored as keys in a hash table */
typedef struct object {

   dstring_t name;         /* name of the object */
   dstring_t description;  /* the user reads this when seen the first time */
   int weight;             /* object's weight */

   ObjectState state;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* Lua state containing scripted functions */
   lua_State *lua;

} Object;


#ifndef OBJECT_C

#include "room.h"
#include "player.h"

/* allows a player to describe an object */
extern void displayObject(Player *player, Object *object);

/* processes the posession of an object from the current room */
extern void takeObject(Player *player, Object *object);

/* let's a player drop the specified object into the current room */
extern void dropObject(Player *player, Object *object);

/* disambiguates in the case where a name refers to more than one object */
extern Object *clarifyObject(GList *objects, int objectCount);

#endif


#endif

