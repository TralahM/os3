/*
 * =====================================================================================
 *
 *       Filename:  crc.h
 *
 *    Description: Cyclic Rendundacy Check 32 bits
 *
 *
 *       Compiler:  gcc
 *
 * =====================================================================================
 */
#include <stdlib.h>

uint32_t calculate_crc32c(uint32_t crc32c, const unsigned char* buffer,
                          unsigned int length);

int hamming_distance(uint8_t* char1, uint8_t* char2);
