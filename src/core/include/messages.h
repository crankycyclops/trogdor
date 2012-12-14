#ifndef MESSAGES_H
#define MESSAGES_H


#include <glib.h>
#include <dstring.h>

/* this might be some other kind of structure in the future */
typedef GHashTable * Messages;


#ifndef MESSAGES_C

/* allocates memory for a hash table of messages indexed by name */
extern Messages createMessages();

/* frees memory associated with a hash table of messages */
extern void destroyMessages(Messages messages);

/* returns a string corresponding to the specified name or NULL if none */
extern dstring_t getMessage(Messages messages, const char *name);

/* prints the message corresponding to the specified name or nothing if none */
extern void displayMessage(Messages messages, const char *name);

/* Sets a new message or updates an existing message.  You don't have to worry
   about memory allocation; this function will allocate and manage a dstring_t
   object for you.  Setting message to NULL will clear it. */   
extern void setMessage(Messages messages, const char *name, const char *message);

#endif


#endif

