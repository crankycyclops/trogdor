
#include <string.h>
#include <libxml/xmlreader.h>

#define PARSEXML_C

#include "include/trogdor.h"
#include "include/construct.h"
#include "include/parsexml.h"
#include "include/data.h"
#include "include/utility.h"
#include "include/state.h"
#include "include/object.h"
#include "include/room.h"
#include "include/player.h"
#include "include/messages.h"


/* returns true if the document was parsed successfully and false otherwise */
int parseGameFile(const char *filename);

/* parse the global event handling section */
static void parseEventSection(xmlTextReaderPtr reader);

/* parse the player configuration section */
static void parsePlayerSection(xmlTextReaderPtr reader);

/* parse the player's inventory settings */
static void parseInventorySettings(xmlTextReaderPtr reader);

/* parse the inventory's weight (if setting exists) */
static void parseInventoryWeight(xmlTextReaderPtr reader);

/* opens the XML file and prepares it for parsing */
static xmlTextReaderPtr readXML(const char *filename);

/* returns the value of #text corresponding to an element */
static const char *getNodeValue(xmlTextReaderPtr reader);

/* make sure the specified closing tag is the next node */
static void checkClosingTag(const char *tag, xmlTextReaderPtr reader);

/* parse a script tag */
static dstring_t parseScriptTag(xmlTextReaderPtr reader);

/* parse an event tag */
static EventHandlerParsed *parseEventTag(xmlTextReaderPtr reader);

/* parse the objects section */
static void parseObjectSection(xmlTextReaderPtr reader);

/* parse the creatures section */
static void parseCreatureSection(xmlTextReaderPtr reader);

/* parse the rooms section */
static void parseRoomSection(xmlTextReaderPtr reader);

/* parses an object */
static void parseObject(xmlTextReaderPtr reader);

/* parses a creature */
static void parseCreature(xmlTextReaderPtr reader);

/* parses a room */
static void parseRoom(xmlTextReaderPtr reader);

/* parses the custom messages section */
static Messages parseMessages(xmlTextReaderPtr reader);

/* parses a custom message */
static const char *parseMessage(xmlTextReaderPtr reader);


/* list of invalid names for rooms, creatures, etc. */
char *invalidNames[] = {
   "room", "object", "player", "creature", NULL
};

/******************************************************************************/

int parseGameFile(const char *filename) {

   int parseStatus;
   xmlTextReaderPtr reader = readXML(filename);

   if (NULL != reader) {

      /* read each node from the XML file */
      while ((parseStatus = xmlTextReaderRead(reader)) > 0) {

         const xmlChar *name;
         const xmlChar *value;

         name = xmlTextReaderConstName(reader);

         if (NULL == name) {
            g_outputError("error parsing XML\n");
            return 0;
         }

         if (1 == xmlTextReaderDepth(reader)) {

            if (XML_COMMENT_NODE == xmlTextReaderNodeType(reader)) {
               continue;
            }

            else if (0 == strcmp("events", name)) {
               parseEventSection(reader);
            }

            else if (0 == strcmp("player", name)) {
               parsePlayerSection(reader);
            }

            else if (0 == strcmp("objects", name)) {
               parseObjectSection(reader);
            }

            else if (0 == strcmp("creatures", name)) {
               parseCreatureSection(reader);
            }

            else if (0 == strcmp("rooms", name)) {
               parseRoomSection(reader);
            }
         }
      }

      xmlFreeTextReader(reader);

      if (parseStatus < 0) {
         g_outputError("failed to parse XML\n");
         return 0;
      }

      return 1;
   }

   else {
      return 1;
   }
}

/******************************************************************************/

