#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
    UInt256 zero; // the value equal to 0
    UInt256 one;  // the value equal to 1
    UInt256 max;  // the value equal to (2^256)-1
    UInt256 msb_set; // the value equal to 2^255
    UInt256 rot; // value used to test rotations
    UInt256 lsb_max;
    UInt256 one_on_1st;
    UInt256 test_read_from_hex;
    UInt256 test_read_from_hex_empty_byte;
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);
void test_add_zero(TestObjs *objs);
void test_sub_zero(TestObjs *objs);


int main(int argc, char **argv) {
    if (argc > 1) {
        tctest_testname_to_execute = argv[1];
    }

    TEST_INIT();

    TEST(test_get_bits);
    TEST(test_create_from_u32);
    TEST(test_create);
    TEST(test_create_from_hex);
    TEST(test_format_as_hex);
    TEST(test_add);
    TEST(test_sub);
    TEST(test_negate);
    TEST(test_rotate_left);
    TEST(test_rotate_right);

    TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
    for (unsigned i = 0; i < 8; ++i) {
        val->data[i] = wordval;
    }
}

TestObjs *setup(void) {
    TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

    // initialize several UInt256 values "manually"
    set_all(&objs->zero, 0);
    set_all(&objs->one, 0);
    objs->one.data[0] = 1U;
    set_all(&objs->max, 0xFFFFFFFFU);
    set_all(&objs->lsb_max, 0);
    objs->lsb_max.data[0] = 0xFFFFFFFFU;
    set_all(&objs->test_read_from_hex, 0);
    objs->test_read_from_hex.data[0] = 0xFFFFFFFFU;
    objs->test_read_from_hex.data[1] = 1U;
    set_all(&objs->test_read_from_hex_empty_byte, 0);
    objs->test_read_from_hex_empty_byte.data[0] = 0xFFFFFFFFU;
    objs->test_read_from_hex_empty_byte.data[1] = 0;
    objs->test_read_from_hex_empty_byte.data[2] = 1U;
    set_all(&objs->one_on_1st, 0);
    objs->one_on_1st.data[1] = 1U;

    // create a value with only the most-significant bit set
    uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
    INIT_FROM_ARR(objs->msb_set, msb_set_data);

    // value with nonzero values in least significant and most significant words
    // Note that the hex representation of this value is
    //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
    uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
    INIT_FROM_ARR(objs->rot, rot_data);

    return objs;
}

void cleanup(TestObjs *objs) {
    free(objs);
}

void test_get_bits(TestObjs *objs) {
    ASSERT(0U == uint256_get_bits(objs->zero, 0));
    ASSERT(0U == uint256_get_bits(objs->zero, 1));
    ASSERT(0U == uint256_get_bits(objs->zero, 2));
    ASSERT(0U == uint256_get_bits(objs->zero, 3));
    ASSERT(0U == uint256_get_bits(objs->zero, 4));
    ASSERT(0U == uint256_get_bits(objs->zero, 5));
    ASSERT(0U == uint256_get_bits(objs->zero, 6));
    ASSERT(0U == uint256_get_bits(objs->zero, 7));

    ASSERT(1U == uint256_get_bits(objs->one, 0));
    ASSERT(0U == uint256_get_bits(objs->one, 1));
    ASSERT(0U == uint256_get_bits(objs->one, 2));
    ASSERT(0U == uint256_get_bits(objs->one, 3));
    ASSERT(0U == uint256_get_bits(objs->one, 4));
    ASSERT(0U == uint256_get_bits(objs->one, 5));
    ASSERT(0U == uint256_get_bits(objs->one, 6));
    ASSERT(0U == uint256_get_bits(objs->one, 7));

    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
    ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
    UInt256 zero = uint256_create_from_u32(0U);
    UInt256 one = uint256_create_from_u32(1U);

    ASSERT_SAME(objs->zero, zero);
    ASSERT_SAME(objs->one, one);
}

void test_create(TestObjs *objs) {
    (void) objs;

    uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
    UInt256 val1 = uint256_create(data1);
    ASSERT(1U == val1.data[0]);
    ASSERT(2U == val1.data[1]);
    ASSERT(3U == val1.data[2]);
    ASSERT(4U == val1.data[3]);
    ASSERT(5U == val1.data[4]);
    ASSERT(6U == val1.data[5]);
    ASSERT(7U == val1.data[6]);
    ASSERT(8U == val1.data[7]);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);

  UInt256 lsb_max = uint256_create_from_hex("ffffffff");
  ASSERT_SAME(objs->lsb_max,lsb_max);

  //test creating from a string that has more than one non-zero 32 bit byte
  UInt256 test_read_from_hex_9_bits = uint256_create_from_hex("1ffffffff");
  ASSERT_SAME(objs->test_read_from_hex,test_read_from_hex_9_bits);

  //test creating from a string with a empty 32 bit byte
  UInt256 test_read_from_hex_empty_byte = uint256_create_from_hex("100000000ffffffff");
  ASSERT_SAME(objs->test_read_from_hex_empty_byte,test_read_from_hex_empty_byte);

  
}

