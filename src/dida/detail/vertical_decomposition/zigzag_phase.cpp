#include "dida/detail/vertical_decomposition/zigzag_phase.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

/// The state of the zig-zag algorithm.
struct ZigzagState
{
  /// The vertices of the polygon.
  VerticesView vertices;

  /// The node pool, used to allocate new nodes.
  NodePool* node_pool;

  /// The current direction.
  HorizontalDirection direction;

  /// Whether we're currently going forward.
  bool forward;

  /// The vertex where the zigzag algorithm should terminate. If the input is a polygon, then this is equal to the first
  /// vertex of the last chain, if the input is a chain, then this is the last_node->vertex_it.
  VerticesView::const_iterator last_vertex_it;

  /// The current edge. This is the edge which contains the current active point.
  Edge current_edge;

  /// The previous node. This is the node whose vertical extension is the boundary of the current region in the
  /// direction opposite to @c direction. @c prev_node is always set.
  Node* prev_node;

  /// The next node. This is the node whose vertical extension is the boundary of the current region in the
  /// direction of @c direction. @c next_node can be @c nullptr.
  Node* next_node;

  /// XXX
  Node* last_node;

  /// The output vector of chain decompositions. The last chain in this array is the chain we're currently building.
  std::vector<ChainDecomposition> chain_decompositions;
};

/// Initializes a @c ZigzagState, when decomposing the interior of a polygon. This function finds a convex side vertex
/// to start the first chain with, and initializes the @c ZigzagState to the state with this vertex as its active point.
void interior_zigzag_init(ZigzagState& state, VerticesView vertices, NodePool& node_pool);

/// Initializes a @c ZigzagState, when decomposing the interior of a polygon. This function initializes @c ZigzagState to the state with
/// first_node->vertex_it as its active point.
void exterior_zigzag_init(ZigzagState& state, VerticesView vertices, Node* first_node, Node* last_node,
                          NodePool& node_pool);

/// Runs the main loop of the zigzag algorithm.
template <Winding winding>
void zigzag_iterate(ZigzagState& state);

/// Performs 'forward' iterations until the next side vertex.
///
/// @tparam The direction. This should be equal to @c state.direction.
/// @param state The @c ZigzagState.
/// @return True if we should continue iterating, false we're finished.
template <HorizontalDirection direction>
bool zigzag_forward(ZigzagState& state);

/// Performs 'reverse' iterations until the next side vertex.
///
/// @tparam The direction. This should be equal to @c state.direction.
/// @param state The @c ZigzagState.
/// @return True if we should continue iterating, false we're finished.
template <HorizontalDirection direction>
bool zigzag_reverse(ZigzagState& state);

/// Handles the case when a convex side vertex is reached in @c zigzag_forward. When called, @c state.current_edge
/// should already have advanced to the outgoing edge of the side vertex.
///
/// This function creates a leaf node at the current vertex. The next iterations will be 'reverse' in the direction
/// opposite to @c direction.
///
/// @tparam The current direction. This should be equal to @c state.direction.
/// @param state The @c ZigzagState.
/// @return True if we should continue iterating, false we're finished.
template <HorizontalDirection direction>
bool zigzag_forward_convex_corner(ZigzagState& state);

/// Handles the case when a convex side vertex is reached in @c zigzag_reverse. When called, @c state.current_edge
/// should already have advanced to the outgoing edge of the side vertex.
///
/// This function ends the current chain and starts a new one. The initial iterations of this new chain will be
/// 'forward' in the direction opposite to @c direction.
///
/// @tparam The current direction. This should be equal to @c state.direction.
/// @param state The @c ZigzagState.
/// @return True if we should continue iterating, false we're finished.
template <HorizontalDirection direction>
bool zigzag_reverse_convex_corner(ZigzagState& state, VerticesView::const_iterator prev_vertex_it);

/// Handles the case when a concave side vertex is reached in @c zigzag_forward or @c zigzag_reverse. When called,
/// @c state.current_edge should already have advanced to the outgoing edge of the side vertex.
///
/// The function creates a node at the current vertex. The next iterations will be 'forward' in the direction opposite
/// to @c direction.
///
/// @tparam The current direction. This should be equal to @c state.direction.
/// @param state The @c ZigZagState.
/// @return True if we should continue iterating, false we're finished.
template <HorizontalDirection direction>
bool zigzag_concave_corner(ZigzagState& state);

/// Returns whether the corner at @c b, with previous vertex @c a and next vertex @c c is a convex corner.
template <Winding winding>
bool is_convex_corner(Point2 a, Point2 b, Point2 c);

