
#include <string.h>
#include <dstring.h>
#include <libxml/xmlreader.h>

#define PARSEXML_C

#include "include/trogdor.h"
#include "include/parsexml.h"

/* returns true if the document was parsed successfully and false otherwise */
int parseGameFile(const char *filename);

/* opens the XML file and prepares it for parsing */
static xmlTextReaderPtr readXML(const char *filename);

/* returns the value of #text corresponding to an element */
static const char *getNodeValue(xmlTextReaderPtr reader);

/* make sure the specified closing tag is the next node */
static void checkClosingTag(const char *tag, xmlTextReaderPtr reader);

/* parse the objects section */
static void parseObjectSection(xmlTextReaderPtr reader);

/* parse the rooms section */
static void parseRoomSection(xmlTextReaderPtr reader);

/* parses an object */
static void parseObject(xmlTextReaderPtr reader);

/* parses a room */
static void parseRoom(xmlTextReaderPtr reader);


int main(int argc, char **argv) {

   parseGameFile("sample.xml");
   return 0;
}


int parseGameFile(const char *filename) {

   int parseStatus;
   xmlTextReaderPtr reader = readXML(filename);

   if (NULL != reader) {

      /* read each node from the XML file */
      while ((parseStatus = xmlTextReaderRead(reader)) > 0) {

         const xmlChar *name;
         const xmlChar *value;

         name = xmlTextReaderConstName(reader);

/* START DEBUG CODE
         value = xmlTextReaderValue(reader);


         printf("\n\n%d %d %s %d\n", xmlTextReaderDepth(reader),
            xmlTextReaderNodeType(reader), name,
            xmlTextReaderIsEmptyElement(reader));
         printf("Value: %s\n\n", value);

         continue;

END DEBUG CODE */

         if (NULL == name) {
            fprintf(stderr, "error parsing XML\n");
            return 0;
         }

         if (1 == xmlTextReaderDepth(reader)) {

            if (XML_COMMENT_NODE == xmlTextReaderNodeType(reader)) {
               continue;
            }

            else if (0 == strcmp("objects", name)) {
               parseObjectSection(reader);
            }

            // TODO: parse creatures

            else if (0 == strcmp("rooms", name)) {
               parseRoomSection(reader);
            }
         }
      }

      xmlFreeTextReader(reader);

      if (parseStatus < 0) {
         fprintf(stderr, "failed to parse XML\n");
         return 0;
      }
   }

   else {
      return 1;
   }
}


static void parseObjectSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      /* ignore XML comments */
      if (XML_COMMENT_NODE == xmlTextReaderNodeType(reader)) {
         continue;
      }

      else if (0 == strcmp("object", xmlTextReaderConstName(reader))) {
         parseObject(reader);
      }

      else {
         fprintf(stderr, "error: only <object> tags are allowed in <objects>\n");
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}


