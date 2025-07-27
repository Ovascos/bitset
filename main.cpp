#include <iostream>

#include "bitset.hpp"
#include <cassert>
#include <bitset>
#include <random>

#define MAX 8000
#define ROUNDS 10000000

int main() {
  bitset bs(MAX);
  std::bitset<MAX> ref;

  // TODO implement dynamic increase

  static_assert(RAND_MAX > MAX);
  assert(bs.capacity() == 2 * 4032);

  for (int i = 0; i < ROUNDS; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;

    bs.set(r, val);
    ref.set(r, val);
  }

  bs.set(MAX-1, true);
  ref.set(MAX-1, true);
  assert(bs.capacity() == 2 * 4032);

  assert(ref.count() == bs.count());

  // check
  for (int i = 0; i < MAX; ++i) {
    assert(bs[i] == ref[i]);
  }

  return 0;
}