template <Winding winding>
void interior_zigzag_init(ZigzagState& state, VerticesView vertices, NodePool& node_pool)
{
  state.vertices = vertices;
  state.node_pool = &node_pool;
  state.last_node = nullptr;

  for (VerticesView::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
  {
    VerticesView::const_iterator prev_it = prev_cyclic(vertices, it);
    VerticesView::const_iterator next_it = next_cyclic(vertices, it);

    bool incoming_towards_right = lex_less_than(*prev_it, *it);
    bool outgoing_towards_right = lex_less_than(*it, *next_it);
    if (incoming_towards_right != outgoing_towards_right)
    {
      if (is_convex_corner<winding>(*prev_it, *it, *next_it))
      {
        HorizontalDirection direction = outgoing_towards_right ? HorizontalDirection::right : HorizontalDirection::left;

        Node* node = state.node_pool->alloc();
        node->direction = direction;
        node->type = NodeType::branch;
        node->vertex_it = it;
        node->lower_opp_edge = Edge::invalid();
        node->upper_opp_edge = Edge::invalid();
        node->neighbors[0] = nullptr;
        node->neighbors[1] = nullptr;
        node->neighbors[2] = nullptr;

        state.direction = direction;
        state.forward = true;
        state.last_vertex_it = it;
        state.current_edge = Edge{it, next_it};
        state.prev_node = node;
        state.next_node = nullptr;

        state.chain_decompositions.push_back(ChainDecomposition{node, nullptr});

        return;
      }
    }
  }

  DIDA_ASSERT(!"No convex side vertex found - input not a valid polygon.");
}

void exterior_zigzag_init(ZigzagState& state, VerticesView vertices, Node* first_node, Node* last_node,
                          NodePool& node_pool)
{
  state.vertices = vertices;
  state.node_pool = &node_pool;
  state.direction = first_node->direction;
  state.forward = true;
  state.last_vertex_it = last_node->vertex_it;
  state.current_edge = Edge::outgoing_edge(vertices, first_node->vertex_it);
  state.prev_node = first_node;
  state.next_node = nullptr;
  state.last_node = last_node;
  state.chain_decompositions.push_back(ChainDecomposition{first_node, nullptr});
}

template <Winding winding>
void zigzag_iterate(ZigzagState& state)
{
  while (true)
  {
    if (state.forward)
    {
      if (state.direction == HorizontalDirection::right)
      {
        if (!zigzag_forward<winding, HorizontalDirection::right>(state))
        {
          break;
        }
      }
      else
      {
        if (!zigzag_forward<winding, HorizontalDirection::left>(state))
        {
          break;
        }
      }
    }
    else
    {
      if (state.direction == HorizontalDirection::right)
      {
        if (!zigzag_reverse<winding, HorizontalDirection::right>(state))
        {
          break;
        }
      }
      else
      {
        if (!zigzag_reverse<winding, HorizontalDirection::left>(state))
        {
          break;
        }
      }
    }
  }
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_forward(ZigzagState& state)
{
  while (true)
  {
    VerticesView::const_iterator prev_vertex_it = state.current_edge.start_vertex_it;
    state.current_edge.start_vertex_it = state.current_edge.end_vertex_it;
    state.current_edge.end_vertex_it = next_cyclic(state.vertices, state.current_edge.end_vertex_it);

    if (lex_less_than_with_direction<direction>(*state.current_edge.end_vertex_it, *state.current_edge.start_vertex_it))
    {
      if (is_convex_corner<winding>(*prev_vertex_it, *state.current_edge.start_vertex_it,
                                    *state.current_edge.end_vertex_it))
      {
        return zigzag_forward_convex_corner<winding, direction>(state, prev_vertex_it);
      }
      else
      {
        return zigzag_concave_corner<winding, direction>(state);
      }
    }
  }
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_reverse(ZigzagState& state)
{
  while (true)
  {
    if (state.next_node &&
        lex_less_than_with_direction<direction>(*state.next_node->vertex_it, *state.current_edge.end_vertex_it))
    {
      if constexpr ((winding == Winding::ccw) == (direction == HorizontalDirection::right))
      {
        state.next_node->lower_opp_edge = state.current_edge;
      }
      else
      {
        state.next_node->upper_opp_edge = state.current_edge;
      }

      state.prev_node = state.next_node;
      state.next_node = state.next_node->neighbors[0];
    }
    else
    {
      VerticesView::const_iterator prev_vertex_it = state.current_edge.start_vertex_it;
      state.current_edge.start_vertex_it = state.current_edge.end_vertex_it;
      state.current_edge.end_vertex_it = next_cyclic(state.vertices, state.current_edge.end_vertex_it);

      if (lex_less_than_with_direction<direction>(*state.current_edge.end_vertex_it,
                                                  *state.current_edge.start_vertex_it))
      {
        if (is_convex_corner<winding>(*prev_vertex_it, *state.current_edge.start_vertex_it,
                                      *state.current_edge.end_vertex_it))
        {
          return zigzag_reverse_convex_corner<winding, direction>(state);
        }
        else
        {
          return zigzag_concave_corner<winding, direction>(state);
        }
      }
    }
  }
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_forward_convex_corner(ZigzagState& state, VerticesView::const_iterator prev_vertex_it)
{
  constexpr bool incoming_is_lower = (winding == Winding::ccw) == (direction == HorizontalDirection::right);

  if (state.current_edge.start_vertex_it == state.last_vertex_it)
  {
    // We've reached the first vertex, so the current chain ends here.
    //
    // Note that the current chain can't be the same chain as the outgoing chain at state.last_vertex_it,
    // because for that we'd have to be going in the reverse direction.

    DIDA_DEBUG_ASSERT(state.chain_decompositions.size() > 1);
    DIDA_DEBUG_ASSERT(!state.last_node);

    Node* node = state.node_pool->alloc();
    node->direction = other_direction(direction);
    node->type = NodeType::branch;
    node->vertex_it = state.current_edge.start_vertex_it;
    node->lower_opp_edge = Edge::invalid();
    node->upper_opp_edge = Edge::invalid();
    node->neighbors[0] = nullptr;
    node->neighbors[1] = nullptr;
    node->neighbors[2] = nullptr;

    if constexpr (incoming_is_lower)
    {
      node->neighbors[2] = state.prev_node;
      state.prev_node->neighbors[2] = node;
    }
    else
    {
      node->neighbors[1] = state.prev_node;
      state.prev_node->neighbors[1] = node;
    }

    state.chain_decompositions.back().last_node = node;
    return false;
  }

  Node* node = state.node_pool->alloc();
  node->direction = direction;
  node->type = NodeType::leaf;
  node->vertex_it = state.current_edge.start_vertex_it;

  Edge incoming_edge{prev_vertex_it, state.current_edge.start_vertex_it};
  Edge outgoing_edge = state.current_edge;
  node->lower_opp_edge = incoming_is_lower ? incoming_edge : outgoing_edge;
  node->upper_opp_edge = incoming_is_lower ? outgoing_edge : incoming_edge;

  node->neighbors[0] = state.prev_node;
  state.prev_node->neighbors[incoming_is_lower ? 2 : 1] = node;

  state.next_node = state.prev_node;
  state.prev_node = node;

  state.direction = other_direction(direction);
  state.forward = false;

  return true;
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_reverse_convex_corner(ZigzagState& state)
{
  constexpr bool incoming_is_lower = (winding == Winding::ccw) == (direction == HorizontalDirection::right);

  // The zigzag algorithm can't handle convex corners at the end of a reverse edge, so we have to start a new chain.

  if (state.current_edge.start_vertex_it == state.last_vertex_it && state.chain_decompositions.size() == 1)
  {
    // We've reached the first vertex while still in our first chain, so it's a closed chain.

    DIDA_DEBUG_ASSERT(state.next_node == state.chain_decompositions[0].first_node);
    DIDA_DEBUG_ASSERT(!state.last_node);

    // Turn the first node into a leaf node.
    Node* first_node = state.next_node;
    first_node->direction = direction;
    first_node->type = NodeType::leaf;

    Edge incoming_edge{prev_cyclic(state.vertices, first_node->vertex_it), first_node->vertex_it};
    Edge outgoing_edge{first_node->vertex_it, next_cyclic(state.vertices, first_node->vertex_it)};
    first_node->lower_opp_edge = incoming_is_lower ? incoming_edge : outgoing_edge;
    first_node->upper_opp_edge = incoming_is_lower ? outgoing_edge : incoming_edge;

    first_node->neighbors[0] = state.prev_node;
    return false;

    // If state.current_edge.start_vertex_it == state.last_vertex_it, but we're not in the first chain, then we'll
    // proceed to create the closing node of the current chain in the default way, and return afterwards.
  }

  Node* old_chain_last_node = state.node_pool->alloc();
  old_chain_last_node->direction = other_direction(direction);
  old_chain_last_node->type = NodeType::branch;
  old_chain_last_node->vertex_it = state.current_edge.start_vertex_it;
  old_chain_last_node->neighbors[0] = state.next_node;

  if constexpr (incoming_is_lower)
  {
    old_chain_last_node->lower_opp_edge = Edge::invalid();
    old_chain_last_node->neighbors[1] = nullptr;
    old_chain_last_node->neighbors[2] = state.prev_node;

    if (state.next_node)
    {
      EdgeRange upper_edge_range{state.next_node->vertex_it, state.prev_node->upper_opp_edge.end_vertex_it};
      old_chain_last_node->upper_opp_edge = edge_for_point_with_monotone_edge_range<other_direction(direction)>(
          state.vertices, upper_edge_range, *state.current_edge.start_vertex_it);
    }
    else
    {
      old_chain_last_node->upper_opp_edge = Edge::invalid();
    }
  }
  else
  {
    old_chain_last_node->upper_opp_edge = Edge::invalid();
    old_chain_last_node->neighbors[1] = state.prev_node;
    old_chain_last_node->neighbors[2] = nullptr;

    if (state.next_node)
    {
      EdgeRange lower_edge_range{state.next_node->vertex_it, state.prev_node->lower_opp_edge.end_vertex_it};
      old_chain_last_node->lower_opp_edge = edge_for_point_with_monotone_edge_range<other_direction(direction)>(
          state.vertices, lower_edge_range, *state.current_edge.start_vertex_it);
    }
    else
    {
      old_chain_last_node->lower_opp_edge = Edge::invalid();
    }
  }

  state.prev_node->neighbors[0] = old_chain_last_node;

  if (state.next_node)
  {
    state.next_node->neighbors[incoming_is_lower ? 1 : 2] = old_chain_last_node;
  }

  state.chain_decompositions.back().last_node = old_chain_last_node;

  if (state.current_edge.start_vertex_it == state.last_vertex_it)
  {
    DIDA_DEBUG_ASSERT(!state.last_node);

    return false;
  }

  Node* new_chain_first_node = state.node_pool->alloc();
  new_chain_first_node->direction = other_direction(direction);
  new_chain_first_node->type = NodeType::branch;
  new_chain_first_node->vertex_it = state.current_edge.start_vertex_it;
  new_chain_first_node->lower_opp_edge = Edge::invalid();
  new_chain_first_node->upper_opp_edge = Edge::invalid();
  new_chain_first_node->neighbors[0] = nullptr;
  new_chain_first_node->neighbors[1] = nullptr;
  new_chain_first_node->neighbors[2] = nullptr;

  state.chain_decompositions.push_back(ChainDecomposition{new_chain_first_node, nullptr});

  state.direction = other_direction(direction);
  state.forward = true;
  state.prev_node = new_chain_first_node;
  state.next_node = nullptr;

  return true;
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_concave_corner(ZigzagState& state)
{
  constexpr bool incoming_is_lower = (winding == Winding::ccw) == (direction == HorizontalDirection::right);

  if (state.current_edge.start_vertex_it == state.last_vertex_it)
  {
    DIDA_DEBUG_ASSERT(state.last_node);

    state.last_node->neighbors[incoming_is_lower ? 2 : 1] = state.prev_node;

    uint8_t prev_node_branch_index = state.forward ? (incoming_is_lower ? 2 : 1) : 0;
    state.prev_node->neighbors[prev_node_branch_index] = state.last_node;

    state.chain_decompositions.back().last_node = state.last_node;

    return false;
  }

  Node* node = state.node_pool->alloc();
  node->direction = other_direction(direction);
  node->type = NodeType::branch;
  node->vertex_it = state.current_edge.start_vertex_it;

  if constexpr (incoming_is_lower)
  {
    node->lower_opp_edge = Edge::invalid();

    if (state.next_node)
    {
      EdgeRange upper_edge_range{state.next_node->vertex_it, state.prev_node->upper_opp_edge.end_vertex_it};
      node->upper_opp_edge = edge_for_point_with_monotone_edge_range<other_direction(direction)>(
          state.vertices, upper_edge_range, *state.current_edge.start_vertex_it);
    }
    else
    {
      node->upper_opp_edge = Edge::invalid();
    }

    node->neighbors[0] = state.next_node;
    node->neighbors[1] = nullptr;
    node->neighbors[2] = state.prev_node;

    uint8_t prev_node_branch_index = state.forward ? (incoming_is_lower ? 2 : 1) : 0;
    state.prev_node->neighbors[prev_node_branch_index] = node;
    if (state.next_node)
    {
      state.next_node->neighbors[1] = node;
    }
  }
  else
  {
    if (state.next_node)
    {
      EdgeRange lower_edge_range{state.next_node->vertex_it, state.prev_node->lower_opp_edge.end_vertex_it};
      node->lower_opp_edge = edge_for_point_with_monotone_edge_range<other_direction(direction)>(
          state.vertices, lower_edge_range, *state.current_edge.start_vertex_it);
    }
    else
    {
      node->lower_opp_edge = Edge::invalid();
    }

    node->upper_opp_edge = Edge::invalid();

    node->neighbors[0] = state.next_node;
    node->neighbors[1] = state.prev_node;
    node->neighbors[2] = nullptr;

    uint8_t prev_node_branch_index = state.forward ? (incoming_is_lower ? 2 : 1) : 0;
    state.prev_node->neighbors[prev_node_branch_index] = node;
    if (state.next_node)
    {
      state.next_node->neighbors[2] = node;
    }
  }

  state.prev_node = node;
  state.next_node = nullptr;

  state.direction = other_direction(direction);
  state.forward = true;

  return true;
}

template <Winding winding>
bool is_convex_corner(Point2 a, Point2 b, Point2 c)
{
  if constexpr (winding == Winding::ccw)
  {
    return cross(b - a, c - a) > 0;
  }
  else
  {
    return cross(b - a, c - a) < 0;
  }
}

} // namespace

std::vector<ChainDecomposition> interior_zigzag_phase(VerticesView vertices, Winding winding, NodePool& node_pool)
{
  ZigzagState state;

  if (winding == Winding::ccw)
  {
    interior_zigzag_init<Winding::ccw>(state, vertices, node_pool);
    zigzag_iterate<Winding::ccw>(state);
  }
  else
  {
    interior_zigzag_init<Winding::cw>(state, vertices, node_pool);
    zigzag_iterate<Winding::cw>(state);
  }

  return std::move(state.chain_decompositions);
}

ExteriorChainDecompositions exterior_zigzag_phase(VerticesView vertices, Winding winding, NodePool& node_pool)
{
  auto [leftmost_vertex_it, rightmost_vertex_it] = std::minmax_element(vertices.begin(), vertices.end(), lex_less_than);

  Node* leftmost_node = node_pool.alloc();
  leftmost_node->direction = HorizontalDirection::right;
  leftmost_node->type = NodeType::branch;
  leftmost_node->vertex_it = leftmost_vertex_it;
  leftmost_node->lower_opp_edge = Edge::invalid();
  leftmost_node->upper_opp_edge = Edge::invalid();
  leftmost_node->neighbors[0] = nullptr;

  Node* rightmost_node = node_pool.alloc();
  rightmost_node->direction = HorizontalDirection::left;
  rightmost_node->type = NodeType::branch;
  rightmost_node->vertex_it = rightmost_vertex_it;
  rightmost_node->lower_opp_edge = Edge::invalid();
  rightmost_node->upper_opp_edge = Edge::invalid();
  rightmost_node->neighbors[0] = nullptr;

  ExteriorChainDecompositions result;
  result.leftmost_node = leftmost_node;
  result.rightmost_node = rightmost_node;

  if(winding == Winding::ccw)
  {
    ZigzagState state;
    exterior_zigzag_init(state, vertices, leftmost_node, rightmost_node, node_pool);
    zigzag_iterate<Winding::ccw>(state);
    result.upper_chain_decompositions = std::move(state.chain_decompositions);

    exterior_zigzag_init(state, vertices, rightmost_node, leftmost_node, node_pool);
    zigzag_iterate<Winding::ccw>(state);
    result.lower_chain_decompositions = std::move(state.chain_decompositions);
  }
  else
  {
    ZigzagState state;
    exterior_zigzag_init(state, vertices, leftmost_node, rightmost_node, node_pool);
    zigzag_iterate<Winding::cw>(state);
    result.lower_chain_decompositions = std::move(state.chain_decompositions);

    exterior_zigzag_init(state, vertices, rightmost_node, leftmost_node, node_pool);
    zigzag_iterate<Winding::cw>(state);
    result.upper_chain_decompositions = std::move(state.chain_decompositions);
  }

  return result;
}

} // namespace dida::detail::vertical_decomposition