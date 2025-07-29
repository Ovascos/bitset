#include "bitset.hpp"

#include <cassert>
#include <algorithm>

#if __cplusplus >= 202002L
#include <bit>
#endif

#define BITS   (sizeof(uint64_t) * 8)

// mask the lower/upper n bits
#define MSK_LO(N)  (((uint64_t)1 << (N)) - 1)
#define MSK_HI(N)  (~(((uint64_t)-1) >> (N)))

// mask the n-th bit
#define MSK_BT(N)  ((uint64_t)1 << (N))

// lowest bit
#define LB(X) (X & MSK_BT(0))

// metadate capacity
#define MS     ((sizeof(uint64_t) * 8) - 1)
// bits per metadata block
#define MMS    (MS * BITS)
// metadata position and offset of a bit
#define MP(b)  ((b / BITS) / MS)
#define MO(b)  ((b / BITS) % MS)
// bitmask of the next metadata identifier (MSBit) and data fields
#define M_NEXT_MSK  MSK_HI(1)
#define M_DATA_MSK  (~M_NEXT_MSK)


/** Extract the n-th bit of b. */
static inline bool get_bit(uint64_t b, unsigned n) {
  assert(n < BITS);
  return b & MSK_BT(n);
}

static inline void set_bit(uint64_t &b, unsigned n) {
  assert(n < BITS);
  b |= MSK_BT(n);
}

static inline void clear_bit(uint64_t &b, unsigned n) {
  assert(n < BITS);
  b &= ~MSK_BT(n);
}

static inline unsigned count_bits(uint64_t b, uint64_t m = -1) {
#if __cplusplus >= 202002L
  return std::popcount(b & m);
#else
  return __builtin_popcountll(b & m);
#endif
}

/** Counts the number of bits set in b within the first n bits. */
static inline unsigned count_bits_lo(uint64_t b, unsigned n) {
  return count_bits(b, MSK_LO(n));
}

bitset::bitset() noexcept : _bits(1) {
  assert(_bits[0] == 0);
}

bitset::bitset(size_t size) noexcept : _bits(MP(size) + 1, M_NEXT_MSK) {
  // clear the last entry
  _bits.back() = 0;
}

bitset::bitset(bitstore &&v) noexcept : _bits(v) { }

void bitset::resize(size_t size) {
  if (size <= capacity())
    return;

  auto it = _bits.begin();
  while(*(it++) & M_NEXT_MSK);
  *(it - 1) |= M_NEXT_MSK;

  size_t cnt_n = MP(size) + 1;
  size_t cnt_m = it - _bits.begin();
  assert(cnt_m < cnt_n);
  unsigned n = cnt_n - cnt_m;
  it = _bits.insert(it, n, M_NEXT_MSK);
  *(it + n - 1) = 0x00;
}

// TODO: all with the same size? use a manager? Pass data offset via param from manager?

/** Gets the offset in the data, based on the metadata and index. */
static inline unsigned get_offset(const uint64_t *md, size_t index) {
  unsigned cnt = 0;
  const uint64_t *p = md;
  while (index >= MMS) {
    assert(*md & M_NEXT_MSK);
    cnt += count_bits(*(p++), M_DATA_MSK);
    index -= MMS;
  }
  assert(MP(index) == 0);
  cnt += count_bits_lo(*p, MO(index));
  // skip the remaining metadata blocks
  while (*(p++) & M_NEXT_MSK);
  return cnt + (p - md);
}

bool bitset::get(size_t index) const {
  unsigned mp = MP(index), mo = MO(index);
  assert(capacity() >= index);
  // check if the corresponding data field exists
  if (!get_bit(_bits[mp], mo))
    return false;
  return get_bit(_bits[get_offset(_bits.data(), index)], index % BITS);
}

