#include "bitset.hpp"

#include <cassert>
#include <bit>

#define BITS   (sizeof(uint64_t) * 8)

// mask the lower/upper n bits
#define MSK_LO(N)  (((uint64_t)1 << (N)) - 1)
#define MSK_HI(N)  (~(((uint64_t)-1) >> (N)))

// mask the n-th bit
#define MSK_BT(N)  ((uint64_t)1 << (N))

// metadate capacity
#define MS     ((sizeof(uint64_t) * 8) - 1)
// bits per metadata block
#define MMS    (MS * BITS)
// metadata position and offset of a bit
#define MP(b)  ((b / BITS) / MS)
#define MO(b)  ((b / BITS) % MS)
// bitmask of the next metadata identifier (MSBit) and data fields
#define M_NEXT_MSK  MSK_HI(1)
#define M_DATA_MAK  (~M_NEXT_MSK)


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
  return std::popcount(b & m);
}

/** Counts the number of bits set in b within the first n bits. */
static inline unsigned count_bits_lo(uint64_t b, unsigned n) {
  return count_bits(b, MSK_LO(n));
}

bitset::bitset() : _bits(1) {
  assert(_bits[0] == 0);
}

bitset::bitset(size_t size) : _bits(MP(size) + 1, M_NEXT_MSK) {
  // clear the last entry
  _bits.back() = 0;
}

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
    cnt += count_bits(*(p++), M_DATA_MAK);
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
    if (*it & M_DATA_MAK) return false;
  } while (*(it++) & M_DATA_MAK);
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

