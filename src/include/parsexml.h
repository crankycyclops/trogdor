
#ifndef PARSEXML_H
#define PARSEXML_H


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


#ifdef PARSEXML_C


/* this macro relies on a dstring_t object being defined with name X */
#define GET_XML_TAG(X) \
\
   if (NULL != room->X) { \
      fprintf(stderr, "Warning: \""#X"\" declared twice. Overwriting the first.\n"); \
   } \
\
   else { \
      if (DSTR_SUCCESS != dstrnalloc(&room->X, 250)) { \
         PRINT_OUT_OF_MEMORY_ERROR; \
      } \
   } \
\
   cstrtodstr(room->X, getNodeValue(reader)); \
   checkClosingTag(#X, reader);


#endif


#ifndef PARSEXML_C

// TODO

#endif


#endif

