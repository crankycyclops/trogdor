
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>


dstring_t readCommand() {

   dstring_t command;

   if (DSTR_SUCCESS != dstrnalloc(&command, 250)) {
      fprintf(stderr, "out of memory\n");
      exit(EXIT_FAILURE);
   }

   printf("> ");
   dstreadl(command);

   // trim surrounding whitespace and convert to all lower case
   dstrtrim(command);
   dstrtolower(command, 0);

   return command;
}

