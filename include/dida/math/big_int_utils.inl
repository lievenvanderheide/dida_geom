#include <x86intrin.h>

namespace dida::math
{

inline char add_with_carry(char carry, uint64_t a, uint64_t b, uint64_t& result)
{
  unsigned long long result_ll;
  char new_carry =
      _addcarry_u64(carry, static_cast<unsigned long long>(a), static_cast<unsigned long long>(b), &result_ll);
  result = static_cast<uint64_t>(result_ll);
  return new_carry;
}

inline char sub_with_borrow(char borrow, uint64_t a, uint64_t b, uint64_t& result)
{
  unsigned long long result_ll;
  char new_borrow =
      _subborrow_u64(borrow, static_cast<unsigned long long>(a), static_cast<unsigned long long>(b), &result_ll);
  result = static_cast<uint64_t>(result_ll);
  return new_borrow;
}

inline SignedMul128Result mul128(int64_t a, int64_t b)
{
  unsigned __int128 result_u128 = static_cast<unsigned __int128>(static_cast<__int128>(a) * static_cast<__int128>(b));
  return SignedMul128Result{static_cast<uint64_t>(result_u128),
                            static_cast<int64_t>(static_cast<uint64_t>(result_u128 >> 64))};
}

inline UnsignedMul128Result mul128(uint64_t a, uint64_t b)
{
  unsigned __int128 result_i128 = static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b);
  return UnsignedMul128Result{static_cast<uint64_t>(result_i128), static_cast<uint64_t>(result_i128 >> 64)};
}

} // namespace dida::math