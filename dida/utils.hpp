#pragma once

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

} // namespace dida

#include "dida/utils.inl"