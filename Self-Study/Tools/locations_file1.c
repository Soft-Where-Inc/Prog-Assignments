/**
 * A different compilation unit, built along with locations_example.c
 * to track where a structure was allocated.
 *
 * History:
 *  3/2024  - Developed on top of version provided by Charles Baylis
 */

#include <stdio.h>
#include <stdlib.h>

#include "locations.h"
#include "locations_example.h"

S *
another_extern_minion(void)
{
    S * nested_s = alloc_S(CREATE_ID());
    return nested_s;
}
