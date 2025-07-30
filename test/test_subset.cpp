#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "../bitset.hpp"

TEST_CASE("subset") {
  const size_t CNT = GENERATE(1, 10, 1000, 1000, 4000, 6000, 12000);
  bitset super(CNT), sub(CNT);

  for (int i = 0; i < CNT; ++i) {
    if (random() % 4 != 0) continue;
    super.set(i, true);
    if (random() % 2) continue;
    sub.set(i, true);
  }

  super.set(4001, true);
  sub.set(4001, false);

  REQUIRE(super >= sub);
  REQUIRE(super > sub);
  REQUIRE(sub <= super);
  REQUIRE(sub < super);

  REQUIRE(sub <= sub);
  REQUIRE(super <= super);
  REQUIRE(sub >= sub);
  REQUIRE(super >= super);
  REQUIRE_FALSE(sub < sub);
  REQUIRE_FALSE(super < super);
  REQUIRE_FALSE(sub > sub);
  REQUIRE_FALSE(super > super);
}

TEST_CASE("subset empty") {
  const size_t CNT = GENERATE(1, 10, 1000, 1000, 4000, 6000, 12000);

  bitset empty1(CNT);
  bitset empty2(CNT);

  REQUIRE(empty1 <= empty2);
  REQUIRE(empty1 >= empty2);
  REQUIRE_FALSE(empty1 < empty2);
  REQUIRE_FALSE(empty1 > empty2);
  REQUIRE(empty1 == empty2);
  REQUIRE_FALSE(empty1 != empty2);
}

TEST_CASE("subset small") {
  const int CNT = GENERATE(1, 10, 1000, 1000, 4000, 6000, 12000);

  bitset s1(CNT);
  bitset s2(CNT);

  const unsigned POS = GENERATE_COPY(take(10, random(0, CNT)));
  s2.set(POS, true);

  REQUIRE(s1 <= s2);
  REQUIRE_FALSE(s1 >= s2);
  REQUIRE(s1 < s2);
  REQUIRE_FALSE(s1 > s2);
  REQUIRE_FALSE(s1 == s2);
  REQUIRE(s1 != s2);
}
