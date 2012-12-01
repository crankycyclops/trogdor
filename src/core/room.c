
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/object.h"
#include "include/event.h"

#define ROOM_C


/* sets our current location in the game */
void setLocation(Room *room);

/* prints description of a room (only the title once already visited) */
void displayRoom(Room *room, int showLongDescription);

/* prints the long description of a room */
static void describeRoom(Room *room);

/******************************************************************************/

void setLocation(Room *room) {

   if (ALLOW_ACTION != event("beforeSetLocation", room)) {
      return;
   }

   location = room;
   displayRoom(room, FALSE);
   room->state.visitedByPlayer = 1;

   event("afterSetLocation", room);
}

/******************************************************************************/

void displayRoom(Room *room, int showLongDescription) {

   GList *objectList = room->objectList;

   if (ALLOW_ACTION != event("beforeRoomDisplay", room)) {
      return;
   }

   printf("\n%s\n", dstrview(room->title));
   if (showLongDescription || 0 == room->state.visitedByPlayer) {
      describeRoom(room);
   }

   /* display objects in the room if they haven't already been seen */
   while (objectList != NULL) {

      if (!((Object *)objectList->data)->state.seenByPlayer) {
         displayObject((Object *)objectList->data);
         ((Object *)objectList->data)->state.seenByPlayer = 1;
      }

      else {
         printf("\nYou see a %s.\n",
            dstrview(((Object *)objectList->data)->name));
      }

      objectList = g_list_next(objectList);
   }

   event("afterRoomDisplay", room);
}

/******************************************************************************/

static void describeRoom(Room *room) {

   printf("\n%s\n", dstrview(room->description));
}
