#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "../bitset.hpp"

TEST_CASE("iterator mini") {
  const size_t CNT = GENERATE(1, 10, 100, 1000, 4000, 6000, 10000, 12000);

  bitset a(CNT);
  a.set(3, true);

  auto it = a.cbegin();
  REQUIRE(it != a.cend());
  REQUIRE(*it == 3);
  auto nit = it++;
  REQUIRE(it == a.cend());
  REQUIRE(*nit == 3);
  auto nnit = ++nit;
  REQUIRE(nnit == nit);
  REQUIRE(nit == a.cend());
}

TEST_CASE("iterator empty") {
  const size_t CNT = GENERATE(1, 10, 100, 1000, 4000, 6000, 10000, 12000);
  bitset a(CNT);
  REQUIRE(a.empty());
  auto it = a.cbegin();
  REQUIRE(it == a.cend());
}

TEST_CASE("iterator metadata") {
  const size_t CNT = GENERATE(1, 10, 100, 1000, 4000, 6000, 10000, 12000, 24000, 54000);

  bitset a(CNT);

  for (int i = 0; i < CNT; i += 3) {
    a.set(i, true);
  }

  auto it = a.cbegin();
  unsigned cnt = 0;
  while(it != a.cend()) {
    REQUIRE(cnt == *it);
    ++it;
    cnt += 3;
  }
}

TEST_CASE("one big set") {
  const size_t CNT = GENERATE(10, 100, 1000, 4000, 6000, 10000, 12000, 24000, 54000);

  bitset a(CNT);

  a.set(CNT - 3, true);

  auto it = a.cbegin();
  REQUIRE_FALSE(it == a.cend());
  REQUIRE(*it == CNT - 3);
  ++it;
  REQUIRE(it == a.cend());
}
