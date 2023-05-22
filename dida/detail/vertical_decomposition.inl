namespace dida::detail::vertical_decomposition
{

Edge Edge::edge_from_index(VerticesView vertices, size_t index)
{
  Edge result;
  result.start_vertex_it = vertices.begin() + index;
  result.end_vertex_it = next_cyclic(vertices, result.start_vertex_it);
  return result;
}

Edge Edge::invalid()
{
  return Edge{nullptr, nullptr};
}

bool Edge::is_valid() const
{
  DIDA_DEBUG_ASSERT((start_vertex_it == nullptr) == (end_vertex_it == nullptr));
  return start_vertex_it != nullptr;
}

Segment2 Edge::segment() const
{
  return Segment2::unsafe_from_endpoints(*start_vertex_it, *end_vertex_it);
}

bool Edge::operator==(const Edge b) const
{
  return start_vertex_it == b.start_vertex_it && end_vertex_it == b.end_vertex_it;
}

VerticalDecompositionRegionsIterator::Region VerticalDecompositionRegionsIterator::region() const
{
  if (direction_ == HorizontalDirection::left)
  {
    return Region{next_node_, cur_node_, leaf_region_branch_index_};
  }
  else
  {
    return Region{cur_node_, next_node_, leaf_region_branch_index_};
  }
}

bool VerticalDecompositionRegionsIterator::Region::operator==(const Region& b) const
{
  return left_node == b.left_node && right_node == b.right_node &&
         ((left_node && right_node) || (leaf_region_branch_index == b.leaf_region_branch_index));
}

} // namespace dida::detail::vertical_decomposition