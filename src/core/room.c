
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/player.h"
#include "include/command.h"
#include "include/object.h"
#include "include/event.h"

#define ROOM_C


/* sets a player's current location in the game */
void setLocation(Player *player, Room *room);

/* prints description of a room (only the title once already visited) */
void displayRoom(Player *player, Room *room, int showLongDescription);

/* prints the long description of a room */
static void describeRoom(Room *room);

/******************************************************************************/

void setLocation(Player *player, Room *room) {

   if (ALLOW_ACTION != event(player, "beforeSetLocation", room)) {
      return;
   }

   player->location = room;
   displayRoom(player, room, FALSE);
   room->state.visitedByPlayer = 1;

   event(player, "afterSetLocation", room);
}

/******************************************************************************/

void displayRoom(Player *player, Room *room, int showLongDescription) {

   GList *objectList = room->objectList;

   if (ALLOW_ACTION != event(player, "beforeRoomDisplay", room)) {
      return;
   }

   g_outputString("\n%s\n", dstrview(room->title));
   if (showLongDescription || 0 == room->state.visitedByPlayer) {
      describeRoom(room);
   }

   /* display objects in the room if they haven't already been seen */
   while (objectList != NULL) {

      if (!((Object *)objectList->data)->state.seenByPlayer) {
         displayObject(player, (Object *)objectList->data);
         ((Object *)objectList->data)->state.seenByPlayer = 1;
      }

      else {
         g_outputString("\nYou see a %s.\n",
            dstrview(((Object *)objectList->data)->name));
      }

      objectList = g_list_next(objectList);
   }

   event(player, "afterRoomDisplay", room);
}

/******************************************************************************/

static void describeRoom(Room *room) {

   g_outputString("\n%s\n", dstrview(room->description));
}
