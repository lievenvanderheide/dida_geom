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

VerticalDecompositionTypesIterator::Region VerticalDecompositionTypesIterator::region() const
{
  if (direction_ == HorizontalDirection::left)
  {
    return Region{next_node_, cur_node_, next_node_incoming_branch_, cur_node_outgoing_branch_};
  }
  else
  {
    return Region{cur_node_, next_node_, cur_node_outgoing_branch_, next_node_incoming_branch_};
  }
}

bool VerticalDecompositionTypesIterator::Region::operator==(const Region& b) const
{
  return left_node == b.left_node && right_node == b.right_node &&
         (!left_node || left_node_branch_index == b.left_node_branch_index) &&
         (!right_node || right_node_branch_index == b.right_node_branch_index);
}

} // namespace dida::detail::vertical_decomposition