static void parseEventSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      if (XML_COMMENT_NODE == xmlTextReaderNodeType(reader)) {
         continue;
      }

      else if (0 == strcmp("script", xmlTextReaderConstName(reader))) {
         dstring_t scriptfile = parseScriptTag(reader);
         if (NULL != scriptfile) {
            globalScriptsParsed = g_list_append(globalScriptsParsed, scriptfile);
         }
      }

      else if (0 == strcmp("event", xmlTextReaderConstName(reader))) {
         EventHandlerParsed *handler = parseEventTag(reader);
         globalEventHandlersParsed = g_list_append(globalEventHandlersParsed,
            handler);
      }

      else {
         g_outputError("error: invalid <%s> tag in <events> section\n",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static dstring_t parseScriptTag(xmlTextReaderPtr reader) {

   dstring_t filename = NULL;

   /* get the script's filename */
   const char *src = xmlTextReaderGetAttribute(reader, "src");

   if (NULL != src) {
      filename = createDstring();
      cstrtodstr(filename, src);
   }

   // TODO: here's where we would parse the text inside a script tag
   checkClosingTag("script", reader);
   return filename;
}

/******************************************************************************/

static EventHandlerParsed *parseEventTag(xmlTextReaderPtr reader) {

   const char *eventName;

   dstring_t event = createDstring();
   dstring_t function = createDstring();

   EventHandlerParsed *handler = malloc(sizeof(EventHandlerParsed));
   if (NULL == handler) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   eventName = xmlTextReaderGetAttribute(reader, "name");
   if (NULL == eventName) {
      g_outputError("error: <event> tag requires name attribute\n");
      exit(EXIT_FAILURE);
   }

   cstrtodstr(event, eventName);
   cstrtodstr(function, getNodeValue(reader));
   dstrtrim(function);

   if (dstrlen(function) < 1) {
      g_outputError("error: <event> tag requires a function name\n");
      exit(EXIT_FAILURE);
   }

   handler->event = event;
   handler->function = function;

   checkClosingTag("event", reader);
   return handler;
}

/******************************************************************************/

static void parsePlayerSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      IF_COMMENT_IGNORE

      else if (0 == strcmp("inventory", xmlTextReaderConstName(reader))) {
         parseInventorySettings(reader);
      }

      else {
         g_outputError("error: invalid <%s> tag in <player> section\n",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static void parseInventorySettings(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 2
   ) {

      IF_COMMENT_IGNORE

      else if (0 == strcmp("weight", xmlTextReaderConstName(reader))) {
         parseInventoryWeight(reader);
      }

      else {
         g_outputError("error: invalid <%s> tag in <inventory>\n",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static void parseInventoryWeight(xmlTextReaderPtr reader) {

   char *weight = (char *)getNodeValue(reader);

   if (isInt(weight)) {
      g_playerConfig.inventory.maxWeight = atoi(weight);
   }

   else {
      g_outputError("error: inventory weight must be a valid integer "
         ">= 0\n");
   }

   checkClosingTag("weight", reader);
}

/******************************************************************************/

static void parseObjectSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      IF_COMMENT_IGNORE

      else if (0 == strcmp("object", xmlTextReaderConstName(reader))) {
         parseObject(reader);
      }

      else {
         g_outputError("error: only <object> tags are allowed in <objects>\n");
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static void parseObject(xmlTextReaderPtr reader) {

   int i;
   int parseStatus;        /* whether or not the parser could extract another node */
   int synonymCount = 0;   /* number of synonyms for an object */

   const char *objectName; /* name of the object */
   ObjectParsed *object;   /* struct to represent what we've parsed for the room */

   object = malloc(sizeof(ObjectParsed));
   if (NULL == object) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   object->name = createDstring();
   object->description = NULL;
   object->weight = createDstring();
   object->takeable = createDstring();
   object->droppable = createDstring();
   object->messages = NULL;
   object->used = 0;

   /* by default, an object has no weight */
   cstrtodstr(object->weight, "0");

   /* by default, an object is takeable */
   cstrtodstr(object->takeable, "1");

   /* by default, an object is droppable */
   cstrtodstr(object->droppable, "1");

   object->synonyms = g_array_sized_new(FALSE, FALSE, sizeof(dstring_t), 5);
   object->scripts = NULL;

   /* record the object's name */
   objectName = xmlTextReaderGetAttribute(reader, "name");

   if (NULL == objectName || 0 == strlen(objectName)) {
      g_outputError("Error: objects must be assigned a unique name\n");
      exit(EXIT_FAILURE);
   }

   /* makes sure object's name is valid */
   for (i = 0; invalidNames[i] != NULL; i++) {
      if (0 == strcmp(invalidNames[i], objectName)) {
         g_outputError("error: '%s' is an invalid object name\n",
            invalidNames[i]);
         exit(EXIT_FAILURE);
      }
   }

   /* make sure the object doesn't already exist */
   if (NULL != g_hash_table_lookup(objectParsedTable, objectName)) {
      g_outputError("object '%s' must be unique\n", objectName);
      exit(EXIT_FAILURE);
   }

   if (0 == strcmp("room", objectName)) {
      g_outputError("error: 'room' is an invalid object name\n");
      exit(EXIT_FAILURE);
   }

   if (0 == strcmp("creature", objectName)) {
      g_outputError("error: 'creature' is an invalid object name\n");
      exit(EXIT_FAILURE);
   }

   cstrtodstr(object->name, objectName);
   dstrtrim(object->name);
   dstrtolower(object->name, 0);

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

      /* we're parsing the object's weight (optional) */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("weight", tagname)) {
         GET_XML_TAG(weight, object)
         dstrtrim(object->weight);
         if (!isInt((char *)dstrview(object->weight))) {
            g_outputError("error: %s is not a valid weight\n",
               dstrview(object->weight));
            exit(EXIT_FAILURE);
         }
      }

      /* we're parsing the object's "takeability" (optional) */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("takeable", tagname)) {
         GET_XML_TAG(takeable, object)
         dstrtrim(object->takeable);
         if (strcmp(dstrview(object->takeable), "0") &&
         strcmp(dstrview(object->takeable), "1")) {
            g_outputError("error: takeable should be set to 1 (true) or 0 (false)\n");
            exit(EXIT_FAILURE);
         }
      }

      /* we're parsing the object's "droppability" (optional) */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("droppable", tagname)) {
         GET_XML_TAG(droppable, object)
         dstrtrim(object->droppable);
         if (0 != strcmp(dstrview(object->droppable), "0") &&
         0 != strcmp(dstrview(object->droppable), "1")) {
            g_outputError("error: droppable should be set to 1 (true) or 0 (false)\n");
            exit(EXIT_FAILURE);
         }
      }

      /* we're parsing a synonym */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("synonym", tagname)) {

         dstring_t synonym = createDstring();

         /* add to the object's list of synonyms */
         cstrtodstr(synonym, getNodeValue(reader));
         dstrtrim(synonym);
         dstrtolower(synonym, 0);
         g_array_append_val(object->synonyms, synonym);

         /* 'room' is an invalid synonym */
         if (0 == strcmp("room", dstrview(synonym))) {
            g_outputError("error: 'room' is an invalid synonym\n");
            exit(EXIT_FAILURE);
         }

         /* make sure we have a valid closing tag */
         checkClosingTag("synonym", reader);
      }

      /* we're parsing a script tag */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("script", tagname)) {

         dstring_t file = createDstring();

         cstrtodstr(file, getNodeValue(reader));
         if (0 == dstrlen(file)) {
            g_outputError("error: <script> has blank value!\n");
         }

         object->scripts = g_list_append(object->scripts, file);
         checkClosingTag("script", reader);
      }

      /* parse custom messages */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("messages", tagname)) {
         object->messages = parseMessages(reader);
      }

      /* an unknown tag was found */
      else {
         g_outputError("Illegal tag <%s> found in object definition",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   /* add the object to the objects parsed table for lookup later */
   g_hash_table_insert(objectParsedTable, (gpointer)dstrview(object->name),
      object);

   return;
}

/******************************************************************************/

static void parseCreatureSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      IF_COMMENT_IGNORE

      else if (0 == strcmp("creature", xmlTextReaderConstName(reader))) {
         parseCreature(reader);
      }

      else {
         g_outputError("error: only <creature> tags are allowed in <creatures>\n");
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static void parseCreature(xmlTextReaderPtr reader) {

   int i;
   int parseStatus;          /* whether the parser could extract another node */

   const char *creatureName; /* name of the creature */
   CreatureParsed *creature; /* struct to represent what we've parsed for the room */

   creature = malloc(sizeof(CreatureParsed));
   if (NULL == creature) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   creature->name = createDstring();
   creature->title = NULL;
   creature->description = NULL;
   creature->messages = NULL;
   creature->used = 0;

   /* by default, a creature is neutral */
   creature->allegiance = NULL;

   creature->objects = g_array_sized_new(FALSE, FALSE, sizeof(dstring_t), 2);
   creature->scripts = NULL;

   /* record the creature's name */
   creatureName = xmlTextReaderGetAttribute(reader, "name");

   if (NULL == creatureName || 0 == strlen(creatureName)) {
      g_outputError("Error: creatures must be assigned a unique name\n");
      exit(EXIT_FAILURE);
   }

   /* makes sure creature's name is valid */
   for (i = 0; invalidNames[i] != NULL; i++) {
      if (0 == strcmp(invalidNames[i], creatureName)) {
         g_outputError("error: '%s' is an invalid creature name\n",
            invalidNames[i]);
         exit(EXIT_FAILURE);
      }
   }

   /* make sure the creature doesn't already exist */
   if (NULL != g_hash_table_lookup(creatureParsedTable, creatureName)) {
      g_outputError("creature '%s' must be unique\n", creatureName);
      exit(EXIT_FAILURE);
   }

   cstrtodstr(creature->name, creatureName);
   dstrtrim(creature->name);
   dstrtolower(creature->name, 0);

   /* parse all of the creature's elements */
   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 2
   ) {
      int        tagtype  = xmlTextReaderNodeType(reader);
      const char *tagname = xmlTextReaderConstName(reader);

      /* ignore XML comment */
      if (XML_COMMENT_NODE == tagtype) {
         continue;
      }

      /* we're parsing the creature's title */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("title", tagname)) {
         GET_XML_TAG(title, creature)
      }

      /* we're parsing the creature's description */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("description", tagname)) {
         GET_XML_TAG(description, creature)
      }

      /* we're parsing the creature's allegiance */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("allegiance", tagname)) {
         GET_XML_TAG(allegiance, creature)
         dstrtrim(creature->allegiance);
         dstrtolower(creature->allegiance, 0);
      }

      /* we're parsing an object owned by the creature */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("object", tagname)) {

         const char *objectName = getNodeValue(reader);
         dstring_t object = createDstring();

         /* make sure the object exists */
         if (NULL == g_hash_table_lookup(objectParsedTable, objectName)) {
            g_outputError("object '%s' belonging to creature '%s' doesn't exist\n",
               objectName, creatureName);
            exit(EXIT_FAILURE);
         }

         /* add object to the creature */
         cstrtodstr(object, objectName);
         dstrtrim(object);
         g_array_append_val(creature->objects, object);

         /* make sure we have a valid closing tag */
         checkClosingTag("object", reader);
      }

      /* we're parsing a script tag */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("script", tagname)) {

         dstring_t file = createDstring();

         cstrtodstr(file, getNodeValue(reader));
         if (0 == dstrlen(file)) {
            g_outputError("error: <script> has blank value!\n");
         }

         creature->scripts = g_list_append(creature->scripts, file);
         checkClosingTag("script", reader);
      }

      /* parse custom messages */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("messages", tagname)) {
         creature->messages = parseMessages(reader);
      }

      /* an unknown tag was found */
      else {
         g_outputError("Illegal tag <%s> found in creature definition",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   /* add the creature to the creatures parsed table for lookup later */
   g_hash_table_insert(creatureParsedTable, (gpointer)dstrview(creature->name),
      creature);

   return;
}

/******************************************************************************/

static void parseRoomSection(xmlTextReaderPtr reader) {

   int parseStatus;

   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 1
   ) {

      IF_COMMENT_IGNORE

      else if (0 == strcmp("room", xmlTextReaderConstName(reader))) {
         parseRoom(reader);
      }

      else {
         g_outputError("error: only <room> tags are allowed in <rooms>\n");
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return;
}

/******************************************************************************/

static void parseRoom(xmlTextReaderPtr reader) {

   int i;
   int parseStatus;      /* whether or not the parser could extract another node */

   const char *roomName; /* name of the room */
   RoomParsed *room;     /* struct to represent what we've parsed for the room */

   room = malloc(sizeof(RoomParsed));
   if (NULL == room) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   room->name = createDstring();
   room->title = NULL;
   room->description = NULL;
   room->north = NULL;
   room->south = NULL;
   room->east = NULL;
   room->west = NULL;
   room->in = NULL;
   room->out = NULL;
   room->up = NULL;
   room->down = NULL;
   room->messages = NULL;
   room->objects = g_array_sized_new(FALSE, FALSE, sizeof(dstring_t *), 5);
   room->creatures = g_array_sized_new(FALSE, FALSE, sizeof(dstring_t *), 5);
   room->scripts = NULL;
   room->eventHandlers = NULL;

   /* record the room's name */
   roomName = xmlTextReaderGetAttribute(reader, "name");

   if (NULL == roomName || 0 == strlen(roomName)) {
      g_outputError("Error: rooms must be assigned a name\n");
      exit(EXIT_FAILURE);
   }

   /* makes sure room's name is valid */
   for (i = 0; invalidNames[i] != NULL; i++) {
      if (0 == strcmp(invalidNames[i], roomName)) {
         g_outputError("error: '%s' is an invalid room name\n",
            invalidNames[i]);
         exit(EXIT_FAILURE);
      }
   }

   /* make sure the room doesn't already exist */
   if (NULL != g_hash_table_lookup(roomParsedTable, roomName)) {
      g_outputError("room '%s' must be unique\n", roomName);
      exit(EXIT_FAILURE);
   }

   cstrtodstr(room->name, roomName);
   dstrtrim(room->name);
   dstrtolower(room->name, 0);

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

      /* we're parsing the room's title */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("title", tagname)) {
         GET_XML_TAG(title, room)
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

      /* get the room we go to by going "in" */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("in", tagname)) {
         GET_XML_TAG(in, room)
      }

      /* get the room we go to by going "out" */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("out", tagname)) {
         GET_XML_TAG(out, room)
      }

      /* get the room we go to by going "up" */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("up", tagname)) {
         GET_XML_TAG(up, room)
      }

      /* get the room we go to by going "down" */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("down", tagname)) {
         GET_XML_TAG(down, room)
      }

      /* an object gets placed in this room */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("object", tagname)) {

         const char *objectName = getNodeValue(reader);
         dstring_t object = createDstring();

         /* make sure the object exists */
         if (NULL == g_hash_table_lookup(objectParsedTable, objectName)) {
            g_outputError("object '%s' in room '%s' doesn't exist\n",
               objectName, roomName);
            exit(EXIT_FAILURE);
         }

         /* add object to the room */
         cstrtodstr(object, objectName);
         dstrtrim(object);
         g_array_append_val(room->objects, object);

         /* make sure we have a valid closing tag */
         checkClosingTag("object", reader);
      }

      /* creature gets placed in this room */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("creature", tagname)) {

         const char *creatureName = getNodeValue(reader);
         dstring_t creature = createDstring();

         /* make sure the creature exists */
         if (NULL == g_hash_table_lookup(creatureParsedTable, creatureName)) {
            g_outputError("creature '%s' in room '%s' doesn't exist\n",
               creatureName, roomName);
            exit(EXIT_FAILURE);
         }

         /* add creature to the room */
         cstrtodstr(creature, creatureName);
         dstrtrim(creature);
         g_array_append_val(room->creatures, creature);

         /* make sure we have a valid closing tag */
         checkClosingTag("creature", reader);
      }

      /* parse custom messages */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("messages", tagname)) {
         room->messages = parseMessages(reader);
      }

      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("script", tagname)) {
         dstring_t scriptfile = parseScriptTag(reader);
         if (NULL != scriptfile) {
            room->scripts = g_list_append(room->scripts, scriptfile);
         }
      }

      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("event", tagname)) {
         EventHandlerParsed *handler = parseEventTag(reader);
         room->eventHandlers = g_list_append(room->eventHandlers, handler);
      }

      /* an unknown tag was found */
      else {
         g_outputError("Illegal tag <%s> found in room definition",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   /* title element is required */
   else if (NULL == room->title) {
      g_outputError("Room \"%s\" is missing required <title> element\n",
         dstrview(room->name));
      exit(EXIT_FAILURE);
   }

   /* description element is required */
   else if (NULL == room->description) {
      g_outputError("Room \"%s\" is missing required <description> element\n",
         dstrview(room->name));
      exit(EXIT_FAILURE);
   }

   /* add the room to the rooms parsed lookup table */
   g_hash_table_insert(roomParsedTable, (gpointer)dstrview(room->name), room);

   return;
}

/******************************************************************************/

static Messages parseMessages(xmlTextReaderPtr reader) {

   int i;
   int parseStatus;   /* whether or not the parser could extract another node */
   const char *name;  /* message's name (hash key) */

   Messages msgs = createMessages();

   /* parse all messages */
   while ((parseStatus = xmlTextReaderRead(reader)) > 0 &&
   xmlTextReaderDepth(reader) > 3
   ) {
      int        tagtype  = xmlTextReaderNodeType(reader);
      const char *tagname = xmlTextReaderConstName(reader);

      /* ignore XML comment */
      if (XML_COMMENT_NODE == tagtype) {
         continue;
      }

      /* we're parsing a message */
      else if (XML_ELEMENT_NODE == tagtype && 0 == strcmp("message", tagname)) {

         const char *message;

         name = xmlTextReaderGetAttribute(reader, "name");
         message = parseMessage(reader);
         setMessage(msgs, name, message);
         checkClosingTag("message", reader);
      }

      else {
         g_outputError("Illegal tag <%s> found in messages section",
            xmlTextReaderConstName(reader));
         exit(EXIT_FAILURE);
      }
   }

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   return msgs;
}

/******************************************************************************/

static const char *parseMessage(xmlTextReaderPtr reader) {

   return getNodeValue(reader);
}

/******************************************************************************/

static void checkClosingTag(const char *tag, xmlTextReaderPtr reader) {

   /* get the closing </description> tag */
   int parseStatus = xmlTextReaderRead(reader);

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
   0 != strcmp(tag, xmlTextReaderConstName(reader))) {
      g_outputError("missing closing </%s>\n", tag);
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

static const char *getNodeValue(xmlTextReaderPtr reader) {

   /* get the corresponding #text node for the value */
   int parseStatus = xmlTextReaderRead(reader);

   if (parseStatus < 0) {
      g_outputError("There was an error parsing game XML file\n");
      exit(EXIT_FAILURE);
   }

   else if (0 != strcmp("#text", xmlTextReaderConstName(reader))) {
      g_outputError("<description> tag must be present and must contain a value\n");
      exit(EXIT_FAILURE);
   }

   return xmlTextReaderValue(reader);
}

/******************************************************************************/

static xmlTextReaderPtr readXML(const char *filename) {

   /* <sarcasm type="mad-at-lack-of-docs">it's magic!</sarcasm> */
   xmlTextReaderPtr reader = xmlReaderForFile(filename, NULL, XML_PARSE_NOBLANKS);

   if (reader != NULL) {
      return reader;
   }

   else {
      g_outputError("unable to open %s\n", filename);
      return NULL;
   }
}

