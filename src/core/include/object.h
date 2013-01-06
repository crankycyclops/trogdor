#ifndef OBJECT_H
#define OBJECT_H


#include <glib.h>
#include <dstring.h>
#include <lua.h>

#include "messages.h"


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

   /* combat-related properties */
   int weapon;             /* true if the object is a weapon */
   int damage;             /* if object is a weapon, how much damage it does */

   Messages messages;      /* hash table of custom messages */
   ObjectState state;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* event handlers */
   lua_State      *L;
   GHashTable     *events;

} Object;

/* represents a creature's or a player's inventory */
typedef struct inventory {

   /* list and by-name index of object pointers */
   GHashTable  *byName;
   GList       *list;

   /* current weight of inventory */
   int weight;

   /* inventory can't weight more than this value (if 0, unlimited) */
   int maxWeight;

} Inventory;


#ifndef OBJECT_C

#include "room.h"
#include "player.h"

/* Allocates memory for a new game object.  If initMessages is true, we allocate
   memory for the messages structure.  Otherwise, just set it to NULL. */
extern Object *createObject(int initMessages);

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

