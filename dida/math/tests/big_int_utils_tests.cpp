#include "dida/math/big_int_utils.hpp"

#include <catch2/catch.hpp>

namespace dida::math
{

TEST_CASE("add_with_carry")
{
  SECTION("No carry")
  {
    uint64_t result;
    char carry = add_with_carry(0, 123, 456, result);
    CHECK(result == 123 + 456);
    CHECK(carry == 0);
  }

  SECTION("Incoming carry")
  {
    uint64_t result;
    char carry = add_with_carry(1, 123, 456, result);
    CHECK(result == 1 + 123 + 456);
    CHECK(carry == 0);
  }

  SECTION("Outgoing carry")
  {
    uint64_t result;
    char carry = add_with_carry(0, 13508956464624346956U, 4937787609085207021U, result);
    CHECK(result == 2361);
    CHECK(carry == 1);
  }

  SECTION("Full wrap around")
  {
    uint64_t result;
    char carry = add_with_carry(1, 0xffffffffffffffff, 1234, result);
    CHECK(result == 1234);
    CHECK(carry == 1);
  }
}

TEST_CASE("sub_with_borrow")
{
  SECTION("No borrow")
  {
    uint64_t result;
    char carry = sub_with_borrow(0, 654, 321, result);
    CHECK(result == 654 - 321);
    CHECK(carry == 0);
  }

  SECTION("Incoming borrow")
  {
    uint64_t result;
    char carry = sub_with_borrow(1, 654, 321, result);
    CHECK(result == 654 - 321 - 1);
    CHECK(carry == 0);
  }

  SECTION("Outgoing borrow")
  {
    uint64_t result;
    char borrow = sub_with_borrow(0, 123, 456, result);
    CHECK(result == 18446744073709551283U);
    CHECK(borrow == 1);
  }

  SECTION("Full wrap around")
  {
    uint64_t result;
    char borrow = sub_with_borrow(1, 1234, 0xffffffffffffffff, result);
    CHECK(result == 1234);
    CHECK(borrow == 1);
  }
}

TEST_CASE("mul128 signed")
{
  SECTION("pos * pos")
  {
    SignedMul128Result result = mul128(5483619329365280178, 3038755100127240877);
    CHECK(result.low_word == 0x2152df011386fd4a);
    CHECK(result.high_word == 0xc893fe095a585a4);
  }

  SECTION("Neg * neg")
  {
    SignedMul128Result result = mul128(-5483619329365280178, -3038755100127240877);
    CHECK(result.low_word == 0x2152df011386fd4a);
    CHECK(result.high_word == 0xc893fe095a585a4);
  }
}

TEST_CASE("mul123 unsigned")
{
  SECTION("Case 1")
  {
    UnsignedMul128Result result = mul128(5483619329365280178U, 3038755100127240877U);
    CHECK(result.low_word == 0x2152df011386fd4a);
    CHECK(result.high_word == 0xc893fe095a585a4);
  }

  SECTION("Case 2")
  {
    UnsignedMul128Result result = mul128(12963124744344271438U, 15407988973582310739U);
    CHECK(result.low_word == 0x2152df011386fd4a);
    CHECK(result.high_word == 0x9643b17cb4df5d45);
  }
}

} // namespace dida::math