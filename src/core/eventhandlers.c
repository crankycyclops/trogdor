
#define EVENTHANDLERS_C

#include "include/eventhandlers.h"
#include "include/timerjobs.h"


/* Responds to afterSetLocation event and automatically initiates an attack
   against the player by any creatures that have auto-attack turned on. */
int eventHandlerAutoAttack(int nargs, va_list args);

/******************************************************************************/

int eventHandlerAutoAttack(int nargs, va_list args) {

   Player *player = va_arg(args, EventArgument *)->value.player;
   Room *room = va_arg(args, EventArgument *)->value.room;

   // TODO: get creature list for room
   // TODO: foreach creature, if auto attack, registe timer job for it
   g_outputString("STUB: eventHandlerAutoAttack!  Player: %s, Room: %s\n",
      dstrview(player->name), dstrview(room->name));

   return EVENT_CONTINUE_HANDLERS | EVENT_ALLOW_ACTION;
}

