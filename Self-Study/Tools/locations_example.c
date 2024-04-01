/**
 * A very simple demo, showing how a program could use CREATE_ID
 * to track where a structure was allocated.
 *
 * Build: gcc -o locations_example locations_example.c locations.c locations_file1.c
 * Usage: ./locations_example
 *
 * History:
 *  3/2024  - Original version provided by Charles Baylis
 */

#include <stdio.h>
#include <stdlib.h>

#include "locations.h"
#include "locations_example.h"

// Function prototypes
S * minion(void);

// allocate a struct S and set the alloc_id to match its allocation source
// position
S *
alloc_S(uint32_t alloc_location_id)
{
    S* result;
    result = malloc(sizeof(*result));
    result->alloc_id = alloc_location_id;
    printf("\n%s: alloc_id=%d\n", __LOC__, result->alloc_id);
    return result;
}

int
main()
{
  printf("Sizeof(struct location)=%lu bytes.\n", sizeof(struct location));

  // allocate a struct S
  S *my_s = alloc_S(CREATE_ID());

  // print the location where my_s was allocated
  print_loc(my_s->alloc_id);

  S *another_s = alloc_S(CREATE_ID());
  print_loc(another_s->alloc_id);

  S *minion_s = minion();

  S *new_s = another_extern_minion();
  print_loc(new_s->alloc_id);

  // Add a new allocation, to verify the logic in the dump program's
  // prSection_details(), which calculates expected number of entries.
  S *another_s_in_main = alloc_S(CREATE_ID());
  print_loc(another_s_in_main->alloc_id);

  // Cleanup
  free(another_s_in_main);
  free(new_s);
  free(minion_s);
  free(another_s);
  free(my_s);
  return 0;
}

S *
minion(void)
{
    S    *nested_s = alloc_S(CREATE_ID());
    print_loc(nested_s->alloc_id);
    return nested_s;
}
