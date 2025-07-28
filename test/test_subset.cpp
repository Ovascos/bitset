#include <catch2/catch_test_macros.hpp>

#include "../bitset.hpp"

TEST_CASE("subset") {
  const size_t CNT = 4000;
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