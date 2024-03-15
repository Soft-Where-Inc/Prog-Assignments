/**
 * locations_example.h
 */
#pragma once


// Define some structure which will be malloc()'ed.
typedef struct some_struct
{
    int     data[16];
    uint32_t alloc_id;  // allocation location information
} S;

// Function prototypes
S *  alloc_S(uint32_t alloc_location_id);
S * another_extern_minion(void);
