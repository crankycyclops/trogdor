
#include <stdio.h>
#include <stdlib.h>

#define ACTION_C

#include "include/trogdor.h"
#include "include/state.h"
#include "include/data.h"
#include "include/command.h"

#include "include/action.h"
#include "include/actions.h"

/*
   Calls whatever action corresponds to the given command.

   Output:
      If there was a syntax error, or if the given verb doesn't correspond to
      any known action, false.  Otherwise, true.
*/
int callAction(Command command);


int callAction(Command command) {

   int i;

   /* initialize our list of verb => function maps */
   static Verb verbs[] = {

      /* all synonyms for moving in a given direction (go requires direction) */
      {"go", &move},
      {"north", &move},
      {"south", &move},
      {"east",  &move},
      {"west",  &move},

      /* verbs for picking up an object (requires a direct object) */
      {"take",  &pickupObject},
      {"grab",  &pickupObject},
      {"own",   &pickupObject},
      {"claim", &pickupObject},
      {"carry", &pickupObject},

      {"quit",  &quitGame},
      {NULL, NULL}
   };

   for (i = 0; verbs[i].word != NULL; i++) {
      if (0 == strcmp(verbs[i].word, dstrview(command.verb))) {
         return verbs[i].action(command);
      }
   }

   /* action was not found, so the verb must not exist in our vocabulary */
   return 0;
}

