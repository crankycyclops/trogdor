#ifndef ROOM_H
#define ROOM_H


#include <dstring.h>


/* Represents the state of a single room */
typedef struct roomState {

   int visitedByPlayer;    /* whether or not player has seen the room */

} RoomState;

/* Represents a single room. Each direction is a pointer to whatever room is
   located in that direction.  If you can't go in that direction, it should be
   set to null.
*/
typedef struct room {

   dstring_t name;         /* room's unique identifier */
   dstring_t title;        /* what we display to tell the user where we are */
   dstring_t description;  /* room's long description */

   struct room *north;
   struct room *south;
   struct room *east;
   struct room *west;

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
} Room;


#ifndef ROOM_C

/* sets our current location in the game */
extern void setLocation(Room *room);

/* prints out the description of a room */
extern void displayRoom(Room *room);

#endif


#endif