void test_format_as_hex(TestObjs *objs) {
    char *s;

    s = uint256_format_as_hex(objs->zero);
    ASSERT(0 == strcmp("0", s));
    free(s);

    s = uint256_format_as_hex(objs->one);
    ASSERT(0 == strcmp("1", s));
    free(s);

    s = uint256_format_as_hex(objs->max);
    ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
    free(s);

    s = uint256_format_as_hex(objs->test_read_from_hex);
    ASSERT(0 == strcmp("1ffffffff", s));
    free(s);

  //test formatting a string that has a empty 32 bit byte
  s = uint256_format_as_hex(objs->test_read_from_hex_empty_byte);
  ASSERT(0 == strcmp("100000000ffffffff", s));
  free(s);
}

void test_add(TestObjs *objs) {
    UInt256 result;

    result = uint256_add(objs->zero, objs->zero);
    ASSERT_SAME(objs->zero, result);

    result = uint256_add(objs->zero, objs->one);
    ASSERT_SAME(objs->one, result);

    //test add one to one
    uint32_t two_data[8] = { 2U };
    UInt256 two;
    INIT_FROM_ARR(two, two_data);
    result = uint256_add(objs->one, objs->one);
    ASSERT_SAME(two, result);

    //test add zero
    result = uint256_add(two,objs->zero);
    ASSERT_SAME(two, result);

    //test carry
    result = uint256_add(objs->lsb_max,objs->one);
    ASSERT_SAME(objs->one_on_1st,result);

    //test overflow
    result = uint256_add(objs->max, objs->one);
    ASSERT_SAME(objs->zero, result);

    UInt256 left,right,correct_result;
    left=uint256_create_from_hex("86146ac8648bdd446dc5247bf3c5e0d08186281d1f83d9786164026ead95f4b");
    right=uint256_create_from_hex("ca05c36598289cbb3888e259ff8493697efb60fbad0071659f0f14ddb0c2801");
    correct_result=uint256_create_from_hex("1501a2e2dfcb479ffa64e06d5f34a743a00818918cc844ade0073174c5e5874c");
    result=uint256_add(left,right);
    ASSERT_SAME(result,correct_result);

    left=uint256_create_from_hex("8826b099c58436e722fbf6faa645c68bb2eb121f52f293ba91f0903d4087877");
    right=uint256_create_from_hex("404fc134cce3d6dfd845a91bed987e1c6b36eb5e6a9b0416432a9d399d34bb3");
    correct_result=uint256_create_from_hex("c87671ce92680dc6fb41a01693de44a81e21fd7dbd8d97d0d51b2d76ddbc42a");
    result=uint256_add(left,right);
    ASSERT_SAME(result,correct_result);

    left=uint256_create_from_hex("bc43c159f7af16de234a02b4cd5dcb587d004a771bb7a4e27a5c668e9e2f0b1");
    right=uint256_create_from_hex("55003206ef890bbefa44acc6c3fb53fe975a22ad57bca3e2955eb85bab6f0a1");
    correct_result=uint256_create_from_hex("11143f360e738229d1d8eaf7b91591f57145a6d24737448c50fbb1eea499e152");
    result=uint256_add(left,right);
    ASSERT_SAME(result,correct_result);
}

void test_sub(TestObjs *objs) {
    UInt256 result;

    result = uint256_sub(objs->zero, objs->zero);
    ASSERT_SAME(objs->zero, result);

    result = uint256_sub(objs->one, objs->one);
    ASSERT_SAME(objs->zero, result);

  //test negative overflow
  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);

    //test minus zero
    result = uint256_sub(objs->one, objs->zero);
    ASSERT_SAME(objs->one, result);

  //test with random data generated
  UInt256 left,right,correct_result;
  left=uint256_create_from_hex("ef256b65e9746e2744f5865c91015ea7ff9d10403f09c92f71cf5a4190dbfb4");
  right=uint256_create_from_hex("aab03404f5bf3b307f8d32689b16aaddb6a20ec64224f7e2dbb7161698bb7f6");
  correct_result=uint256_create_from_hex("44753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be");
  result=uint256_sub(left,right);
  ASSERT_SAME(result,correct_result);

    left=uint256_create_from_hex("d04035ca78313006ce75e268f264adb68a2cd6fc58b5162562d826e7d15f73a");
    right=uint256_create_from_hex("bbc32d931d41356e86fc23ac57ec10c80861c183aaa90c57e1eae1019b6771d");
    correct_result=uint256_create_from_hex("147d08375aeffa984779bebc9a789cee81cb1578ae0c09cd80ed45e635f801d");
    result=uint256_sub(left,right);
    ASSERT_SAME(result,correct_result);

    left=uint256_create_from_hex("dd75b9574c4db0b67567219c13580a16b38c363b1d4b111eb3835ad57c2a26d");
    right=uint256_create_from_hex("9db1c509a40d6956167cf6e70b6c43a27ab4fc0a6f9991fdccc94a8aa73bde1");
    correct_result=uint256_create_from_hex("3fc3f44da84047605eea2ab507ebc67438d73a30adb17f20e6ba104ad4ee48c");
    result=uint256_sub(left,right);
    ASSERT_SAME(result,correct_result);
}

