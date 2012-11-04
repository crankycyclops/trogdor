
#ifndef PARSEXML_H
#define PARSEXML_H


#include "dstring.h"

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
   dstrtrim(Y->X); \
   checkClosingTag(#X, reader);


#endif


#ifndef PARSEXML_C

/* returns true if the document was parsed successfully and false otherwise */
extern int parseGameFile(const char *filename);

#endif


#endif

