#include <catch2/catch_test_macros.hpp>

#include "../bitset.hpp"

#include <cassert>
#include <bitset>
#include <ostream>
#include <random>

template<size_t I>
static bool operator==(const bitset &b, const std::bitset<I> &r) {
  assert(b.capacity() >= I);

  for (int i = 0; i < I; ++i) {
    if (b[i] != r[i]) return false;
  }

  return true;
}

static std::ostream& operator<<(std::ostream& os, const bitset& b) {
  for (int i = 0; i < b.capacity(); ++i) {
    os << (b[i] ? '1' : '0');
  }
  return os;
}

TEST_CASE("insert bits") {
  const size_t CNT = 4000;
  const size_t ROUNDS = 10000;

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
  REQUIRE(bs == ref);
}

TEST_CASE("test increase") {
  const size_t CNT = 12000;
  const size_t ROUNDS = 10000;

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
  REQUIRE(bs == ref);
}

TEST_CASE("equality") {
  const size_t CNT = 4000;
  const size_t ROUNDS = 10000;

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
  const size_t ROUNDS = 10000;

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

TEST_CASE("and") {
  const size_t MAX = 12000;
  const size_t RND = 4000;

  bitset b1(MAX), b2(MAX);
  std::bitset<MAX> r1, r2;

  for (int i = 0; i < RND; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;
    b1.set(r, val);
    r1.set(r, val);
  }

  for (int i = 0; i < RND; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;
    b2.set(r, val);
    r2.set(r, val);
  }

  REQUIRE(b1 == r1);
  REQUIRE(b2 == r2);
  bitset b = b1 & b2;
  auto r = r1 & r2;
  REQUIRE(b == r);
}

TEST_CASE("or") {
  const size_t MAX = 3000;
  const size_t RND = 500;

  bitset b1(MAX), b2(MAX);
  std::bitset<MAX> r1, r2;

  for (int i = 0; i < RND; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;
    b1.set(r, val);
    r1.set(r, val);
  }

  for (int i = 0; i < RND; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;
    b2.set(r, val);
    r2.set(r, val);
  }

  REQUIRE(b1 == r1);
  REQUIRE(b2 == r2);
  bitset b = b1 | b2;
  auto r = r1 | r2;
  REQUIRE(b == r);
}
