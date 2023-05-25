#pragma once

#include <cstddef>

namespace dida
{

/// Returns an iterator to the previous element from @c it, while wrapping around if the beginning of the container is
/// reached.
///
/// @tparam Container The container type.
/// @tparam Iterator The iterator type.
/// @param container The container.
/// @param it The original iterator.
/// @return An iterator to the previous element from @c it.
template <class Container, class Iterator>
Iterator prev_cyclic(Container& container, Iterator it);

/// Returns an iterator to the next element from @c it, while wrapping around if the end of the container is reached.
///
/// @tparam Container The container type.
/// @tparam Iterator The iterator type.
/// @param container The container.
/// @param it The original iterator.
/// @return An iterator to the previous element from @c it.
template <class Container, class Iterator>
Iterator next_cyclic(Container& container, Iterator it);

/// Returns the distance from @c a to @c b in a cyclically ordered set. The result is the number of times @c next_cyclic
/// has to be called to go from @c a to @c b.
///
/// @tparam Container The container type.
/// @tparam Iterator The iterator type.
/// @param container The container.
/// @param a The iterator of the element to start measuring the distance from.
/// @param b The iterator of the element to which to measure the distance.
/// @return The distance from @c a to @c b.
template <class Container, class Iterator>
size_t distance_cyclic(const Container& container, Iterator a, Iterator b);

/// Returns the predecessor of @c a modulo @c m.
///
/// @pre @c a should be in the range <tt>0 <= a < m</tt>.
/// @tparam IntType The type of @c a and @c m.
/// @param a The integer modulo @c m whose predecessor to compute.
/// @param m The integer modulo which the result is computed.
/// @return The predecessor of @c a modulo @c m.
template <class IntType>
IntType pred_modulo(IntType a, IntType m);

/// Returns the successor of @c a modulo @c m.
///
/// @pre @c a should be in the range <tt>0 <= a < m</tt>.
/// @tparam IntType The type of @c a and @c m.
/// @param a The integer modulo @c m whose successor to compute.
/// @param m The integer modulo which the result is computed.
/// @return The successor of @c a modulo @c m.
template <class IntType>
IntType succ_modulo(IntType a, IntType m);

/// Computes <tt>a + b</tt> modulo m. Operand @c a must already be in the range <tt>0 <= a < m</tt>, and operand @c b
/// in the range <tt>0 <= b <= m</tt> (note that @c b is allowed to be equal to @c m).
///
/// @tparam IntType The type of @c a, @c b and @c m.
/// @param a The first operand.
/// @param a The second operand.
/// @return The unique integer modulo @c m which is congruent to <tt>a + b</tt>
template <class IntType>
IntType add_modulo(IntType a, IntType b, IntType m);

/// Computes <tt>a - b</tt> modulo m. Operand @c a must already be in the range <tt>0 <= a < m</tt>, and operand @c b
/// in the range <tt>0 <= b <= m</tt> (note that @c b is allowed to be equal to @c m).
///
/// @tparam IntType The type of @c a, @c b and @c m.
/// @param a The first operand.
/// @param a The second operand.
/// @return The unique integer modulo @c m which is congruent to <tt>a - b</tt>
template <class IntType>
IntType sub_modulo(IntType a, IntType b, IntType m);

/// Returns whether @c a, @c b and @c c occur in that order in a cyclically ordered set, that is, when starting from @c
/// a, and cyclically iterating over the elements, element @c b is encountered before element @c c.
///
/// If @c a, @c b, @c c are not all distinct then the reslt is @c false.
///
/// @tparam T The type of elements to compare.
/// @param a The first operand.
/// @param b The second operand.
/// @param c The third operand.
/// @return True iff @c a, @c b and @c are cyclically ordered.
template <class T>
bool cyclic_order(const T& a, const T& b, const T& c);

/// An overload of @c cyclic_order which takes a user provided @c less_than functor.
template <class T, class LessThan>
bool cyclic_order(const T& a, const T& b, const T& c, LessThan less_than);

} // namespace dida

#include "dida/utils.inl"