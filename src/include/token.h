#ifndef TOKEN_H
#define TOKEN_H


/* Call this first.  Initializes the tokenizer. */
extern void initTokenizer(const char *string);

/* Returns the next token or NULL if none left. */
extern char *getNextToken();

/* Push back a token so we can get it again.  Can only push back one at a
   time. */
extern void pushBackToken(char *token);


#endif

