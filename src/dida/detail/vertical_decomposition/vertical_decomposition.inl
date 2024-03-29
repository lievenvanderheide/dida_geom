namespace dida::detail::vertical_decomposition
{

YOnEdge y_on_edge_for_x(Segment2 edge, ScalarDeg1 x)
{
  DIDA_DEBUG_ASSERT(edge.start().x() != edge.end().x());

  // start_x + t * dir_x = x
  // t = (x - start_x) / dir_x
  // y = start_y + t * dir_y
  // y = start_y + (x - start_x) / dir_x * dir_y
  // y = (start_y * dir_x + (x - start_x) * dir_y) / dir_x

  Point2 start = edge.start();
  Vector2 dir = edge.end() - edge.start();

  ScalarDeg2 num = start.y() * dir.x() + (x - start.x()) * dir.y();
  ScalarDeg1 denom = dir.x();
  if (denom < 0)
  {
    num = -num;
    denom = -denom;
  }

  return YOnEdge(num, denom);
}

constexpr HorizontalDirection other_direction(HorizontalDirection direction)
{
  return direction == HorizontalDirection::left ? HorizontalDirection::right : HorizontalDirection::left;
}

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

Edge Edge::incoming_edge(VerticesView vertices, VertexIt end_vertex_it)
{
  return Edge{prev_cyclic(vertices, end_vertex_it), end_vertex_it};
}

Edge Edge::outgoing_edge(VerticesView vertices, VertexIt start_vertex_it)
{
  return Edge{start_vertex_it, next_cyclic(vertices, start_vertex_it)};
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

template <Winding winding>
bool Edge::on_interior_side(Point2 point) const
{
  ScalarDeg2 side = cross(*end_vertex_it - *start_vertex_it, point - *start_vertex_it);
  return winding == Winding::ccw ? side > 0 : side < 0;
}

template <Winding winding>
bool Edge::on_exterior_side(Point2 point) const
{
  ScalarDeg2 side = cross(*end_vertex_it - *start_vertex_it, point - *start_vertex_it);
  return winding == Winding::ccw ? side < 0 : side > 0;
}


bool Edge::operator==(const Edge b) const
{
  return start_vertex_it == b.start_vertex_it && end_vertex_it == b.end_vertex_it;
}

bool Edge::operator!=(const Edge b) const
{
  return start_vertex_it != b.start_vertex_it || end_vertex_it != b.end_vertex_it;
}

std::ostream& operator<<(std::ostream& s, Edge edge)
{
  if (edge.is_valid())
  {
    return s << "{" << *edge.start_vertex_it << ", " << *edge.end_vertex_it << "}";
  }
  else
  {
    return s << "Edge::invalid()";
  }
}

uint8_t Node::neighbor_branch_index(const Node* neighbor) const
{
  DIDA_DEBUG_ASSERT(neighbor);

  if (neighbors[0] == neighbor)
  {
    return 0;
  }
  else if (neighbors[1] == neighbor)
  {
    return 1;
  }
  else
  {
    DIDA_DEBUG_ASSERT(neighbors[2] == neighbor);
    return 2;
  }
}

void Node::replace_neighbor(Node* neighbor, Node* new_neighbor)
{
  DIDA_DEBUG_ASSERT(neighbor);

  if (neighbors[0] == neighbor)
  {
    neighbors[0] = new_neighbor;
  }
  else if (neighbors[1] == neighbor)
  {
    neighbors[1] = new_neighbor;
  }
  else
  {
    DIDA_DEBUG_ASSERT(neighbors[2] == neighbor);
    neighbors[2] = new_neighbor;
  }
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

template <HorizontalDirection direction>
Edge edge_for_point_with_monotone_edge_range(VerticesView vertices, EdgeRange edge_range, Point2 point)
{
  size_t range_begin_index = static_cast<size_t>(edge_range.start_vertex_it - vertices.begin());
  size_t range_num_edges = distance_cyclic(vertices, edge_range.start_vertex_it, edge_range.end_vertex_it);
  while (range_num_edges > 1)
  {
    size_t range_mid_offset = range_num_edges / 2;
    size_t range_mid_index = add_modulo(range_begin_index, range_mid_offset, vertices.size());
    if (lex_less_than_with_direction<direction>(point, vertices[range_mid_index]))
    {
      range_num_edges = range_mid_offset;
    }
    else
    {
      range_begin_index = range_mid_index;
      range_num_edges -= range_mid_offset;
    }
  }

  return Edge::edge_from_index(vertices, range_begin_index);
}

bool Region::operator==(const Region& b) const
{
  return left_node == b.left_node && right_node == b.right_node;
}

EdgeRange Region::lower_boundary(VerticalDecompositionType vd_type) const
{
  DIDA_DEBUG_ASSERT(left_node && right_node);

  if (vd_type == VerticalDecompositionType::interior_decomposition)
  {
    // In an interior decomposition, lower boundaries go towards the right.
    return EdgeRange{
        left_node->neighbors[2] == right_node ? left_node->vertex_it : left_node->lower_opp_edge.start_vertex_it,
        right_node->neighbors[2] == left_node ? right_node->vertex_it : right_node->lower_opp_edge.end_vertex_it,
    };
  }
  else
  {
    // In an exterior decomposition, lower boundaries go towards the left.
    return EdgeRange{
        right_node->neighbors[2] == left_node ? right_node->vertex_it : right_node->lower_opp_edge.start_vertex_it,
        left_node->neighbors[2] == right_node ? left_node->vertex_it : left_node->lower_opp_edge.end_vertex_it,
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
        right_node->neighbors[1] == left_node ? right_node->vertex_it : right_node->upper_opp_edge.start_vertex_it,
        left_node->neighbors[1] == right_node ? left_node->vertex_it : left_node->upper_opp_edge.end_vertex_it,
    };
  }
  else
  {
    // In an exterior decomposition, upper boundaries go towards the right.
    return EdgeRange{
        left_node->neighbors[1] == right_node ? left_node->vertex_it : left_node->upper_opp_edge.start_vertex_it,
        right_node->neighbors[1] == left_node ? right_node->vertex_it : right_node->upper_opp_edge.end_vertex_it,
    };
  }
}

RegionIterator::RegionIterator(const Node* first_node)
{
  first_node_ = first_node;

  if (first_node->type == NodeType::leaf)
  {
    cur_node_ = first_node_;
    next_node_ = first_node_->neighbors[0];
    direction_ = other_direction(first_node_->direction);
  }
  else
  {
    // If neighbors[1] and neighbors[2] are equal then we're dealing with the exterior decomposition of a monotone
    // polygon, which is not supported by this class.
    DIDA_DEBUG_ASSERT(first_node->neighbors[1] != first_node->neighbors[2]);

    cur_node_ = first_node;
    next_node_ = first_node->neighbors[first_node->direction == HorizontalDirection::left ? 1 : 2];
    direction_ = first_node->direction;
  }

  if (should_skip_current_region())
  {
    move_next();
  }
}

bool RegionIterator::move_next()
{
  do
  {
    if (next_node_->type == NodeType::leaf)
    {
      if (next_node_ == first_node_)
      {
        return false;
      }

      std::swap(next_node_, cur_node_);
      direction_ = other_direction(direction_);
    }
    else
    {
      DIDA_DEBUG_ASSERT(next_node_->type == NodeType::branch);

      if (next_node_ == first_node_ &&
          cur_node_ == next_node_->neighbors[direction_ == HorizontalDirection::left ? 1 : 2])
      {
        return false;
      }

      if (cur_node_ == next_node_->neighbors[0])
      {
        cur_node_ = next_node_;
        next_node_ = next_node_->neighbors[direction_ == HorizontalDirection::left ? 2 : 1];
      }
      else if (cur_node_ == next_node_->neighbors[1])
      {
        cur_node_ = next_node_;
        next_node_ = next_node_->neighbors[direction_ == HorizontalDirection::left ? 2 : 0];
        direction_ = HorizontalDirection::right;
      }
      else
      {
        DIDA_DEBUG_ASSERT(cur_node_ == next_node_->neighbors[2]);

        cur_node_ = next_node_;
        next_node_ = next_node_->neighbors[direction_ == HorizontalDirection::left ? 0 : 1];
        direction_ = HorizontalDirection::left;
      }
    }
  } while (should_skip_current_region());

  return true;
}

Region RegionIterator::region() const
{
  if (direction_ == HorizontalDirection::left)
  {
    return Region{next_node_, cur_node_};
  }
  else
  {
    return Region{cur_node_, next_node_};
  }
}

bool RegionIterator::should_skip_current_region() const
{
  // We should skip the current region if we're on the upper boundary of a region which also has a lower boundary.

  // If we're going towards the right, then we're on the lower boundary of the current region, so we should not skip it.
  if (direction_ == HorizontalDirection::right)
  {
    return false;
  }

  // If we get here, then we're on the upper boundary of the current region.

  // If lower_opp_edge is set, then we're sure there's a lower boundary, and since we're currently on the upper
  // boundary, we should skip it.
  if (cur_node_->lower_opp_edge.is_valid())
  {
    return true;
  }

  if (cur_node_->direction == HorizontalDirection::left)
  {
    // We're going to the left, and the direction of the current node is towards the left too. If we're inside the upper
    // branch, then there's a lower boundary, so the current region should be skipped, if we're inside the lower branch,
    // then there's no lower edge, so we shouldn't skip it.
    return next_node_ == cur_node_->neighbors[2];
  }
  else
  {
    // We're going to the left, and the direction of the current node is towards the right, so we're on the side of the
    // region which extends from cur_node->lower_opp_edge to cur_node->upper_opp_edge. Since we already know that
    // cur_node->lower_opp_edge is invalid, we know the current region doesn't have a lower boundary, so we shouldn't
    // skip it.
    return false;
  }
}

} // namespace dida::detail::vertical_decomposition