#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* converts a string to all lowercase letters */
char *strtolower(char *const str);

/* trims all whitespace from the left of a string */
char *ltrim(char *const str);

/* trims all whitespace from the right of a string */
char *rtrim(char *const str);


char *strtolower(char *const str) {

   int i;

   for(i = 0; str[i]; i++) {
      str[i] = tolower(str[i]);
   }

   return str;
}


char *ltrim(char *const str) {

   char *p;

   for (p = str; *p != '\0' && isspace(*p); p++);
   memmove(str, p, strlen(p) + 1);

   return str;
}


char *rtrim(char *const str) {

   char *p;

   for (p = str + strlen(str) - 1; p >= str && isspace(*p); p--);

   p++;
   *p = '\0';

   return str;
}

