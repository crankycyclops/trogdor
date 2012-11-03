
#include <string.h>

/* Call this first.  Initializes the tokenizer. */
void initTokenizer(const char *string);

/* Returns the next token or NULL if none left. */
char *getNextToken();

/* Push back a token so we can get it again.  Can only push back one at a
   time. */
void pushBackToken(const char *token);


/* This is where tokens go when they are "pushed back."  We can only push back
   one token at a time */
static char *undeleted;

static char *curToken;


void initTokenizer(const char *string) {

   curToken = strtok(string, " \t");
}


char *getNextToken() {

   char *token = curToken;

   if (NULL != undeleted) {
      token = undeleted;
      undeleted = NULL;
   }

   else {
      curToken = strtok(NULL, " \t");
   }

   return token;
}


void pushBackToken(const char *token) {

   undeleted = token;
   return;
}

