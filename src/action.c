
#include <stdio.h>

#include "include/trogdor.h"
#include "include/state.h"

#define NORTH 1
#define SOUTH 2
#define EAST  3
#define WEST  4

/* moves the user in the specified direction */
void move(int direction);


void move(int direction) {

   switch (direction) {

      case NORTH:

         if (NULL != location->north) {
            location = location->north;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         break;

      case SOUTH:

         if (NULL != location->south) {
            location = location->south;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         break;

      case EAST:

         if (NULL != location->east) {
            location = location->east;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         break;

      case WEST:

         if (NULL != location->west) {
            location = location->west;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         break;

      default:
         break;
   }

   return;
}