void bitset::set(size_t index, bool value) {
  unsigned mp = MP(index), mo = MO(index);
  assert(capacity() >= index);
  bool mb = get_bit(_bits[mp], mo);

  // no need to do anything
  // TODO check if bool is normalize (and remove if so)
  if (!mb && !value) return;

  // get the index of the data field
  unsigned d_idx = get_offset(_bits.data(), index);

  // check if the corresponding data field exists
  if (!mb) {
    assert(value);
    // insert the new field
    _bits.insert(_bits.begin() + d_idx, 0x00);
    set_bit(_bits[mp], mo);
  }

  (value ? set_bit : clear_bit)(_bits[d_idx], index % BITS);

  // clear the data field if empty
  if (_bits[d_idx] == 0x00) {
    assert(!value);
    _bits.erase(_bits.begin() + d_idx);
    clear_bit(_bits[mp], mo);
  }
}

void bitset::clear() {
  _bits.clear();
  _bits.push_back(0);
  assert(count() == 0);
}

bool bitset::empty() const {
  auto it = _bits.cbegin();
  assert(it != _bits.cend());
  do {
    if (*it & M_DATA_MSK) return false;
  } while (*(it++) & M_DATA_MSK);
  return true;
}

size_t bitset::capacity() const {
  auto it = _bits.cbegin();
  while(*(it++) & M_NEXT_MSK);
  assert(it <= _bits.cend());
  return BITS * MS * (it - _bits.cbegin());
}

size_t bitset::count() const {
  size_t cnt = 0;
  auto it = _bits.cbegin();
  while(*(it++) & M_NEXT_MSK);
  while(it != _bits.cend()) cnt += count_bits(*(it++));
  return cnt;
}

bool bitset::operator==(const bitset &other) const {
  assert(capacity() == other.capacity());
  assert(_bits.size() == other._bits.size());
  return _bits == other._bits;
}

bool bitset::operator!=(const bitset &other) const {
  assert(capacity() == other.capacity());
  assert(_bits.size() == other._bits.size());
  return _bits != other._bits;
}

/** Checks if the bitset of a is a subset of b. */
static inline bool subset(uint64_t a, uint64_t b) {
  /*
   * a b |  a & ~b
   * ----+-------
   * 0 0 |  0
   * 0 1 |  0
   * 1 0 |  1
   * 1 1 |  0
   */
  return (a & ~b) == 0;
}

static inline bool subset_proper(uint64_t a, uint64_t b) {
  return subset(a, b) && a != b;
}

static bool subset_check(const bitset::bitstore &a, const bitset::bitstore &b, bool proper) {
  // check metadata
  auto it1 = a.cbegin();
  auto it2 = b.cbegin();
  do {
    if (!subset(*it1, *it2)) return false;
  } while (*(it1++) & *(it2++) & M_NEXT_MSK);

  // check data
  auto m1 = a.cbegin();
  auto m2 = b.cbegin();
  do {
    uint64_t v1 = *m1 & M_DATA_MSK;
    uint64_t v2 = *m2 & M_DATA_MSK;
    uint64_t s = ~v1 & v2; // skip
    uint64_t c = v1 & v2;  // check
    assert(!(v1 & ~v2)); // checked above
    assert(!(s & c)); // either check or skip
    for (; c; c >>= 1, s >>= 1) {
      if (LB(c)) {
        if (!((proper ? subset_proper : subset)(*it1, *it2))) return false;
        it1++; it2++;
      } else if (LB(s)) {
        it2++;
      }
    }
    it2 += count_bits(s);
  } while (*(m1++) & *(m2++) & M_NEXT_MSK);

  return true;
}

bool bitset::operator<=(const bitset &other) const {
  assert(capacity() == other.capacity());
  return subset_check(_bits, other._bits, false);
}

bool bitset::operator>=(const bitset &other) const {
  assert(capacity() == other.capacity());
  return subset_check(other._bits, _bits, false);
}

bool bitset::operator<(const bitset &other) const {
  assert(capacity() == other.capacity());
  return subset_check(_bits, other._bits, true);
}

bool bitset::operator>(const bitset &other) const {
  assert(capacity() == other.capacity());
  return subset_check(other._bits, _bits, true);
}

typedef uint64_t (*op_t)(uint64_t, uint64_t);

