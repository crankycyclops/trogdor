
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/room.h"
#include "include/state.h"
#include "include/shell.h"
#include "include/token.h"
#include "include/action.h"
#include "include/vocabulary.h"

#define COMMAND_C

#include "include/command.h"

/* initializes the dstring_t objects inside of the command struct */
static void initCommand(Command *command);

/* frees memory used by the command object */
static void destroyCommand(Command *command);

/* executes a user input command */
void executeCommand();

/* debugging function */
static void printCommand(Command command);

/* main parse function that breaks a sentence down into its command parts */
static Command parseCommand(dstring_t sentence);

/* extracts a direct object from a sentence */
static int parseDirectObject(Command *command);

/* extracts an indirect object from a sentence */
static int parseIndirectObject(Command *command);

/* returns true if the specified word is a recognized preposition */
static int isPreposition(const char *word);

/* returns true if the specified word is a filler word */
static int isFillerWord(const char *word);

/******************************************************************************/

static void initCommand(Command *command) {

      command->verb = NULL;
      command->directObject = NULL;
      command->preposition = NULL;
      command->indirectObject = NULL;
      command->error = 0;

      return;
}

/******************************************************************************/

static void destroyCommand(Command *command) {

      if (NULL != command->verb) {
         if (DSTR_SUCCESS != dstrfree(&command->verb)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      if (NULL != command->directObject) {
         if (DSTR_SUCCESS != dstrfree(&command->directObject)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      if (NULL != command->preposition) {
         if (DSTR_SUCCESS != dstrfree(&command->preposition)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      if (NULL != command->indirectObject) {
         if (DSTR_SUCCESS != dstrfree(&command->indirectObject)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      return;
}

/******************************************************************************/

void executeCommand() {

   Command command;
   dstring_t commandStr = NULL;

   /* keep reading from the shell until we get something useful */
   do {
      commandStr = readCommand();
   } while (0 == dstrlen(commandStr));

   command = parseCommand(commandStr);

   if (!callAction(command)) {
      printf("Sorry, I don't understand you.\n");
   }

   // TODO: This should work and is a memory leak!
   // dstrfree(commandStr);
   destroyCommand(&command);

   return;
}

/******************************************************************************/

static Command parseCommand(dstring_t sentence) {

   Command  command;
   char     *token;

   initCommand(&command);

   initTokenizer(dstrview(sentence));
   token = getNextToken();

   // the user pressed enter without issuing a command, so ignore it
   if (!token) {
      return command;
   }

   /* the first token will always be considered the "verb" */
   if (DSTR_SUCCESS != dstralloc(&command.verb)) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }
   
   cstrtodstr(command.verb, token);

   /* read ahead */
   token = getNextToken();
   pushBackToken(token);

   /* we may have a direct and/or indirect object to look at */
   if (NULL != token) {

      int status;

      status  = parseDirectObject(&command);
      status += parseIndirectObject(&command);

      /* if we detected additional characters, but don't have a direct object
         or indirect object, then we have a syntax error */
      if (!status) {
         command.error = 1;
         return command;
      }
   }

   /* we're done parsing! */
   return command;
}

/******************************************************************************/

static int parseDirectObject(Command *command) {

   char *token = getNextToken();

   while (NULL != token && !isPreposition(token)) {

      // ignore filler words such as "the"
      if (isFillerWord(token)) {
         token = getNextToken();
         continue;
      }

      /* initialize the string if we haven't done so yet */
      if (NULL == command->directObject) {
         if (DSTR_SUCCESS != dstralloc(&command->directObject)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      if (dstrlen(command->directObject) > 0) {
         dstrcatcs(command->directObject, " ");
      }

      dstrcatcs(command->directObject, token);

      token = getNextToken();
   }

   /* we have a preposition to push back onto the token stream */
   if (NULL != token) {
      pushBackToken(token);
   }

   /* return true if a direct object was found */
   return dstrlen(command->directObject) > 0 ? 1 : 0;
}

/******************************************************************************/

static int parseIndirectObject(Command *command) {

   char *token = getNextToken();

   /* an indirect object must be preceded by a preposition */
   if (NULL == token || !isPreposition(token)) {
      pushBackToken(token);
      return 0;
   }

   if (DSTR_SUCCESS != dstralloc(&command->preposition)) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   dstrcatcs(command->preposition, token);

   /* anything after the preposition will be counted as part of the IDO */
   token = getNextToken();

   while (NULL != token) {

      // ignore filler words such as "the"
      if (isFillerWord(token)) {
         token = getNextToken();
         continue;
      }

      /* initialize the string if we haven't done so yet */
      if (NULL == command->indirectObject) {
         if (DSTR_SUCCESS != dstralloc(&command->indirectObject)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }
      }

      if (dstrlen(command->indirectObject) > 0) {
         dstrcatcs(command->indirectObject, " ");
      }

      dstrcatcs(command->indirectObject, token);

      token = getNextToken();
   }

   /* no indirect object was parsed */
   if (0 == dstrlen(command->indirectObject)) {

      /* a dangling preposition is considered a syntax error */
      if (dstrlen(command->preposition) > 0) {
         command->error = 1;
      }

      return 0;
   }

   // we found a preposition + indirect object
   else {
      return 1;
   }
}

/******************************************************************************/

static int isFillerWord(const char *word) {

   int i;

   for (i = 0; fillerWords[i] != NULL; i++) {
      if (0 == strcmp(word, fillerWords[i])) {
         return 1;
      }
   }

   return 0;
}

/******************************************************************************/

static int isPreposition(const char *word) {

   int i;

   for (i = 0; prepositions[i] != NULL; i++) {
      if (0 == strcmp(word, prepositions[i])) {
         return 1;
      }
   }

   return 0;
}

/******************************************************************************/

static void printCommand(Command command) {

   printf("\n\nVerb: %s\n", NULL == command.verb ? "NULL" : dstrview(command.verb));
   printf("DO: %s\n", NULL == command.directObject ? "NULL" : dstrview(command.directObject));
   printf("Prep: %s\n", NULL == command.preposition ? "NULL" : dstrview(command.preposition));
   printf("IDO: %s\n", NULL == command.indirectObject ? "NULL" : dstrview(command.indirectObject));
   printf("Error: %d\n\n", command.error);

   return;
}

