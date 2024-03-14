#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <stdint.h>

/* information describing a source position */
struct location
{
    const char *const fn;
    const char *const file;
    int line;
};

/* a dummy location id used as reference point within the loc_ids section. All
 * location ids are stored as an offset from this variable */
extern struct location loc_id_ref;

/* helper macro for CREATE_ID. This causes the compiler to create an instance
 * of struct location describing for a function, filename, and line number
 * and returns the offset from the loc_id_ref pointer.
 */
#define CREATE_ID_INNER(func, file, line) \
  ({ \
    static struct location cur_loc \
        __attribute__((section("loc_ids"))) = {func, file, line}; \
   ((intptr_t)&cur_loc - (intptr_t)&loc_id_ref); \
  })

/* get an 4-byte id describing the source position where this macro
 * is used */
#define CREATE_ID() CREATE_ID_INNER(__FUNCTION__, __FILE__, __LINE__)

/* print the location described by a location id created by CREATE_ID() */
void print_loc(int id);

#endif
