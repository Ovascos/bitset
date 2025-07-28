#include <catch2/catch_test_macros.hpp>

#include "../bitset.hpp"

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
