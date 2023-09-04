#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

char *replace_substring(char *str1, char *str2, int index) {
    int j = 0;
    for (int i = index; i < index + strlen(str2); i++) {
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

UInt256 uint256_create_from_hex2(const char *hex) {
    UInt256 ans;

    for (int i = 0; i < 8; i++) {
        ans.data[i] = 0;
    }

    int index = 0;
    for (int i = 0; i < strlen(hex); i += 8) {

        //get current substring
        char *currC = malloc(65 * sizeof(char));
        printf("Length of hex: %d \n", strlen(hex));

        for (int j = 0; j < 8 && i + j < strlen(hex); j++) {
            int currIndex = j + i;
            strncat(currC, hex + currIndex, 1);

            printf(currC);
        }

        unsigned long currCtoLong = strtoul(currC, NULL, 16);
        ans.data[index] = currCtoLong;
        index++;
        free(currC);
    }
    return ans;
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
    // int start = 0;
    int end = hexLength;
    // if (hexLength > 64) {
    //   start = hexLength - 64;
    // }
    // char hexstring[65];
    // for (int i = 0;i<strlen(hex);i++) {
    //   hex
    // }

    //seperate hex in to 8 groups, each group having 8 bits
    for (int i = 0; i < 8; i++) {
        int start = end - 8;
        if (start < 0) {
            start = 0;
            strncpy(substring, hex + start, end - start);
            result.data[i] = strtoul(substring, NULL, 16);
            break;
        } else {
            strncpy(substring, hex + start, end - start);
            result.data[i] = strtoul(substring, NULL, 16);
        }
        end -= 8;
        // hexLength = strlen(hex);
        // if (hexLength <= 8) {
        //   result.data[i] = strtoul(hex,NULL,16);
        //   break;
        // } else {
        //   for (int j = 0;j<8;j++) {
        //     substring[7-j] = hex[strlen(hex)-1];
        //     hex[strlen(hex)-1] = '\0';
        //   }
        //   hex = hex+8;
        // }

        // int start = end - 8;
        // if(start < 0) {
        //   start = 0;
        // }
        // for(int j = 7;j>7-end+start;j--) {
        //   substring[j] = hex[i*8+j];
        // }
        // for(int j = 7-end+start;j>=0;j--) {
        //   substring[j] = '0';
        // }
        // result.data[i] = strtoul(substring,NULL,16);
        // end -= 8;
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
            //printf("\nbefore\nbuf: %s\nhex: %s\ndata: %d\n",buf,hex,val.data[7-i]);
            //strncpy(buf, hex+end-strlen(buf)-1, strlen(buf));
            replace_substring(hex, buf, end - strlen(buf));
            //printf("\nbuf: %s\nhex: %s\nindex: %d\n",buf,hex,end-strlen(buf));
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
        if (currSum < left.data[i] || prevHas1 && currSum <= left.data[i]) {
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
    UInt256 one = uint256_create_from_hex("1");
    UInt256 ans;
    ans = uint256_add(result, one);

    // TODO: implement
    return ans;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
//    UInt256 result;
//    char *toString;
//    for (int i = 0; i < 8; i++) {
//        uint32_t ansL = val.data[i];
//        while (ansL != 0) {
//            int var = ansL % 2;
//            char *str = "";
//            sprintf(str, "%d", var);
//            ansL = ansL / 2;
//            strcat(toString, str);
//        }
//    }

//    for(int i = 0; i < nbits; i++){
//        char* first = toString[0];
//        toString++;
//        strcat(toString,first);
//    }

//    int index = 0;
//    for(int i = 0; i < strlen(toString); i+= 8){
//        char* this8val;
//        for(int j = 0; j < 8; j++){
//            strcat(this8val,toString[i + j]);
//        }
//        uint32_t  thisUint;
//        strtoul(this8val, thisUint, 10);
//        result.data[index] = thisUint;
//        index++;
//    }
    // TODO: implement
    return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
    UInt256 result;
    // TODO: implement
    return result;
}
