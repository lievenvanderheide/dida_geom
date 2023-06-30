#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

#include "dida/assert.hpp"

namespace dida
{

/// A view into a contiguous block of memory containing elements of type @c T.
///
/// @tparam T The type of elements in the view.
template <class T>
class ArrayView
{
public:
  using iterator = T*;
  using const_iterator = const T*;

  /// Constructs an empty @c ArrayView.
  inline ArrayView();

  ///@{
  /// Constructs an @c ArrayView into the content of the given @c std::vector.
  ///
  /// Since the @c ArrayView does not own the data it refers to, it's the responsibility of the user to make sure the @c
  /// std::vector remains alive (and isn't resized) during the lifetime of the @c ArrayView.
  ///
  /// @param v The vector with the data the @c ArrayView should refer to.
  inline ArrayView(std::vector<T>& v);
  template <typename SrcT, typename = std::enable_if_t<
                               std::is_const_v<T> && std::is_same_v<std::remove_const_t<T>, std::remove_const_t<SrcT>>>>
  inline ArrayView(const std::vector<SrcT>& v);
  ///@}

  /// Constructs an @c ArrayView using a pointer and size.
  ///
  /// @param begin A pointer to the beginning of the memory block.
  /// @param size The number of elements in the memory block.
  inline ArrayView(T* begin, size_t size);

  /// Returns the number of elements in this @c ArrayView.
  ///
  /// @return The number of elements.
  inline size_t size() const;

  ///@{
  /// Returns the element at @c index of this @c ArrayView.
  ///
  /// @param index The index of the element.
  /// @return The element at the given index.
  inline T& operator[](size_t index);
  inline const T& operator[](size_t index) const;
  ///@}

  ///@{
  /// Returns an iterator to the beginning of this @c ArrayView.
  ///
  /// @return The iterator.
  inline iterator begin();
  const_iterator begin() const;
  ///@}

  ///@{
  /// Returns an iterator to the end of this @c ArrayView.
  ///
  /// @return The iterator.
  inline iterator end();
  inline const_iterator end() const;
  ///@}

private:
  T* begin_;
  size_t size_;
};

} // namespace dida

#include "dida/array_view.inl"