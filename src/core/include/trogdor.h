
#ifndef TROGDOR_H
#define TROGDOR_H


#include <pthread.h>
#include <glib.h>
#include <dstring.h>

/* used to identify entities referenced by a void pointer */
enum EntityType {entity_room, entity_player, entity_creature, entity_object};

typedef struct {
   void *entity;
   enum EntityType type;
} Entity;


/* out of memory errors happen often enough to write a macro */
#define PRINT_OUT_OF_MEMORY_ERROR {g_outputError("out of memory"); exit(EXIT_FAILURE);}


#endif

