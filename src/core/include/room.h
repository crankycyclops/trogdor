#ifndef ROOM_H
#define ROOM_H


#include <glib.h>
#include <dstring.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "messages.h"
#include "eventlist.h"


/* Represents the state of a single room */
typedef struct roomState {

   GHashTable *players;    /* list of players who have been to this room */
   int visitedByAPlayer;   /* whether or not a player has seen the room */

} RoomState;

/* Represents a single room. Each direction is a pointer to whatever room is
   located in that direction.  If you can't go in that direction, it should be
   set to null.
*/
typedef struct room {

   dstring_t name;           /* room's unique identifier */
   dstring_t title;          /* what we display to tell the user where we are */
   dstring_t description;    /* room's long description */

   struct room *north;
   struct room *south;
   struct room *east;
   struct room *west;

   struct room *in;
   struct room *out;
   struct room *up;
   struct room *down;

   Messages messages;        /* hash table of custom messages */

   EventHandlerList *events; /* event handlers */
   lua_State *L;             /* scripting environment for the room */

   RoomState state;

   /* A lookup table that maps object names and synonyms to actual objects.
      There may be ambiguities where more than one object has the same synonym.
      We therefore set as the value for each key a GArray.  If there is only one
      object associated with a given word (name or synonym), we know which
      object the name resolves to.  If we get more than one in the array, we
      have an ambiguity that must be resolved through further clarification from
      the user. */
   GHashTable *objectByName;

   /* We also need an iterable array of all objects in a room so that we can
      quickly operate on all objects when we enter a room. */
   GList *objectList;

   /* maps creature names to creature objects */
   GHashTable *creatureByName;

   /* Iterable list of all creatures in a room */
   GList *creatureList;

} Room;

#include "object.h"


#ifndef ROOM_C

#include "player.h"

/* Allocates memory for a room.  If initMessages is true, we allocate memory
   for the messages structure.  Otherwise, just set it to NULL. */
extern Room *createRoom(int initMessages);

/* frees memory associated with a room */
extern void destroyRoom(Room *room);

/* sets a player's current location in the game */
extern void setLocation(Player *player, Room *room, int triggerEvents);

/* prints out the description of a room */
extern void displayRoom(Player *player, Room *room, int showLongDescription);

#endif


#endif

