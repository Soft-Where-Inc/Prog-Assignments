/**
 * locations.h : Definitions to generate code-location. Works on Linux.
 *
 * History:
 *  3/2024  - Original version provided by Charles Baylis
 *
 * From: Charles Baylis <cbaylis...
 * To: Cc: Aditya Gurajada <adityagurajada@yahoo.com>
 * Sent: Wednesday, March 13, 2024 at 03:05:42 PM PDT
 * Subject: Re: Feedback points for you to take back to your dev teams.
 *
 * Hi Aditya
 *
 * There's a couple of compiler-related things which could be interesting to
 * combine with your ideas:
 *
 * 1) Encode code-location of "events" in a 4-byte int value
 *
 * Related idea 1:
 * One way to create a 4-byte value is to put the source position into a
 * separate ELF section, then the offset into that section acts as the
 * identifier.This is something I've always wanted to try out, but never had a
 * reason so far.
 * I've attached a very simple proof of concept - but be warned it's very
 * experimental!
 *
 * The CREATE_ID() macro returns an int which represents the source line where
 * the macro was used, and this can be decoded using the print_loc() function
 */
#ifndef __LOCATIONS_H__
#define __LOCATIONS_H__

#include <stdint.h>

/* information describing a source position */
struct location
{
    const char *const fn;
    const char *const file;
    int line;
};

/* A dummy location id used as reference point within the loc_ids section. All
 * location ids are stored as an offset from this variable.
 */
extern struct location loc_id_ref;

/* Helper macro for CREATE_ID. This causes the compiler to create an instance
 * of struct location describing for a function, filename, and line number
 * and returns the offset from the loc_id_ref pointer.
 */
#if __APPLE__
#define CREATE_ID_INNER(func, file, line) \
  ({ \
    static struct location cur_loc \
        __attribute__((section("__DATA, loc_ids"))) = {func, file, line}; \
   ((intptr_t)&cur_loc - (intptr_t)&loc_id_ref); \
  })
#else   // __APPLE__
#define CREATE_ID_INNER(func, file, line) \
  ({ \
    static struct location cur_loc \
        __attribute__((section("loc_ids"))) = {func, file, line}; \
   ((intptr_t)&cur_loc - (intptr_t)&loc_id_ref); \
  })
#endif  // __APPLE__

/* Get an 4-byte id describing the source position where this macro is used */
#define CREATE_ID() CREATE_ID_INNER(__FUNCTION__, __FILE__, __LINE__)

/* Print the location described by a location id created by CREATE_ID() */
void print_loc(int id);

#endif  // __LOCATIONS_H__
