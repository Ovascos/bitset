#ifndef BITSET_HPP
#define BITSET_HPP

#include <vector>
#include <cstdint>

class bitset {
public:
  bitset();
  explicit bitset(size_t size);

  void set(size_t index, bool value);
  bool get(size_t index) const;

  inline bool operator[](size_t index) const {
    return get(index);
  }

  /** Clears the bitset. */
  void clear();

  /** Checks if the bitset is empty. */
  bool empty() const;

  /** Gets the current capacity of the bitset. */
  size_t capacity() const;

  /** Counts the number of bits set to true in the bitset. */
  size_t count() const;

  // bitwise operations
  bitset operator&(const bitset& other) const;
  bitset operator|(const bitset& other) const;
  bitset operator^(const bitset& other) const;
  bitset operator~() const;

  /**
   * @brief Subtracts the bits of another bitset from this one.
   * @details b1 - b2 is equivalent to b1 ^ (b1 & b2).
   */
  bitset operator-(const bitset& other) const;

  void operator&=(const bitset& other);
  void operator|=(const bitset& other);
  void operator^=(const bitset& other);

  // comparison
  bool operator==(const bitset& other) const;
  bool operator!=(const bitset& other) const;
  /**
   * @brief Checks if this bitset is a subset of another bitset.
   */
  bool operator<=(const bitset& other) const;
  bool operator>=(const bitset& other) const;
  bool operator<(const bitset& other) const;
  bool operator>(const bitset& other) const;

private:
  std::vector<uint64_t> _bits;
};

#endif //BITSET_HPP
