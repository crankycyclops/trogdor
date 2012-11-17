
#include <stdlib.h>
#include <ctype.h>

/* returns TRUE if a string is an integer and FALSE if it's not */
int isInt(char *str);

/******************************************************************************/

int isInt(char *str) {

   char *p;

   for (p = str; *p != '\0'; p++) {
      if (!isdigit(*p)) {
         return 0;
      }
   }

   return 1;
}

