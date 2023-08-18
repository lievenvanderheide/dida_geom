namespace dida
{

template <class T>
ArrayView<T>::ArrayView() : begin_(nullptr), size_(0)
{
}

template <class T>
ArrayView<T>::ArrayView(std::vector<T>& v) : begin_(v.data()), size_(v.size())
{
}

template <class T>
template <typename SrcT, typename>
ArrayView<T>::ArrayView(const std::vector<SrcT>& v) : begin_(v.data()), size_(v.size())
{
}

template <class T>
template <size_t N>
ArrayView<T>::ArrayView(std::array<T, N>& v)
{
  begin_ = v.data();
  size_ = v.size();
}

template <class T>
template <typename SrcT, size_t N, typename>
ArrayView<T>::ArrayView(const std::array<SrcT, N>& v)
{
  begin_ = v.data();
  size_ = v.size();
}

template <class T>
inline ArrayView<T>::ArrayView(T* begin, size_t size) : begin_(begin), size_(size)
{
}

template <class T>
inline size_t ArrayView<T>::size() const
{
  return size_;
}

template <class T>
inline T& ArrayView<T>::operator[](size_t index)
{
  DIDA_DEBUG_ASSERT(index < size_);
  return begin_[index];
}

template <class T>
inline const T& ArrayView<T>::operator[](size_t index) const
{
  DIDA_DEBUG_ASSERT(index < size_);
  return begin_[index];
}

template <class T>
inline typename ArrayView<T>::iterator ArrayView<T>::begin()
{
  return begin_;
}

template <class T>
inline typename ArrayView<T>::iterator ArrayView<T>::end()
{
  return begin_ + size_;
}

template <class T>
inline typename ArrayView<T>::const_iterator ArrayView<T>::begin() const
{
  return begin_;
}

template <class T>
inline typename ArrayView<T>::const_iterator ArrayView<T>::end() const
{
  return begin_ + size_;
}

} // namespace dida