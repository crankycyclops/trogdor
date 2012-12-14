
#define MESSAGES_C

#include <glib.h>
#include <dstring.h>

#include "include/utility.h"
#include "include/messages.h"


/* allocates memory for a hash table of messages indexed by name */
Messages createMessages();

/* frees memory associated with a hash table of messages */
void destroyMessages(Messages messages);

/* returns a string corresponding to the specified name or NULL if none */
dstring_t getMessage(Messages messages, const char *name);

/* prints the message corresponding to the specified name or nothing if none */
void displayMessage(Messages messages, const char *name);

/* Sets a new message or updates an existing message.  You don't have to worry
   about memory allocation; this function will allocate and manage a dstring_t
   object for you.  Setting message to NULL will clear it. */   
void setMessage(Messages messages, const char *name, const char *message);

/******************************************************************************/

Messages createMessages() {

   return g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

void destroyMessages(Messages messages) {

   g_hash_table_destroy(messages);
   return;
}

/******************************************************************************/

dstring_t getMessage(Messages messages, const char *name) {

   return g_hash_table_lookup(messages, name);
}

/******************************************************************************/

void displayMessage(Messages messages, const char *name) {

   dstring_t message = g_hash_table_lookup(messages, name);

   if (NULL != message) {
      g_outputString("%s\n", dstrview(message));
   }

   return;
}

/******************************************************************************/

void setMessage(Messages messages, const char *name, const char *message) {

   dstring_t newMessage;

   /* we're clearing an existing message */
   if (NULL == message) {

      dstring_t oldMessage = g_hash_table_lookup(messages, name);

      if (NULL != oldMessage) {
         dstrfree(&oldMessage);
         g_hash_table_remove(messages, name);
      }

      return;
   }

   newMessage = g_hash_table_lookup(messages, name);

   /* replacing an existing message */
   if (NULL != newMessage) {
      cstrtodstr(newMessage, message);
      return;
   }

   /* adding a new message */
   else {
      newMessage = createDstring();
      cstrtodstr(newMessage, message);
      g_hash_table_insert(messages, (char *)name, newMessage);
      return;
   }
}

