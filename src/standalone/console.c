
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>


int consoleOut(const char *format, ...) {

   int status;
   va_list args;

   va_start(args, format);
   status = vprintf(format, args);
   va_end(args);

   return status;
}

/******************************************************************************/

int consoleError(const char *format, ...) {

   int status;
   va_list args;

   va_start(args, format);
   status = vfprintf(stderr, format, args);
   va_end(args);

   return status;
}

/******************************************************************************/

dstring_t consoleIn() {

   dstring_t command;

   if (DSTR_SUCCESS != dstrnalloc(&command, 250)) {
      fprintf(stderr, "out of memory\n");
      exit(EXIT_FAILURE);
   }

   printf("\n> ");
   dstreadl(command);

   // trim surrounding whitespace and convert to all lower case
   dstrtrim(command);
   dstrtolower(command, 0);

   return command;
}

