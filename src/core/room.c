
#include <stdio.h>
#include <stdlib.h>

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

   /* this only tells us that the room was visited and not by who*/
   room->state.visitedByAPlayer = 1;

   /* this tells us who's visited the room */
   g_hash_table_insert(room->state.players, (char *)dstrview(player->name),
      player);

   event(player, "afterSetLocation", room);
}

/******************************************************************************/

void displayRoom(Player *player, Room *room, int showLongDescription) {

   GList *objectList = room->objectList;

   if (ALLOW_ACTION != event(player, "beforeRoomDisplay", room)) {
      return;
   }

   g_outputString("\n%s\n", dstrview(room->title));
   if (showLongDescription ||
   NULL == g_hash_table_lookup(room->state.players,
   (char *)dstrview(player->name))) {
      describeRoom(room);
   }

   /* display objects in the room if they haven't already been seen */
   while (objectList != NULL) {
      displayObject(player, (Object *)objectList->data, FALSE);
      objectList = g_list_next(objectList);
   }

   event(player, "afterRoomDisplay", room);
}

/******************************************************************************/

static void describeRoom(Room *room) {

   g_outputString("\n%s\n", dstrview(room->description));
}
