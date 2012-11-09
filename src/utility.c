
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"


void displayRoom(Room *room) {

   printf("%s\n", dstrview(room->description));
}

