
#include <stdio.h>
#include <stdint.h>

#include "locations.h"

struct location loc_id_ref __attribute__((section("loc_ids")));

void print_loc(int id)
{
   struct location *loc = (struct location *)(((intptr_t)&loc_id_ref) + id);
   printf("Location is in function '%s', %s:%d\n", loc->fn, loc->file, loc->line);
}
