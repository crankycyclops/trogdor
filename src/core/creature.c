
#define CREATURE_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/event.h"

/* allocates memory for a creature */
Creature *creatureAlloc();


/******************************************************************************/

Creature *creatureAlloc() {

   Creature *newcreature;

   newcreature = malloc(sizeof(Creature));
   if (NULL == newcreature) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   /* initialize the creature's state */
   newcreature->state.seenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   newcreature->state.alive = 1;
   newcreature->state.seenByAPlayer = 0;

   return newcreature;
}

