#include <utility>

namespace dida::detail
{

template <class T, size_t block_size>
TmpObjectPool<T, block_size>::TmpObjectPool()
{
  first_block_.head = 0;
  first_block_.next_block = nullptr;

  cur_block_ = &first_block_;
}

template <class T, size_t block_size>
TmpObjectPool<T, block_size>::~TmpObjectPool()
{
  destruct_objects(first_block_);

  Block* heap_block = first_block_.next_block;
  while (heap_block)
  {
    Block* next_block = heap_block->next_block;
    destruct_objects(*heap_block);
    delete heap_block;
    heap_block = next_block;
  }
}

template <class T, size_t block_size>
template <class... Args>
T* TmpObjectPool<T, block_size>::alloc(Args&&... args)
{
  if (cur_block_->head == block_size * sizeof(T))
  {
    Block* new_block = new Block;
    new_block->head = 0;
    new_block->next_block = nullptr;

    cur_block_->next_block = new_block;
    cur_block_ = new_block;
  }

  T* new_object = new (cur_block_->data + cur_block_->head) T(std::forward<Args>(args)...);
  cur_block_->head += sizeof(T);
  return new_object;
}

template <class T, size_t block_size>
void TmpObjectPool<T, block_size>::destruct_objects(Block& block)
{
  for (size_t i = 0; i < block.head; i += sizeof(T))
  {
    reinterpret_cast<T*>(block.data + i)->~T();
  }
}

} // namespace dida::detail