static bitset::bitstore combine(const bitset::bitstore &a, const bitset::bitstore &b, const op_t op) {
  auto it1 = a.cbegin();
  auto it2 = b.cbegin();
  while (*(it1++) & *(it2++) & M_NEXT_MSK);

  bitset::bitstore out(it1 - a.cbegin());

  // check data
  auto m1 = a.cbegin();
  auto m2 = b.cbegin();
  size_t mo = 0;
  do {
    uint64_t v1 = *m1 & M_DATA_MSK;
    uint64_t v2 = *m2 & M_DATA_MSK;
    uint64_t o = 0;
    int rem = BITS - 1;
    for (; v1 | v2; v1 >>= 1, v2 >>= 1, o >>= 1) {
      -- rem;
      uint64_t a1 = LB(v1) ? *(it1++) : 0;
      uint64_t a2 = LB(v2) ? *(it2++) : 0;
      uint64_t v = op(a1, a2);
      if (v) {
        o |= MSK_HI(1);
        out.push_back(v);
      }
    }
    assert(rem >= 0);
    o >>= rem;
    assert((o & M_DATA_MSK) == o);
    out[mo++] = o | M_NEXT_MSK;
  } while (*(m1++) & *(m2++) & M_NEXT_MSK);
  assert((*(m1-1) & M_NEXT_MSK) == (*(m2-1) & M_NEXT_MSK));
  // clear the next bit for the last metadata word
  out[mo-1] &= ~M_NEXT_MSK;

  return std::move(out);
}

bitset bitset::operator&(const bitset &other) const {  // check metadata
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a & b; };
  return bitset(std::move(combine(_bits, other._bits, o)));
}

bitset bitset::operator|(const bitset &other) const {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a | b; };
  return bitset(std::move(combine(_bits, other._bits, o)));
}

bitset bitset::operator^(const bitset &other) const {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a ^ b; };
  return bitset(std::move(combine(_bits, other._bits, o)));
}

bitset bitset::operator-(const bitset &other) const {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a ^ (a & b); };
  return bitset(std::move(combine(_bits, other._bits, o)));
}

static void update(bitset::bitstore &a, const bitset::bitstore &b, const op_t op) {
  auto it1 = a.begin();
  auto it2 = b.cbegin();
  while (*(it1++) & *(it2++) & M_NEXT_MSK);

  // check data
  unsigned m1 = 0;
  unsigned m2 = 0;
  do {
    uint64_t v1 = a[m1] & M_DATA_MSK;
    uint64_t v2 = b[m2] & M_DATA_MSK;
    uint64_t o = 0;
    int rem = BITS - 1;
    for (; v1 | v2; v1 >>= 1, v2 >>= 1, o >>= 1) {
      -- rem;
      uint64_t a1 = LB(v1) ? *(it1) : 0;
      uint64_t a2 = LB(v2) ? *(it2++) : 0;
      uint64_t v = op(a1, a2);
      if (LB(v1)) {
        *it1 = v;
        it1++;
        o |= (v ? MSK_HI(1) : 0);
      } else if (v) {
        it1 = a.insert(it1, v);
        it1++;
        o |= MSK_HI(1);
      }
    }
    assert(rem >= 0);
    o >>= rem;
    a[m1] = o;
    assert((o & M_DATA_MSK) == o);
  } while (a[m1++] & b[m2++] & M_NEXT_MSK);
  assert((a[m1-1] & M_NEXT_MSK) == (b[m2-1] & M_NEXT_MSK));

  // clear all empty fields
  a.erase(std::remove(a.begin() + m1, a.end(), 0), a.end());
}

void bitset::operator&=(const bitset &other) {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a & b; };
  update(_bits, other._bits, o);
}

void bitset::operator|=(const bitset &other) {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a | b; };
  update(_bits, other._bits, o);
}

void bitset::operator^=(const bitset &other) {
  assert(capacity() == other.capacity());
  const auto o = [](uint64_t a, uint64_t b) { return a ^ b; };
  update(_bits, other._bits, o);
}
