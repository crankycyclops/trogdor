
#include <stdlib.h>
#include <ctype.h>

#include "include/trogdor.h"

/* returns TRUE if a string is an integer and FALSE if it's not */
int isInt(char *str);

/* returns TRUE if a string is a float and FALSE if it's not */
int isDouble(char *str);

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

/******************************************************************************/

int isDouble(char *str) {

   char *p;
   int digit = 0;   /* set to 1 when the first digit is encountered */
   int decimal = 0; /* set to 1 when a decimal is encountered */


   for (p = str; *p != '\0'; p++) {

      if (!isdigit(*p)) {

         /* first character can be an optional sign */
         if (('+' == *p || '-' == *p) && p == str) {
            continue;
         }

         /* we encountered a decimal point */
         else if (*p == '.' && digit && !decimal) {
            decimal = 1;
            continue;
         }

         /* GAH!  Evil invalid character!  Red alert!  Danger Will Robinson! */
         return 0;
      }

      else {
         digit = 1;
      }
   }

   return 1;
}

