
#ifndef PARSEXML_H
#define PARSEXML_H


#include "dstring.h"

typedef struct roomParsed {

   dstring_t name;
   dstring_t description;
   dstring_t north;
   dstring_t south;
   dstring_t east;
   dstring_t west;

   /* an array of object identifiers */
   dstring_t *objects;
   
} RoomParsed;

typedef struct objectParsed {

   dstring_t name;
   dstring_t description;

   /* an array of synonyms */
   dstring_t *synonyms;
} ObjectParsed;


#ifdef PARSEXML_C


/* this macro relies on a dstring_t object being defined with name X */
#define GET_XML_TAG(X, Y) \
\
   if (NULL != Y->X) { \
      fprintf(stderr, "Warning: \""#X"\" declared twice. Overwriting the first.\n"); \
   } \
\
   else { \
      if (DSTR_SUCCESS != dstrnalloc(&Y->X, 250)) { \
         PRINT_OUT_OF_MEMORY_ERROR; \
      } \
   } \
\
   cstrtodstr(Y->X, getNodeValue(reader)); \
   checkClosingTag(#X, reader);


#endif


#ifndef PARSEXML_C

/* returns true if the document was parsed successfully and false otherwise */
extern int parseGameFile(const char *filename);

#endif


#endif

