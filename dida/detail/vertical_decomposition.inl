namespace dida::detail::vertical_decomposition
{

template <HorizontalDirection direction>
bool lex_less_than_with_direction(Point2 a, Point2 b)
{
  if constexpr (direction == HorizontalDirection::left)
  {
    return lex_greater_than(a, b);
  }
  else
  {
    return lex_less_than(a, b);
  }
}

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

EdgeRange EdgeRange::invalid()
{
  return EdgeRange{nullptr, nullptr};
}

bool EdgeRange::is_valid() const
{
  DIDA_DEBUG_ASSERT(!start_vertex_it == !end_vertex_it);
  return start_vertex_it != nullptr;
}

bool Region::operator==(const Region& b) const
{
  return left_node == b.left_node && right_node == b.right_node &&
         (!left_node || left_node_branch_index == b.left_node_branch_index) &&
         (!right_node || right_node_branch_index == b.right_node_branch_index);
}

bool Region::is_leaf() const
{
  return !left_node || !right_node;
}

EdgeRange Region::lower_boundary(VerticalDecompositionType vd_type) const
{
  DIDA_DEBUG_ASSERT(left_node && right_node);

  if (vd_type == VerticalDecompositionType::interior_decomposition)
  {
    // In an interior decomposition, lower boundaries go towards the right.
    return EdgeRange{
        left_node_branch_index == 2 ? left_node->vertex_it : left_node->lower_opp_edge.start_vertex_it,
        right_node_branch_index == 2 ? right_node->vertex_it : right_node->lower_opp_edge.end_vertex_it,
    };
  }
  else
  {
    // In an exterior decomposition, lower boundaries go towards the left.
    return EdgeRange{
        right_node_branch_index == 2 ? right_node->vertex_it : right_node->lower_opp_edge.start_vertex_it,
        left_node_branch_index == 2 ? left_node->vertex_it : left_node->lower_opp_edge.end_vertex_it,
    };
  }
}

EdgeRange Region::upper_boundary(VerticalDecompositionType vd_type) const
{
  DIDA_DEBUG_ASSERT(left_node && right_node);

  if (vd_type == VerticalDecompositionType::interior_decomposition)
  {
    // In an interior decomposition, upper boundaries go towards the left.
    return EdgeRange{
        right_node_branch_index == 1 ? right_node->vertex_it : right_node->upper_opp_edge.start_vertex_it,
        left_node_branch_index == 1 ? left_node->vertex_it : left_node->upper_opp_edge.end_vertex_it,
    };
  }
  else
  {
    // In an exterior decomposition, upper boundaries go towards the right.
    return EdgeRange{
        left_node_branch_index == 1 ? left_node->vertex_it : left_node->upper_opp_edge.start_vertex_it,
        right_node_branch_index == 1 ? right_node->vertex_it : right_node->upper_opp_edge.end_vertex_it,
    };
  }
}

namespace
{

template <HorizontalDirection direction>
VertexIt leaf_reflex_vertex_with_direction(VerticesView vertices, EdgeRange edge_range)
{
  DIDA_DEBUG_ASSERT(edge_range.start_vertex_it && edge_range.end_vertex_it);

  // Note, edge_range contains at least 3 vertices, and the reflex vertex can't be the first or last vertex of the
  // range, so we don't need to include those to vertices in the search range.

  size_t range_begin_index = static_cast<size_t>(edge_range.start_vertex_it - vertices.begin()) + 1;
  size_t range_num_vertices = distance_cyclic(vertices, edge_range.start_vertex_it, edge_range.end_vertex_it) - 1;

  while (range_num_vertices > 1)
  {
    size_t range_mid_offset = range_num_vertices / 2;
    size_t range_mid_index = add_modulo(range_begin_index, range_mid_offset, vertices.size());
    size_t range_mid_prev_index = add_modulo(range_begin_index, range_mid_offset - 1, vertices.size());
    if (lex_less_than_with_direction<direction>(vertices[range_mid_prev_index], vertices[range_mid_index]))
    {
      range_begin_index = range_mid_index;
      range_num_vertices -= range_mid_offset;
    }
    else
    {
      range_num_vertices = range_mid_offset;
    }
  }

  return vertices.begin() + range_begin_index;
}

} // namespace

VertexIt Region::leaf_reflex_vertex(VerticesView vertices, VerticalDecompositionType vd_type) const
{
  DIDA_DEBUG_ASSERT(!left_node != !right_node);

  if (left_node)
  {
    if (!left_node->lower_opp_edge.is_valid())
    {
      return nullptr;
    }

    EdgeRange edge_range;
    if (vd_type == VerticalDecompositionType::interior_decomposition)
    {
      // The edge range starts at the lower edge and ends at the upper edge.
      edge_range.start_vertex_it =
          left_node_branch_index == 2 ? left_node->vertex_it : left_node->lower_opp_edge.start_vertex_it;
      edge_range.end_vertex_it =
          left_node_branch_index == 1 ? left_node->vertex_it : left_node->upper_opp_edge.end_vertex_it;
    }
    else
    {
      // The edge range starts at the upper edge and ends at the lower edge.
      edge_range.start_vertex_it =
          left_node_branch_index == 1 ? left_node->vertex_it : left_node->upper_opp_edge.start_vertex_it;
      edge_range.end_vertex_it =
          left_node_branch_index == 2 ? left_node->vertex_it : left_node->lower_opp_edge.end_vertex_it;
    }

    return leaf_reflex_vertex_with_direction<HorizontalDirection::right>(vertices, edge_range);
  }
  else
  {
    if (!right_node->lower_opp_edge.is_valid())
    {
      return nullptr;
    }

    EdgeRange edge_range;
    if (vd_type == VerticalDecompositionType::interior_decomposition)
    {
      // The edge range starts at the upper edge and ends at the lower edge.
      edge_range.start_vertex_it =
          right_node_branch_index == 1 ? right_node->vertex_it : right_node->upper_opp_edge.start_vertex_it;
      edge_range.end_vertex_it =
          right_node_branch_index == 2 ? right_node->vertex_it : right_node->lower_opp_edge.end_vertex_it;
    }
    else
    {
      // The edge range starts at the lower edge and ends at the upper edge.
      edge_range.start_vertex_it =
          right_node_branch_index == 2 ? right_node->vertex_it : right_node->lower_opp_edge.start_vertex_it;
      edge_range.end_vertex_it =
          right_node_branch_index == 1 ? right_node->vertex_it : right_node->upper_opp_edge.end_vertex_it;
    }

    return leaf_reflex_vertex_with_direction<HorizontalDirection::left>(vertices, edge_range);
  }
}

Region RegionIterator::region() const
{
  if (direction_ == HorizontalDirection::left)
  {
    return Region{next_node_, cur_node_, next_node_branch_index_, cur_node_branch_index_};
  }
  else
  {
    return Region{cur_node_, next_node_, cur_node_branch_index_, next_node_branch_index_};
  }
}

} // namespace dida::detail::vertical_decomposition