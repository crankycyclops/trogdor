
#include <stdlib.h>
#include <ctype.h>

#include "include/trogdor.h"

/* returns TRUE if a string is an integer and FALSE if it's not */
int isInt(char *str);

/* allocate a new dstring_t object */
dstring_t createDstring();

/******************************************************************************/

dstring_t createDstring() {

   dstring_t newstr;

   if (DSTR_SUCCESS != dstralloc(&newstr)) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   return newstr;
}

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

