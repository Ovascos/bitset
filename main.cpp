#include <iostream>

#include "bitset.hpp"
#include <cassert>
#include <bitset>
#include <random>

#define MAX 12000
#define ROUNDS 10000000

int main() {
  bitset bs, bs2;
  std::bitset<MAX> ref;

  static_assert(RAND_MAX > MAX);
  assert(bs.capacity() == 4032);

  for (int i = 0; i < ROUNDS/2; ++i) {
    unsigned r = random() % MAX / 3;
    bool val = random() % 2;

    bs.set(r, val);
    ref.set(r, val);
  }

  bs.resize(MAX);
  bs.set(MAX-1, true);
  ref.set(MAX-1, true);
  assert(bs.capacity() == 3 * 4032);

  for (int i = 0; i < ROUNDS/2; ++i) {
    unsigned r = random() % MAX;
    bool val = random() % 2;

    bs.set(r, val);
    ref.set(r, val);
  }

  // check
  assert(ref.count() == bs.count());
  for (int i = 0; i < MAX; ++i) {
    assert(bs[i] == ref[i]);
  }

  // clear all
  for (int i = MAX/2+1; i < MAX; ++i) {
    bs.set(i, false);
  }

  bs2.resize(MAX);
  assert(bs2.empty());
  for (int i = 0; i < MAX/2; ++i) {
    if (ref[i]) bs2.set(i, true);
  }

  assert(bs == bs2);

  return 0;
}
