#include "dida/math/int128.hpp"

#include <catch2/catch.hpp>

namespace dida::math
{

TEST_CASE("Int128::operator==/!=")
{
  SECTION("Equal")
  {
    const Int128 a(0x14db818e2187895a, 0x863d518366f95809);
    const Int128 b(0x14db818e2187895a, 0x863d518366f95809);
    CHECK(a == b);
    CHECK_FALSE(a != b);
  }

  SECTION("Word 0 different")
  {
    const Int128 a(0x14db818e2187895a, 0x863d518366f95809);
    const Int128 b(0x89995abdcc1bf533, 0x863d518366f95809);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }

  SECTION("Word 1 different")
  {
    const Int128 a(0x14db818e2187895a, 0x863d518366f95809);
    const Int128 b(0x14db818e2187895a, 0xfaa9c3d2c6496984);
    CHECK_FALSE(a == b);
    CHECK(a != b);
  }
}

TEST_CASE("Int128 addition")
{
  Int128 a(0x43aac733d4780ef7, 0x433c4d4476d3d00b);
  Int128 b(0xd520e5563f9f6b7f, 0x5cac1ff155385e25);
  Int128 expected_result(0x18cbac8a14177a76, 0x9fe86d35cc0c2e31);

  SECTION("operator+")
  {
    Int128 result = a;
    result += b;
    CHECK(result == expected_result);
  }

  SECTION("operator+=")
  {
    Int128 result = a + b;
    CHECK(result == expected_result);
  }
}

TEST_CASE("Int128 subtraction")
{
  Int128 a(0x43aac733d4780ef7, 0x433c4d4476d3d00b);
  Int128 b(0x2adf1aa9c0609481, 0xa353e00eaac7a1da);
  Int128 expected_result(0x18cbac8a14177a76, 0x9fe86d35cc0c2e31);

  SECTION("operator+")
  {
    Int128 result = a;
    result -= b;
    CHECK(result == expected_result);
  }

  SECTION("operator+=")
  {
    Int128 result = a - b;
    CHECK(result == expected_result);
  }
}

TEST_CASE("Int128 unary operator-")
{
  SECTION("Small number, pos to neg")
  {
    Int128 a(7441, 0);
    Int128 result = -a;
    CHECK(result == Int128(0xffffffffffffe2ef, 0xffffffffffffffff));
  }

  SECTION("Small number, neg to pos")
  {
    Int128 a(0xffffffffffffe2ef, 0xffffffffffffffff);
    Int128 result = -a;
    CHECK(result == Int128(7441, 0));
  }

  SECTION("Large number, pos to neg")
  {
    Int128 a(0xd36dec03d1a20b94, 0x5fb07e28f79262f5);
    Int128 result = -a;
    CHECK(result == Int128(0x2c9213fc2e5df46c, 0xa04f81d7086d9d0a));
  }

  SECTION("Large number, neg to pos")
  {
    Int128 a(0x2c9213fc2e5df46c, 0xa04f81d7086d9d0a);
    Int128 result = -a;
    CHECK(result == Int128(0xd36dec03d1a20b94, 0x5fb07e28f79262f5));
  }
}

TEST_CASE("Int128::multiply(int64_t, int64_t")
{
  SECTION("Pos * pos")
  {
    Int128 result = Int128::multiply(4104136789188515624, 5097340703310780667);
    Int128 expected_result = *Int128::from_string("20920183507485837399169888946366641208");
    CHECK(result == expected_result);
  }

  SECTION("Neg * neg")
  {
    Int128 result = Int128::multiply(-8093618667873191041, -1356390665853648473);
    Int128 expected_result = *Int128::from_string("10978108814082036948648275954786930393");
  }
}

TEST_CASE("Int128::add_checked")
{
  SECTION("Pos + pos, no overflow")
  {
    // 105227881237694100329063014827352455189 + 64913302222775131402624288888531650538 =
    // 170141183460469231731687303715884105727
    Int128 a(0x85f78cfd63f0f815, 0x4f2a271a5ccd0dca);
    Int128 b(0x7a0873029c0f07ea, 0x30d5d8e5a332f235);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(*result == Int128(0xffffffffffffffff, 0x7fffffffffffffff));
  }

  SECTION("Pos + pos, overflow")
  {
    // 105227881237694100329063014827352455189 + 64913302222775131402624288888531650539 =
    // 170141183460469231731687303715884105728
    Int128 a(0x85f78cfd63f0f815, 0x4f2a271a5ccd0dca);
    Int128 b(0x7a0873029c0f07eb, 0x30d5d8e5a332f235);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(result == std::nullopt);
  }

  SECTION("Neg + neg, no overflow")
  {
    // -120769712366065449235956825647743170644 + -49371471094403782495730478068140935084 =
    // -170141183460469231731687303715884105728
    Int128 a(0xd5a311b8e1dd07ac, 0xa524997612f1053b);
    Int128 b(0x2a5cee471e22f854, 0xdadb6689ed0efac4);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(*result == Int128(0, 0x8000000000000000));
  }

  SECTION("Neg + neg, overflow")
  {
    // -120769712366065449235956825647743170645 + -49371471094403782495730478068140935084 =
    // -170141183460469231731687303715884105728
    Int128 a(0xd5a311b8e1dd07ab, 0xa524997612f1053b);
    Int128 b(0x2a5cee471e22f854, 0xdadb6689ed0efac4);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(result == std::nullopt);
  }

  SECTION("Pos + neg")
  {
    Int128 a(0x20e4609049ea9722, 0x1fdc72aec10128fa);
    Int128 b(0x2b907cbe837d68a6, 0xdf36e31884797345);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(result == Int128(0x4c74dd4ecd67ffc8, 0xff1355c7457a9c3f));
  }

  SECTION("Neg + pos")
  {
    Int128 a(0x933c4ef2b79e98b7, 0xe739eafb19511b80);
    Int128 b(0x9f3c42c375b470e5, 0x505b1c0895c465ed);
    std::optional<Int128> result = a.add_checked(b);
    CHECK(result == Int128(0x327891b62d53099c, 0x37950703af15816e));
  }
}

TEST_CASE("Int128::sub_checked")
{
  SECTION("Pos - neg, no overflow")
  {
    // 105227881237694100329063014827352455189 - -64913302222775131402624288888531650538 =
    // 170141183460469231731687303715884105727
    Int128 a(0x85f78cfd63f0f815, 0x4f2a271a5ccd0dca);
    Int128 b(0x85f78cfd63f0f816, 0xcf2a271a5ccd0dca);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(*result == Int128(0xffffffffffffffff, 0x7fffffffffffffff));
  }

  SECTION("Pos - neg, overflow")
  {
    // 105227881237694100329063014827352455189 - -64913302222775131402624288888531650539 =
    // 170141183460469231731687303715884105728
    Int128 a(0x85f78cfd63f0f815, 0x4f2a271a5ccd0dca);
    Int128 b(0x85f78cfd63f0f815, 0xcf2a271a5ccd0dca);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(result == std::nullopt);
  }

  SECTION("Neg - pos, no overflow")
  {
    // -120769712366065449235956825647743170644 -49371471094403782495730478068140935084 =
    // -170141183460469231731687303715884105728
    Int128 a(0xd5a311b8e1dd07ac, 0xa524997612f1053b);
    Int128 b(0xd5a311b8e1dd07ac, 0x2524997612f1053b);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(*result == Int128(0, 0x8000000000000000));
  }

  SECTION("Neg - pos, overflow")
  {
    // -120769712366065449235956825647743170645 - 49371471094403782495730478068140935084 =
    // -170141183460469231731687303715884105728
    Int128 a(0xd5a311b8e1dd07ab, 0xa524997612f1053b);
    Int128 b(0xd5a311b8e1dd07ac, 0x2524997612f1053b);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(result == std::nullopt);
  }

  SECTION("Pos - pos")
  {
    Int128 a(0x20e4609049ea9722, 0x1fdc72aec10128fa);
    Int128 b(0xd46f83417c82975a, 0x20c91ce77b868cba);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(result == Int128(0x4c74dd4ecd67ffc8, 0xff1355c7457a9c3f));
  }

  SECTION("Neg - neg")
  {
    Int128 a(0x933c4ef2b79e98b7, 0xe739eafb19511b80);
    Int128 b(0x60c3bd3c8a4b8f1b, 0xafa4e3f76a3b9a12);
    std::optional<Int128> result = a.sub_checked(b);
    CHECK(result == Int128(0x327891b62d53099c, 0x37950703af15816e));
  }
}

TEST_CASE("Int128::from_string")
{
  SECTION("Empty string")
  {
    std::optional<Int128> result = Int128::from_string("");
    CHECK(result == std::nullopt);
  }

  SECTION("Just a - sign")
  {
    std::optional<Int128> result = Int128::from_string("-");
    CHECK(result == std::nullopt);
  }

  SECTION("1 chunk, positive")
  {
    std::optional<Int128> result = Int128::from_string("4936");
    CHECK(*result == Int128(4936, 0));
  }

  SECTION("1 chunk, negative")
  {
    std::optional<Int128> result = Int128::from_string("-4936");
    CHECK(*result == Int128(static_cast<uint64_t>(-4936), -1));
  }

  SECTION("Invalid character in chunk 1")
  {
    std::optional<Int128> result = Int128::from_string("7f4936");
    CHECK(result == std::nullopt);
  }

  SECTION("2 chunks, positive")
  {
    std::optional<Int128> result = Int128::from_string("25843697617534880583288416");
    CHECK(*result == Int128(0xa7b4999f4ab88660, 0x15609d));
  }

  SECTION("2 chunks, negative")
  {
    std::optional<Int128> result = Int128::from_string("-252226212217183878705951056559071");
    CHECK(*result == Int128(0xf7ca314c53248421, 0xfffff390752746b8));
  }

  SECTION("Invalid character in chunk 2")
  {
    std::optional<Int128> result = Int128::from_string("51a84522271176542496408743");
    CHECK(result == std::nullopt);
  }

  SECTION("Large positive number")
  {
    std::optional<Int128> result = Int128::from_string("124157835837910802133188454831525806606");
    CHECK(*result == Int128(0x8e9877aab781420e, 0x5d67edec4f5d4f33));
  }

  SECTION("Large negative number")
  {
    std::optional<Int128> result = Int128::from_string("-157131623128588561860452654257017098680");
    CHECK(*result == Int128(0x3dbc56f53fdd6e48, 0x89c98cd8a3929f5d));
  }

  SECTION("Largest positive number")
  {
    std::optional<Int128> result = Int128::from_string("170141183460469231731687303715884105727");
    CHECK(result == Int128(0xffffffffffffffff, 0x7fffffffffffffff));
  }

  SECTION("Just overflowing positive number")
  {
    std::optional<Int128> result = Int128::from_string("170141183460469231731687303715884105728");
    CHECK(result == std::nullopt);
  }

  SECTION("Largest negative number")
  {
    std::optional<Int128> result = Int128::from_string("-170141183460469231731687303715884105728");
    CHECK(*result == Int128(0, 0x8000000000000000));
  }

  SECTION("Just overflowing negative number")
  {
    std::optional<Int128> result = Int128::from_string("-170141183460469231731687303715884105729");
    CHECK(result == std::nullopt);
  }

  SECTION("Way too large positive number")
  {
    std::optional<Int128> result = Int128::from_string("69743641148273155711537180638183289487697959149");
    CHECK(result == std::nullopt);
  }

  SECTION("Way too large positive number")
  {
    std::optional<Int128> result = Int128::from_string("-69743641148273155711537180638183289487697959149");
    CHECK(result == std::nullopt);
  }
}

} // namespace dida::math