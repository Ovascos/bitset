#include <catch2/catch_test_macros.hpp>

#include "../bitset.hpp"

#include <cassert>
#include <bitset>
#include <random>

#define ROUNDS 10000

TEST_CASE("insert bits") {
  const size_t CNT = 4000;

  bitset bs;
  std::bitset<CNT> ref;

  static_assert(RAND_MAX > CNT);
  REQUIRE( bs.capacity() == 4032 );

  for (int i = 0; i < ROUNDS; ++i) {
    unsigned r = random() % CNT;
    bool val = random() % 4;

    bs.set(r, val);
    ref.set(r, val);
  }

  REQUIRE(ref.count() == bs.count());
  for (int i = 0; i < CNT; ++i) {
    REQUIRE(bs[i] == ref[i]);
  }
}

TEST_CASE("test increase") {
  const size_t CNT = 12000;

  bitset bs;
  std::bitset<CNT> ref;

  static_assert(RAND_MAX > CNT);
  REQUIRE(bs.capacity() == 4032);

  for (int i = 0; i < ROUNDS; ++i) {
    unsigned r = random() % CNT / 3;
    bool val = random() % 2;

    bs.set(r, val);
    ref.set(r, val);
  }

  bs.resize(CNT);
  bs.set(CNT-1, true);
  ref.set(CNT-1, true);
  REQUIRE(bs.capacity() == 3 * 4032);

  for (int i = 0; i < CNT; ++i) {
    REQUIRE(bs[i] == ref[i]);
  }
}

TEST_CASE("equality") {
  const size_t CNT = 4000;

  bitset bs;
  std::bitset<CNT> ref;

  static_assert(RAND_MAX > CNT);
  REQUIRE(bs.capacity() == 4032);

  for (int i = 0; i < ROUNDS; ++i) {
    unsigned r = random() % CNT;
    bool val = random() % 2;

    bs.set(r, val);
    ref.set(r, val);
  }

  bitset bs2;
  REQUIRE(bs2.empty());
  REQUIRE(bs2.capacity() == 4032);
  for (int i = 0; i < CNT; ++i) {
    if (ref[i]) bs2.set(i, true);
  }

  REQUIRE(bs == bs2);
  REQUIRE_FALSE(bs != bs2);
}

TEST_CASE("clearing") {
  const size_t CNT = 8000;

  bitset bs(CNT);

  static_assert(RAND_MAX > CNT);
  REQUIRE(bs.capacity() == 2 * 4032);

  for (int i = 0; i < ROUNDS; ++i) {
    unsigned r = random() % CNT;
    bool val = random() % 2;

    bs.set(r, val);
  }

  for (int i = 0; i < CNT; ++i) {
    bs.set(i, false);
  }

  bitset bs2;
  REQUIRE(bs2.empty());
  REQUIRE(bs2.capacity() == 4032);
  bs2.resize(CNT);
  REQUIRE(bs2.capacity() == 2 * 4032);

  REQUIRE(bs == bs2);
  REQUIRE_FALSE(bs != bs2);
}
