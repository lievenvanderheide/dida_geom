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

  /// The first vertex of the first decomposition.
  VerticesView::const_iterator first_vertex_it;

  /// The current edge. This is the edge which contains the current active point.
  Edge current_edge;

  /// The previous node. This is the node whose vertical extension is the boundary of the current region in the
  /// direction opposite to @c direction. @c prev_node is always set.
  Node* prev_node;

  /// The next node. This is the node whose vertical extension is the boundary of the current region in the
  /// direction of @c direction. @c next_node can be @c nullptr.
  Node* next_node;

  /// The output vector of chain decompositions. The last chain in this array is the chain we're currently building.
  std::vector<ChainDecomposition> chain_decompositions;
};

/// Initializes a @c ZigzagState. This function finds a convex side vertex to start the first chain with, and
/// initializes the @c ZigzagState to the state with this vertex as its active point.
///
///
/// @param state The @c ZigzagState. On call, the @c vertices and @c node_pool members must be set, on return, the full
/// @c ZigzagState will be in a valid state.
void zigzag_init(ZigzagState& state);

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
template <HorizontalDirection direction>
void zigzag_concave_corner(ZigzagState& state);

/// Returns whether the corner at @c b, with previous vertex @c a and next vertex @c c is a convex corner.
template <Winding winding>
bool is_convex_corner(Point2 a, Point2 b, Point2 c);

template <Winding winding>
void zigzag_init(ZigzagState& state)
{
  for (VerticesView::const_iterator it = state.vertices.begin(); it != state.vertices.end(); ++it)
  {
    VerticesView::const_iterator prev_it = prev_cyclic(state.vertices, it);
    VerticesView::const_iterator next_it = next_cyclic(state.vertices, it);

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
        state.first_vertex_it = it;
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
        zigzag_concave_corner<winding, direction>(state);
        return true;
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
          zigzag_concave_corner<winding, direction>(state);
          return true;
        }
      }
    }
  }
}

template <Winding winding, HorizontalDirection direction>
bool zigzag_forward_convex_corner(ZigzagState& state, VerticesView::const_iterator prev_vertex_it)
{
  constexpr bool incoming_is_lower = (winding == Winding::ccw) == (direction == HorizontalDirection::right);

  if (state.current_edge.start_vertex_it == state.first_vertex_it)
  {
    // We've reached the first vertex, so the current chain ends here.
    //
    // Note that the current chain can't be the same chain as the outgoing chain at state.first_vertex_it,
    // because for that we'd have to be going in the reverse direction.

    DIDA_DEBUG_ASSERT(state.chain_decompositions.size() > 1);

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

  if (state.current_edge.start_vertex_it == state.first_vertex_it && state.chain_decompositions.size() == 1)
  {
    // We've reached the first vertex while still in our first chain, so it's a closed chain.

    DIDA_DEBUG_ASSERT(state.next_node == state.chain_decompositions[0].first_node);

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

  if (state.current_edge.start_vertex_it == state.first_vertex_it)
  {
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
void zigzag_concave_corner(ZigzagState& state)
{
  constexpr bool incoming_is_lower = (winding == Winding::ccw) == (direction == HorizontalDirection::right);

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

template <Winding winding>
std::vector<ChainDecomposition> vertical_decomposition_zigzag_phase(VerticesView vertices, NodePool& node_pool)
{
  ZigzagState state;
  state.vertices = vertices;
  state.node_pool = &node_pool;

  zigzag_init<winding>(state);

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

  return std::move(state.chain_decompositions);
}

} // namespace

std::vector<ChainDecomposition> vertical_decomposition_zigzag_phase(VerticesView vertices, Winding winding,
                                                                    NodePool& node_pool)
{
  if (winding == Winding::ccw)
  {
    return vertical_decomposition_zigzag_phase<Winding::ccw>(vertices, node_pool);
  }
  else
  {
    return vertical_decomposition_zigzag_phase<Winding::cw>(vertices, node_pool);
  }
}

} // namespace dida::detail::vertical_decomposition