void test_negate(TestObjs *objs) {
    UInt256 result;

    result = uint256_negate(objs->zero);
    ASSERT_SAME(objs->zero, result);

    result = uint256_negate(objs->one);
    ASSERT_SAME(objs->max, result);

    result = uint256_negate(objs->max);
    ASSERT_SAME(objs->one, result);
}

void test_rotate_left(TestObjs *objs) {
    UInt256 result;

    // rotating the value with just the most significant bit set
    // one position to the left should result in the value equal to 1
    // (i.e., the value with only the least significant bit set)
    result = uint256_rotate_left(objs->msb_set, 1);
    // printf("\n");
    // for(int i = 0;i<8;i++) {
    //   printf("%u\n",result.data[i]);
    // }
    ASSERT_SAME(objs->one, result);

    // after rotating the "rot" value left by 4 bits, the resulting value should be
    //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
    result = uint256_rotate_left(objs->rot, 4);
    ASSERT(0x00000ABCU == result.data[0]);
    ASSERT(0U == result.data[1]);
    ASSERT(0U == result.data[2]);
    ASSERT(0U == result.data[3]);
    ASSERT(0U == result.data[4]);
    ASSERT(0U == result.data[5]);
    ASSERT(0U == result.data[6]);
    ASSERT(0xD0000000U == result.data[7]);

    result = uint256_rotate_left(objs->one,32);
    ASSERT_SAME(objs->one_on_1st,result);

  //test with random data generated
  UInt256 long_hex,correct_result_8_bits,correct_result_36_bits,correct_result_260_bits,long_hex_rotated_left_8_bits,long_hex_rotated_left_36_bits,long_hex_rotated_left_260_bits;
  long_hex=uint256_create_from_hex("44753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be1");
  correct_result_8_bits=uint256_create_from_hex("753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be144");
  correct_result_36_bits=uint256_create_from_hex("3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be144753760f");
  correct_result_260_bits=uint256_create_from_hex("4753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be14");
  long_hex_rotated_left_8_bits = uint256_rotate_left(long_hex,8);
  long_hex_rotated_left_36_bits = uint256_rotate_left(long_hex,36);
  long_hex_rotated_left_260_bits = uint256_rotate_left(long_hex,260);
  ASSERT_SAME(correct_result_8_bits,long_hex_rotated_left_8_bits);
  ASSERT_SAME(correct_result_36_bits,long_hex_rotated_left_36_bits);
  ASSERT_SAME(correct_result_260_bits,long_hex_rotated_left_260_bits);
}

void test_rotate_right(TestObjs *objs) {
    UInt256 result;

    // rotating 1 right by 1 position should result in a value with just
    // the most-significant bit set
    result = uint256_rotate_right(objs->one, 1);
    ASSERT_SAME(objs->msb_set, result);

    // after rotating the "rot" value right by 4 bits, the resulting value should be
    //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
    result = uint256_rotate_right(objs->rot, 4);
    ASSERT(0x0000000AU == result.data[0]);
    ASSERT(0U == result.data[1]);
    ASSERT(0U == result.data[2]);
    ASSERT(0U == result.data[3]);
    ASSERT(0U == result.data[4]);
    ASSERT(0U == result.data[5]);
    ASSERT(0U == result.data[6]);
    ASSERT(0xBCD00000U == result.data[7]);

    result = uint256_rotate_right(objs->one_on_1st,32);
    ASSERT_SAME(objs->one,result);

  //test with random data generated
  UInt256 long_hex,correct_result_8_bits,correct_result_36_bits,correct_result_260_bits,long_hex_rotated_right_8_bits,long_hex_rotated_right_36_bits,long_hex_rotated_right_260_bits;
  long_hex=uint256_create_from_hex("44753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be1");
  correct_result_8_bits=uint256_create_from_hex("e144753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207b");
  correct_result_36_bits=uint256_create_from_hex("af8207be144753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442");
  correct_result_260_bits=uint256_create_from_hex("144753760f3b532f6c56853f3f5eab3ca48fb0179fce4d14c9618442af8207be");
  long_hex_rotated_right_8_bits = uint256_rotate_right(long_hex,8);
  long_hex_rotated_right_36_bits = uint256_rotate_right(long_hex,36);
  long_hex_rotated_right_260_bits = uint256_rotate_right(long_hex,260);
  ASSERT_SAME(correct_result_8_bits,long_hex_rotated_right_8_bits);
  ASSERT_SAME(correct_result_36_bits,long_hex_rotated_right_36_bits);
  ASSERT_SAME(correct_result_260_bits,long_hex_rotated_right_260_bits);
}