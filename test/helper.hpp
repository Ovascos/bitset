#include "../bitset.hpp"

#include <bitset>
#include <cassert>
#include <ostream>

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
