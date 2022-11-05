namespace dida
{

template <class Container, class Iterator>
Iterator prev_cyclic(Container& container, Iterator it)
{
  return it != container.begin() ? it - 1 : container.end() - 1;
}

template <class Container, class Iterator>
Iterator next_cyclic(Container& container, Iterator it)
{
  ++it;
  return it != container.end() ? it : container.begin();
}

} // namespace dida