// a very simple demo, showing how a program could use CREATE_ID
// to track where a structure was allocated

#include <stdio.h>
#include <stdlib.h>

#include "locations.h"

struct S
{
    int data[16];
    int alloc_id;
};

// allocate a struct S and set the alloc_id to match its allocation source
// position
struct S* alloc_S(int alloc_location_id)
{
    struct S* result;
    result = malloc(sizeof(*result));
    result->alloc_id = alloc_location_id;
    return result;
}

int main()
{
  // allocate a struct S
  struct S *my_s = alloc_S(CREATE_ID());

  // print the location where my_s was allocated
  print_loc(my_s->alloc_id);

  return 0;
}