static void parseObject(xmlTextReaderPtr reader) {

   int parseStatus;        /* whether or not the parser could extract another node */
   int synonymCount = 0;   /* number of synonyms for an object */

   const char *objectName; /* name of the object */
   ObjectParsed *object;   /* struct to represent what we've parsed for the room */

   object = malloc(sizeof(ObjectParsed));
   if (NULL == object) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   object->name = NULL;
   object->description = NULL;
   object->synonyms = NULL;

   /* record the object's name */
   objectName = xmlTextReaderGetAttribute(reader, "name");

   if (NULL == objectName || 0 == strlen(objectName)) {
      fprintf(stderr, "Error: objects must be assigned a name\n");
      exit(EXIT_FAILURE);
   }

   if (DSTR_SUCCESS != dstralloc(&object->name)) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   cstrtodstr(object->name, objectName);

   /* parse all of the object's elements */
   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 2
   ) {
      int        tagtype  = xmlTextReaderNodeType(reader);
      const char *tagname = xmlTextReaderConstName(reader);

      /* ignore XML comment */
      if (XML_COMMENT_NODE == tagtype) {
         continue;
      }

      /* we're parsing the object's description */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("description", tagname)) {
         GET_XML_TAG(description, object)
      }

      /* we're parsing a synonym */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("synonym", tagname)) {

         dstring_t synonym = NULL;
         dstring_t *newSynonyms;

         if (DSTR_SUCCESS != dstralloc(&synonym)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }

         cstrtodstr(synonym, getNodeValue(reader));

         /* add the synonym to the array */
         synonymCount++;

         /* first synonym in the array */
         if (1 == synonymCount) {

            object->synonyms = malloc(sizeof(dstring_t *));

            if (NULL == object->synonyms) {
               PRINT_OUT_OF_MEMORY_ERROR;
            }
         }

         /* adding more synonyms to the array */
         else {

            newSynonyms = realloc(object->synonyms, synonymCount * sizeof(dstring_t *));

            if (NULL == newSynonyms) {
               PRINT_OUT_OF_MEMORY_ERROR;
            }

            object->synonyms = newSynonyms;
         }

         object->synonyms[synonymCount - 1] = synonym;

         /* make sure we have a valid closing tag */
         checkClosingTag("synonym", reader);
      }

      /* an unknown tag was found */
      else {
         fprintf(stderr, "Illegal tag <%s> found in object definition",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   /* make sure to NULL terminate our array of synonyms */
   if (synonymCount > 0) {

      dstring_t *synonymsObjects =
         realloc(object->synonyms, synonymCount * sizeof(dstring_t *));

      if (NULL == synonymsObjects) {
         PRINT_OUT_OF_MEMORY_ERROR;
      }

      object->synonyms = synonymsObjects;
   }
}


static void parseRoomSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      /* ignore XML comments */
      if (XML_COMMENT_NODE == xmlTextReaderNodeType(reader)) {
         continue;
      }

      else if (0 == strcmp("room", xmlTextReaderConstName(reader))) {
         parseRoom(reader);
      }

      else {
         fprintf(stderr, "error: only <room> tags are allowed in <rooms>\n");
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}


static void parseRoom(xmlTextReaderPtr reader) {

   int parseStatus;      /* whether or not the parser could extract another node */
   int objectCount = 0;  /* number of objects belonging to a room */

   const char *roomName; /* name of the room */
   RoomParsed *room;     /* struct to represent what we've parsed for the room */

   room = malloc(sizeof(RoomParsed));
   if (NULL == room) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   room->name = NULL;
   room->description = NULL;
   room->north = NULL;
   room->south = NULL;
   room->east = NULL;
   room->west = NULL;
   room->objects = NULL;

   /* record the room's name */
   roomName = xmlTextReaderGetAttribute(reader, "name");

   if (NULL == roomName || 0 == strlen(roomName)) {
      fprintf(stderr, "Error: rooms must be assigned a name\n");
      exit(EXIT_FAILURE);
   }

   if (DSTR_SUCCESS != dstralloc(&room->name)) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   cstrtodstr(room->name, roomName);

   /* parse all of the room's elements */
   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 2
   ) {
      int        tagtype  = xmlTextReaderNodeType(reader);
      const char *tagname = xmlTextReaderConstName(reader);

      /* ignore XML comment */
      if (XML_COMMENT_NODE == tagtype) {
         continue;
      }

      /* we're parsing the room's description */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("description", tagname)) {
         GET_XML_TAG(description, room)
      }

      /* get the room north of this one */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("north", tagname)) {
         GET_XML_TAG(north, room)
      }

      /* get the room south of this one */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("south", tagname)) {
         GET_XML_TAG(south, room)
      }

      /* get the room east of this one */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("east", tagname)) {
         GET_XML_TAG(east, room)
      }

      /* get the room west of this one */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("west", tagname)) {
         GET_XML_TAG(west, room)
      }

      /* an object gets placed in this room */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("object", tagname)) {

         dstring_t object = NULL;
         dstring_t *newObjects;

         if (DSTR_SUCCESS != dstralloc(&object)) {
            PRINT_OUT_OF_MEMORY_ERROR;
         }

         cstrtodstr(object, getNodeValue(reader));

         /* add the object to the array */
         objectCount++;

         /* first object in the array */
         if (1 == objectCount) {

            room->objects = malloc(sizeof(dstring_t *));

            if (NULL == room->objects) {
               PRINT_OUT_OF_MEMORY_ERROR;
            }
         }

         /* adding more objects to the array */
         else {

            newObjects = realloc(room->objects, objectCount * sizeof(dstring_t *));

            if (NULL == newObjects) {
               PRINT_OUT_OF_MEMORY_ERROR;
            }

            room->objects = newObjects;
         }

         room->objects[objectCount - 1] = object;

         /* make sure we have a valid closing tag */
         checkClosingTag("object", reader);
      }

      /* an unknown tag was found */
      else {
         fprintf(stderr, "Illegal tag <%s> found in room definition",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   /* make sure to NULL terminate our array of objects */
   if (objectCount > 0) {

      dstring_t *newObjects =
         realloc(room->objects, objectCount * sizeof(dstring_t *));

      if (NULL == newObjects) {
         PRINT_OUT_OF_MEMORY_ERROR;
      }

      room->objects = newObjects;
   }

   return;
}


static void checkClosingTag(const char *tag, xmlTextReaderPtr reader) {

   /* get the closing </description> tag */
   int parseStatus = xmlTextReaderRead(reader);

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
   0 != strcmp(tag, xmlTextReaderConstName(reader))) {
      fprintf(stderr, "missing closing </%s>\n", tag);
      exit(EXIT_FAILURE);
   }
}


static const char *getNodeValue(xmlTextReaderPtr reader) {

   /* get the corresponding #text node for the value */
   int parseStatus = xmlTextReaderRead(reader);

   if (parseStatus < 0) {
      fprintf(stderr, "There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   else if (0 != strcmp("#text", xmlTextReaderConstName(reader))) {
      fprintf(stderr, "<description> tag must be present and must contain a value\n");
      exit(EXIT_FAILURE);
   }

   return xmlTextReaderValue(reader);
}


static xmlTextReaderPtr readXML(const char *filename) {

   /* <sarcasm type="mad-at-lack-of-docs">it's magic!</sarcasm> */
   xmlTextReaderPtr reader = xmlReaderForFile(filename, NULL, XML_PARSE_NOBLANKS);

   if (reader != NULL) {
      return reader;
   }

   else {
      fprintf(stderr, "unable to open %s\n", filename);
      return NULL;
   }
}

