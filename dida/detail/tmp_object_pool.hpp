#pragma once

#include <cstddef>

namespace dida::detail
{

/// An object pool, meant to be used to allocate temporary objects with.
///
/// This pool allocates memory block which can hold @c block_size elements, where the first block is allocated inline in
/// the object, and subsequent blocks allocated on the heap. All allocated objects are destructed when the pool is
/// destructed.
///
/// @tparam T The type of the objects to allocate.
/// @tparam block_size The number of elements per block.
template <class T, size_t block_size>
class TmpObjectPool
{
public:
  /// Constructs a @c TmpObjectPool.
  TmpObjectPool();

  ~TmpObjectPool();

  /// Allocates a new object from this pool, using the constructor which takes the given arguments.
  ///
  /// @param args The argument pack to pass to the constructor.
  /// @return A pointer to the newly allocated object.
  template <class... Args>
  T* alloc(Args&&... args);

private:
  /// A block.
  struct Block
  {
    /// The byte index of the beginning of the free memory in @c data.
    size_t head;

    /// A pointer to the next @c Block, or @c nullptr if this is the last (and hence current) block. This is used to
    /// reach all blocks when the pool is destructed.
    Block* next_block;

    /// The block of memory, into which the new objects are constructed.
    char data[block_size * sizeof(T)];
  };

  /// Calls the destructor of all objects in @c block.
  ///
  /// @param block The block whose objects to destruct.
  void destruct_objects(Block& block);

  /// The first block.
  Block first_block_;

  /// The current block.
  Block* cur_block_;
};

} // namespace dida::detail

#include "dida/detail/tmp_object_pool.inl"