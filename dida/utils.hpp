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

} // namespace dida

#include "dida/utils.inl"