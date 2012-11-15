
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/state.h"
#include "include/data.h"
#include "include/command.h"
#include "include/actions.h"
#include "include/vocabulary.h"

/*
   Calls whatever action corresponds to the given command.

   Output:
      If there was a syntax error, or if the given verb doesn't correspond to
      any known action, false.  Otherwise, true.
*/
int callAction(Command command);

/******************************************************************************/

int callAction(Command command) {

   int i;

   /* initialize our list of verb => function maps */
   Verb *verbs = getVerbs();

   for (i = 0; verbs[i].word != NULL; i++) {
      if (0 == strcmp(verbs[i].word, dstrview(command.verb))) {
         return verbs[i].action(command);
      }
   }

   /* action was not found, so the verb must not exist in our vocabulary */
   return 0;
}

