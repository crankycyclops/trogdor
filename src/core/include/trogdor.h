
#ifndef TROGDOR_H
#define TROGDOR_H


#include <glib.h>
#include <dstring.h>

/* out of memory errors happen often enough to write a macro */
#define PRINT_OUT_OF_MEMORY_ERROR {fprintf(stderr, "out of memory"); exit(EXIT_FAILURE);}


#endif

