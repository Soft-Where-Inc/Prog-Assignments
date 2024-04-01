/**
 * locations.c: Definitions
 *
 * History:
 *  3/2024  - Original version provided by Charles Baylis
 */
#include <stdio.h>
#include <stdint.h>

#include "locations.h"

#if __APPLE__
struct location loc_id_ref __attribute__((section("__DATA, loc_ids")));
#else
struct location loc_id_ref __attribute__((section("loc_ids")));
#endif  // __APPLE__

void print_loc(int id)
{
   struct location *loc = (struct location *)( ( (intptr_t)&loc_id_ref ) + id );
   printf("Location is in function '%s', %s:%d\n", loc->fn, loc->file, loc->line);
}
