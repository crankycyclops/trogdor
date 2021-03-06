
#include <stdlib.h>

#include "include/vocabulary.h"
#include "include/command.h"
#include "include/player.h"
#include "include/actions.h"


/* return our list of verb => action mappings */
Verb *getVerbs();

/******************************************************************************/

const char *fillerWords[] = {
   "the",
   NULL
};

const char *prepositions[] = {
   "about",
   "after",
   "against",
   "along",
   "alongside",
   "amid",
   "amidst",
   "among",
   "amongst",
   "around",
   "aside",
   "astride",
   "at",
   "before",
   "behind",
   "below",
   "beneath",
   "beside",
   "besides",
   "between",
   "beyond",
   "by",
   "for",
   "from",
   "in",
   "inside",
   "into",
   "near",
   "of",
   "off",
   "on",
   "onto",
   "out",
   "over",
   "through",
   "to",
   "toward",
   "towards",
   "under",
   "underneath",
   "upon",
   "via",
   "with",
   "within",
   "without",
   NULL
};

/******************************************************************************/

Verb *getVerbs() {

   /* initialize our list of verb => function maps */
   static Verb verbs[] = {

      /* all synonyms for moving in a given direction (go requires direction) */
      {"go",    &actionMove},
      {"move",  &actionMove},
      {"north", &actionMove},
      {"south", &actionMove},
      {"east",  &actionMove},
      {"west",  &actionMove},
      {"in",    &actionMove},
      {"out",   &actionMove},
      {"up",    &actionMove},
      {"down",  &actionMove},

      /* verbs for picking up an object (requires a direct object) */
      {"take",  &actionPickupObject},
      {"grab",  &actionPickupObject},
      {"own",   &actionPickupObject},
      {"claim", &actionPickupObject},
      {"carry", &actionPickupObject},

      /* verbs for dropping an object (requires direct object) */
      {"drop",  &actionDropObject},

      /* verbs for displaying things */
      {"show",     &actionLook},
      {"describe", &actionLook},
      {"look",     &actionLook},
      {"examine",  &actionLook},

      /* combat */
      {"attack", &actionAttack},
      {"hit",    &actionAttack},
      {"harm",   &actionAttack},
      {"kill",   &actionAttack},
      {"injure", &actionAttack},
      {"maim",   &actionAttack},
      {"fight",  &actionAttack},

      /* player status */
      {"list",      &actionList},
      {"inv",       &actionList},
      {"inventory", &actionList},
      {"health",    &actionHealth},

      /* misc actions */
      {"jump", &actionJump},
      {"leap", &actionJump},

      /* fun: bare bones profanity recognition :) */
      {"fuck",    &actionProfanity},
      {"shit",    &actionProfanity},
      {"damn",    &actionProfanity},
      {"bitch",   &actionProfanity},
      {"asshole", &actionProfanity},

      /* TODO: lose command = lose game (haha) */
      {"quit",  &actionQuit},

      {NULL, NULL}
   };

   return verbs;
}

