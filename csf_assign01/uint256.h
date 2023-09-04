#ifndef UINT256_H
#define UINT256_H

#include <stdint.h>

// Data type representing a 256-bit unsigned integer, represented
// as an array of 8 uint32_t values. It is expected that the value
// at index 0 is the least significant, and the value at index 7
// is the most significant.
//
// Don't make any changes to the representation of this data type.
// (I.e., don't add fields, remove fields, change names of fields, etc.)
typedef struct {
  uint32_t data[8];
} UInt256;

char *replace_substring(char *str1, char *str2,int index);

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val);

// Create a UInt256 value from an array of 8 uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create(const uint32_t data[8]);

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex);

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val);

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index);

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right);

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right);

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val);
#if 0

// Not assigning this for Fall 2023
// Compute the product of two UInt256 values.
UInt256 uint256_mul(UInt256 left, UInt256 right);
#endif

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits);

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits);

// You may add additional functions if you would like to

#endif // UINT256_H
