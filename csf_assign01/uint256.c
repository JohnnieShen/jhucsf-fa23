#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

char *replace_substring(char *str1, char *str2, int index) {
    int j = 0;
    for (int i = index; (size_t)i < index + strlen(str2); i++) {
        str1[i] = str2[j];
        j++;
    }
    return str1;
}

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
    UInt256 result;
    result.data[0] = val;
    for (int i = 1; i < 8; i++) {
        result.data[i] = 0;
    }
    return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
    UInt256 result;
    for (int i = 0; i < 8; i++) {
        result.data[i] = data[i];
    }
    return result;
}


// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
    UInt256 result;
    //set all values in result to 0
    for (int i = 0; i < 8; i++) {
        result.data[i] = 0;
    }
    //to record the buffer
    char substring[9];
    int hexLength = strlen(hex);
    int end = hexLength;
    //seperate hex in to 8 groups, each group having 8 bits
    for (int i = 0; i < 8; i++) {
        for (int j = 0;j<8;j++) {
            substring[j] = '\0';
        }
        int start = end - 8;
        if (start < 0) {
            start = 0;
            strncpy(substring, hex + start, end - start);
            //printf("\n\n%s %d %d\n\n",substring,start,end);
            result.data[i] = strtoul(substring, NULL, 16);
            break;
        } else {
            strncpy(substring, hex + start, end - start);
            //printf("\n\n%s %d %d\n\n",substring,start,end);
            result.data[i] = strtoul(substring, NULL, 16);
        }
        end -= 8;
    }
    return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
    char *hex = (char *) malloc(65 * sizeof(char));
    for (int i = 0; i < 64; i++) {
        hex[i] = '0';
    }
    int end = 64;
    char *buf = (char *) malloc(9 * sizeof(char));
    for (int i = 7; i >= 0; i--) {
        if (i == 7 || val.data[7 - i] != 0) {
            sprintf(buf, "%x", val.data[7 - i]);
            replace_substring(hex, buf, end - strlen(buf));
            end -= 8;
        } else {
            break;
        }
    }
    char *tmp = hex;
    while (tmp[0] == '0' && strlen(tmp) > 1) {
        tmp++;
    }
    memmove(hex, tmp, 65 - (tmp - hex));
    free(buf);
    return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
    uint32_t bits;
    bits = val.data[index];
    return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
    UInt256 sum;
    int prevHas1 = 0;
    for (int i = 0; i < 8; i++) {
        uint32_t currSum = left.data[i] + right.data[i];
        if (prevHas1) {
            currSum++;
        }
        if (currSum < left.data[i] || (prevHas1 && currSum <= left.data[i])) {
            prevHas1 = 1;
        }
        sum.data[i] = currSum;
    }
    // TODO: implement
    return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
    UInt256 result;
    UInt256 negRight = uint256_negate(right);
    result = uint256_add(left, negRight);


    // TODO: implement
    return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
    UInt256 result;
    for (int i = 0; i < 8; i++) {
        result.data[i] = ~val.data[i];
    }
    UInt256 one = uint256_create_from_u32(1);
    UInt256 ans;
    ans = uint256_add(result, one);

    // TODO: implement
    return ans;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
    int nbitsAfterShift = nbits % 256;
    int biggerShift = nbitsAfterShift / 32;
    int smallerShift = nbitsAfterShift % 32;
    UInt256 result;
    for(int i = 0;i<8;i++) {
      result.data[(i+biggerShift)%8] = val.data[i];
    }
    int index = 0;
    uint32_t shifted, further_shifted, rotated;
    while(index < 7) {
      if (index != 0 ){
        shifted = further_shifted;
      } else {
        shifted = result.data[index] >> (32-smallerShift);
      }
      further_shifted = result.data[index+1] >> (32-smallerShift);
      rotated = result.data[index+1] << smallerShift;
      result.data[index+1] = rotated | shifted;
      index++;
    }
    rotated = result.data[0] << smallerShift;
    result.data[0] = rotated | further_shifted;
    return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
    int nbitsAfterShift = nbits % 256;
    int biggerShift = nbitsAfterShift / 32;
    int smallerShift = nbitsAfterShift % 32;
    UInt256 result;
    for(int i = 0;i<8;i++) {
      result.data[i] = val.data[(i+biggerShift)%8];
    }
    int index = 7;
    uint32_t shifted, further_shifted, rotated;
    while(index > 0) {
      if (index != 7 ){
        shifted = further_shifted;
      } else {
        shifted = result.data[index] << (32-smallerShift);
      }
      further_shifted = result.data[index-1] << (32-smallerShift);
      rotated = result.data[index-1] >> smallerShift;
      result.data[index-1] = rotated | shifted;
      index--;
    }
    rotated = result.data[7] >> smallerShift;
    result.data[7] = rotated | further_shifted;
    return result;
}
