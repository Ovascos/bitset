#include <catch2/catch_test_macros.hpp>

#include "../bitset.hpp"

TEST_CASE("inplace: 0 |= 1") {
  bitset a(1000);
  bitset b(1000);
  b.set(0, true);
  a |= b;
  REQUIRE(a.get(0) == true);
}

TEST_CASE("inplace: and") {
  const size_t CNT = 2000;
  const size_t SZE = 4000;

  bitset a(SZE), b(SZE);

  for (int i = 0; i < CNT; ++i) {
    unsigned r = random();
    if (r & 0x1) a.set(i, true);
    if (r & 0x2) b.set(i, true);
  }

  bitset ref = a & b;
  a &= b;
  REQUIRE(a == ref);
}

TEST_CASE("inplace: or") {
  const size_t CNT = 2000;
  const size_t SZE = 4000;

  bitset a(SZE), b(SZE);

  for (int i = 0; i < CNT; ++i) {
    unsigned r = random();
    if (r & 0x1) a.set(i, true);
    if (r & 0x2) b.set(i, true);
  }

  bitset ref = a | b;
  a |= b;
  REQUIRE(a == ref);
}

TEST_CASE("inplace: xor") {
  const size_t CNT = 2000;
  const size_t SZE = 4000;

  bitset a(SZE), b(SZE);

  for (int i = 0; i < CNT; ++i) {
    unsigned r = random();
    if (r & 0x1) a.set(i, true);
    if (r & 0x2) b.set(i, true);
  }

  bitset ref = a ^ b;
  a ^= b;
  REQUIRE(a == ref);
}

TEST_CASE("clear") {
  bitset a(2*4032);
  REQUIRE(a.capacity() == 2*4032);
  a.clear();
  REQUIRE(a.capacity() == 2*4032);
}

TEST_CASE("resize") {
  bitset a(1);
  REQUIRE(a.capacity() == 4032);
  a.resize(4032);
  REQUIRE(a.capacity() == 4032);

  bitset b(4032);
  REQUIRE(b.capacity() == 4032);
  b.resize(4032 * 2);
  REQUIRE(b.capacity() == 2*4032);
}
