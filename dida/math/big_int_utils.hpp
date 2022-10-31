#pragma once

#include <cstdint>
#include <x86intrin.h>

namespace dida::math
{

/// Computes the sum <tt>a + b + carry</tt>, stores bits 0:63 in @c result, and returns bit 64 as the carry for the next
/// @c add_with_carry call.
///
/// @param carry The incoming carry. Should be 0 or 1.
/// @param a The first operand.
/// @param b The second operand.
/// @param result A reference to the @c uint64_t which will receive the result.
/// @return The outgoing carry.
inline char add_with_carry(char carry, uint64_t a, uint64_t b, uint64_t& result);

/// Computes the difference <tt>a - b - borrow</tt>, stores bits 0:63 in @c result, and returns bit 64 as the borrow for
/// the next @c sub_with_borrow call.
///
/// @param carry The incoming borrow. Should be 0 or 1.
/// @param a The first operand.
/// @param b The second operand.
/// @param result A reference to the @c uint64_t which will receive the result.
/// @return The outgoing carry.
inline char sub_with_borrow(char borrow, uint64_t a, uint64_t b, uint64_t& result);

/// The result of a signed @c mul128 call.
struct SignedMul128Result
{
  /// The low word.
  uint64_t low_word;

  /// The high word.
  int64_t high_word;
};

/// Multiplies 2 signed 64 bit integers, and returns the full result as a @c SignedMul128Result.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return A @c SignedMul128Result containing the result.
inline SignedMul128Result mul128(int64_t a, int64_t b);

/// The result of an unsigned @c mul128 call.
struct UnsignedMul128Result
{
  /// The low word.
  uint64_t low_word;

  /// The high word.
  uint64_t high_word;
};

/// Multiplies 2 unsigned 64 bit integers, and returns the full result as a @c UnsignedMul128Result.
///
/// @param a The first operand.
/// @param b The second operand.
/// @return An @c UnsignedMul128Result containing the result.
inline UnsignedMul128Result mul128(uint64_t a, uint64_t b);

} // namespace dida::math

#include "dida/math/big_int_utils.inl"