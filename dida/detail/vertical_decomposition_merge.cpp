#include "dida/detail/vertical_decomposition_merge.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

struct ChainMergeState
{
  Node* next;
  Node* prev;

  Node* opp_last;
  uint8_t opp_last_branch_index;

  Edge edge;
  Edge opp_edge;
};

struct MergeState
{
  VerticesView vertices;
  NodePool& node_pool;

  HorizontalDirection direction;

  ChainMergeState a;
  ChainMergeState b;

  Node* last_merged;
  uint8_t last_merged_branch_index;
};

void init_merge(MergeState& merge_state, Node* a_node, Node* b_node);

template <HorizontalDirection direction>
bool merge_iteration(MergeState& merge_state);

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_forward_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q);

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_reverse_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q);

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_reverse_outer_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q);

template <HorizontalDirection direction, bool p_is_lower>
void merge_tail(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q);

template <bool a_is_lower>
void merge_closing_vertex(MergeState& merge_state);

void push_merged_node(MergeState& merge_state, Node* node, uint8_t incoming_branch_index,
                      uint8_t outgoing_branch_index);

void push_opp_node(ChainMergeState& chain_merge_state, Node* node, uint8_t incoming_branch_index,
                   uint8_t outgoing_branch_index);

template <HorizontalDirection direction, bool chain_is_lower>
void advance_edge(MergeState& merge_state, ChainMergeState& chain_merge_state, Point2 point);

template <HorizontalDirection direction, bool chain_is_lower>
void advance_opp_edge(MergeState& merge_state, ChainMergeState& chain_merge_state, Point2 point);

uint8_t neighbor_branch_index(const Node* node, const Node* neighbor);

void replace_neighbor(Node* node, Node* neighbor, Node* new_neighbor);

void init_merge(MergeState& merge_state, Node* a_node, Node* b_node)
{
  VertexIt vertex_it = a_node->vertex_it;
  VertexIt prev_vertex_it = prev_cyclic(merge_state.vertices, vertex_it);
  VertexIt next_vertex_it = next_cyclic(merge_state.vertices, vertex_it);

  merge_state.direction = a_node->direction;
  DIDA_DEBUG_ASSERT(a_node->direction == b_node->direction);

  bool a_is_lower = merge_state.direction == HorizontalDirection::left;
  bool b_is_lower = merge_state.direction == HorizontalDirection::right;

  merge_state.a.prev = a_node;
  merge_state.a.next = a_node->neighbors[a_is_lower ? 2 : 1];
  merge_state.a.edge = Edge{prev_vertex_it, vertex_it};
  merge_state.a.opp_edge = a_is_lower ? a_node->upper_opp_edge : a_node->lower_opp_edge;

  merge_state.b.prev = b_node;
  merge_state.b.next = b_node->neighbors[b_is_lower ? 2 : 1];
  merge_state.b.edge = Edge{vertex_it, next_vertex_it};
  merge_state.b.opp_edge = b_is_lower ? b_node->upper_opp_edge : b_node->lower_opp_edge;

  // If both a_node and b_node have a neighbor[0], then the two chains would intersect, so that can't happen.
  DIDA_DEBUG_ASSERT(!a_node->neighbors[0] || !b_node->neighbors[0]);

  Node* leaf_node;
  if (a_node->neighbors[0])
  {
    if (a_node->lower_opp_edge.is_valid() && a_node->upper_opp_edge.is_valid())
    {
      a_node->type = NodeType::outer_branch;

      merge_state.a.opp_last = a_node;
      merge_state.a.opp_last_branch_index = a_is_lower ? 2 : 1;
      merge_state.b.opp_last = nullptr;
    }
    else
    {
      merge_state.a.opp_last = a_node->neighbors[0];
      merge_state.a.opp_last_branch_index = neighbor_branch_index(a_node->neighbors[0], a_node);
      merge_state.b.opp_last = nullptr;
    }

    leaf_node = b_node;
  }
  else if (b_node->neighbors[0])
  {
    if (b_node->lower_opp_edge.is_valid() && b_node->upper_opp_edge.is_valid())
    {
      b_node->type = NodeType::outer_branch;

      merge_state.a.opp_last = nullptr;
      merge_state.b.opp_last = b_node;
      merge_state.b.opp_last_branch_index = b_is_lower ? 2 : 1;
    }
    else
    {
      merge_state.a.opp_last = nullptr;
      merge_state.b.opp_last = b_node->neighbors[0];
      merge_state.b.opp_last_branch_index = neighbor_branch_index(b_node->neighbors[0], b_node);
    }

    leaf_node = a_node;
  }
  else
  {
    merge_state.a.opp_last = nullptr;
    merge_state.b.opp_last = nullptr;

    leaf_node = a_node;
  }

  merge_state.last_merged = leaf_node;
  merge_state.last_merged_branch_index = 0;

  leaf_node->direction = other_direction(merge_state.direction);
  leaf_node->type = NodeType::leaf;
  leaf_node->lower_opp_edge = a_is_lower ? merge_state.a.edge : merge_state.b.edge;
  leaf_node->upper_opp_edge = a_is_lower ? merge_state.b.edge : merge_state.a.edge;
}

template <HorizontalDirection direction>
bool merge_iteration(MergeState& merge_state)
{
  constexpr bool a_is_lower = direction == HorizontalDirection::left;
  constexpr bool b_is_lower = direction == HorizontalDirection::right;

  if (!merge_state.b.next)
  {
    merge_tail<direction, a_is_lower>(merge_state, merge_state.a, merge_state.b);
    return false;
  }

  if (!merge_state.a.next)
  {
    merge_tail<direction, b_is_lower>(merge_state, merge_state.b, merge_state.a);
    return false;
  }

  if (merge_state.a.next->vertex_it == merge_state.b.next->vertex_it)
  {
    merge_closing_vertex<a_is_lower>(merge_state);
    return false;
  }

  if (lex_less_than_with_direction<direction>(*merge_state.a.next->vertex_it, *merge_state.b.next->vertex_it))
  {
    if (merge_state.a.next->direction == direction)
    {
      merge_iteration_forward_branch<direction, a_is_lower>(merge_state, merge_state.a, merge_state.b);
    }
    else if (merge_state.a.next->type == NodeType::branch)
    {
      merge_iteration_reverse_branch<direction, a_is_lower>(merge_state, merge_state.a, merge_state.b);
    }
    else
    {
      DIDA_DEBUG_ASSERT(merge_state.a.next->type == NodeType::outer_branch);
      merge_iteration_reverse_outer_branch<direction, a_is_lower>(merge_state, merge_state.a, merge_state.b);
    }
  }
  else
  {
    if (merge_state.b.next->direction == direction)
    {
      merge_iteration_forward_branch<direction, b_is_lower>(merge_state, merge_state.b, merge_state.a);
    }
    else if (merge_state.b.next->type == NodeType::branch)
    {
      merge_iteration_reverse_branch<direction, b_is_lower>(merge_state, merge_state.b, merge_state.a);
    }
    else
    {
      DIDA_DEBUG_ASSERT(merge_state.b.next->type == NodeType::outer_branch);
      merge_iteration_reverse_outer_branch<direction, b_is_lower>(merge_state, merge_state.b, merge_state.a);
    }
  }

  return true;
}

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_forward_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q)
{
  advance_edge<direction, !p_is_lower>(merge_state, q, *p.next->vertex_it);

  if (q.edge.on_interior_side(*p.next->vertex_it))
  {
    if constexpr (p_is_lower)
    {
      p.opp_edge = p.next->upper_opp_edge;
      p.next->upper_opp_edge = q.edge;
    }
    else
    {
      p.opp_edge = p.next->lower_opp_edge;
      p.next->lower_opp_edge = q.edge;
    }

    p.edge = p_is_lower == (direction == HorizontalDirection::right)
                 ? Edge{p.next->vertex_it, next_cyclic(merge_state.vertices, p.next->vertex_it)}
                 : Edge{prev_cyclic(merge_state.vertices, p.next->vertex_it), p.next->vertex_it};

    push_merged_node(merge_state, p.next, 0, p_is_lower ? 2 : 1);

    p.prev = p.next;
    p.next = p.next->neighbors[p_is_lower ? 2 : 1];
  }
  else
  {
    if constexpr (p_is_lower)
    {
      p.edge = p.next->lower_opp_edge;
      p.next->lower_opp_edge = Edge::invalid();
    }
    else
    {
      p.edge = p.next->upper_opp_edge;
      p.next->upper_opp_edge = Edge::invalid();
    }

    p.opp_edge = p_is_lower == (direction == HorizontalDirection::right)
                     ? Edge{prev_cyclic(merge_state.vertices, p.next->vertex_it), p.next->vertex_it}
                     : Edge{p.next->vertex_it, next_cyclic(merge_state.vertices, p.next->vertex_it)};

    push_opp_node(p, p.next, 0, p_is_lower ? 1 : 2);

    p.prev = p.next;
    p.next = p.next->neighbors[p_is_lower ? 1 : 2];
  }
}

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_reverse_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q)
{
  bool p_vertex_in_front_of_q = p_is_lower == (p.next->neighbors[2] == p.prev);

  if (p_vertex_in_front_of_q)
  {
    advance_edge<direction, !p_is_lower>(merge_state, q, *p.next->vertex_it);

    bool should_turn_around;
    if (q.opp_edge.is_valid())
    {
      advance_opp_edge<direction, !p_is_lower>(merge_state, q, *p.next->vertex_it);

      Edge p_opp_edge = p_is_lower ? p.next->upper_opp_edge : p.next->lower_opp_edge;
      if (p_opp_edge.is_valid())
      {
        YOnEdge y_on_p_opp_edge = y_on_edge_for_x(p_opp_edge.segment(), p.next->vertex_it->x());
        YOnEdge y_on_q_opp_edge = y_on_edge_for_x(q.opp_edge.segment(), p.next->vertex_it->x());
        should_turn_around = p_is_lower ? y_on_q_opp_edge < y_on_p_opp_edge : y_on_q_opp_edge > y_on_p_opp_edge;
      }
      else
      {
        should_turn_around = true;
      }
    }
    else
    {
      should_turn_around = false;
    }

    if (should_turn_around)
    {
      // The current branch vertex is visible from q.edge, and we're turning around.

      push_merged_node(merge_state, p.next, p_is_lower ? 2 : 1, p_is_lower ? 1 : 2);

      if (p.opp_last)
      {
        Node* new_node = merge_state.node_pool.alloc();
        new_node->direction == p.next->direction;
        new_node->type = NodeType::outer_branch;
        new_node->vertex_it = p.next->vertex_it;
        new_node->lower_opp_edge = p.next->lower_opp_edge;
        new_node->upper_opp_edge = p.next->upper_opp_edge;

        new_node->neighbors[0] = p.next->neighbors[0];
        replace_neighbor(new_node->neighbors[0], p.next, new_node);

        new_node->neighbors[p_is_lower ? 2 : 1] = p.opp_last;
        p.opp_last->neighbors[p.opp_last_branch_index] = new_node;

        p.opp_last = new_node;
        p.opp_last_branch_index = p_is_lower ? 1 : 2;
      }
      else
      {
        p.opp_last = p.next->neighbors[0];
        if (p.opp_last)
        {
          p.opp_last_branch_index = neighbor_branch_index(p.opp_last, p.next);
        }
      }

      if constexpr (p_is_lower)
      {
        p.opp_edge = p.next->lower_opp_edge;
        p.next->upper_opp_edge = q.edge;
        p.next->lower_opp_edge = q.opp_edge;
      }
      else
      {
        p.opp_edge = p.next->upper_opp_edge;
        p.next->lower_opp_edge = q.edge;
        p.next->upper_opp_edge = q.opp_edge;
      }

      p.edge = p_is_lower == (direction == HorizontalDirection::right)
                   ? Edge{p.next->vertex_it, next_cyclic(merge_state.vertices, p.next->vertex_it)}
                   : Edge{prev_cyclic(merge_state.vertices, p.next->vertex_it), p.next->vertex_it};

      q.edge = q.opp_edge;
      q.opp_edge = Edge::invalid();

      merge_state.direction = other_direction(direction);

      p.next->neighbors[0] = q.next;
      replace_neighbor(q.next, q.prev, p.next);

      p.prev = p.next;
      p.next = p.next->neighbors[p_is_lower ? 1 : 2];

      q.prev = q.next;
      q.next = q.opp_last;
      q.next->neighbors[q.opp_last_branch_index] = q.prev;
      q.opp_last = nullptr;
    }
    else
    {
      // The branch vertex is visible from q.edge, but we're not turning around.

      if (p_is_lower)
      {
        p.edge = p.next->lower_opp_edge;
        p.opp_edge = p.next->upper_opp_edge;
        p.next->upper_opp_edge = q.edge;
      }
      else
      {
        p.edge = p.next->upper_opp_edge;
        p.opp_edge = p.next->lower_opp_edge;
        p.next->lower_opp_edge = q.edge;
      }

      push_merged_node(merge_state, p.next, p_is_lower ? 2 : 1, 0);

      p.prev = p.next;
      p.next = p.next->neighbors[0];
    }
  }
  else
  {
    // The branch vertex is invisible from the current edge of 'q'.

    if constexpr (p_is_lower)
    {
      p.edge = p.next->lower_opp_edge;
      p.opp_edge = p.next->upper_opp_edge;
      p.next->lower_opp_edge = Edge::invalid();
    }
    else
    {
      p.edge = p.next->upper_opp_edge;
      p.opp_edge = p.next->lower_opp_edge;
      p.next->upper_opp_edge = Edge::invalid();
    }

    push_opp_node(p, p.next, p_is_lower ? 1 : 2, 0);

    p.prev = p.next;
    p.next = p.next->neighbors[0];
  }
}

template <HorizontalDirection direction, bool p_is_lower>
void merge_iteration_reverse_outer_branch(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q)
{
  Node* node = p.next;

  if constexpr (p_is_lower)
  {
    p.edge = node->lower_opp_edge;
    p.opp_edge = node->upper_opp_edge;
    p.opp_last = node->neighbors[2];
  }
  else
  {
    p.edge = node->upper_opp_edge;
    p.opp_edge = node->lower_opp_edge;
    p.opp_last = node->neighbors[2];
  }

  p.next = node->neighbors[0];
  replace_neighbor(p.next, node, p.prev);

  p.opp_last_branch_index = neighbor_branch_index(p.opp_last, node);
}

template <HorizontalDirection direction, bool p_is_lower>
void merge_tail(MergeState& merge_state, ChainMergeState& p, ChainMergeState& q)
{
  DIDA_DEBUG_ASSERT(!q.next);
  DIDA_DEBUG_ASSERT(!q.opp_last);

  if (p.opp_last)
  {
    advance_opp_edge<direction, p_is_lower>(merge_state, p, *merge_state.last_merged->vertex_it);

    Node* node = merge_state.node_pool.alloc();
    node->direction = other_direction(direction);
    node->type = NodeType::outer_branch;
    node->vertex_it = merge_state.last_merged->vertex_it;

    if constexpr (p_is_lower)
    {
      node->lower_opp_edge = merge_state.last_merged->lower_opp_edge;
      node->upper_opp_edge = p.opp_edge;

      node->neighbors[0] = p.next;
      node->neighbors[1] = merge_state.last_merged;
      node->neighbors[2] = p.opp_last;
    }
    else
    {
      node->lower_opp_edge = p.opp_edge;
      node->upper_opp_edge = merge_state.last_merged->upper_opp_edge;

      node->neighbors[0] = p.next;
      node->neighbors[1] = p.opp_last;
      node->neighbors[2] = merge_state.last_merged;
    }

    replace_neighbor(p.next, p.prev, node);
    p.opp_last->neighbors[p.opp_last_branch_index] = node;
    merge_state.last_merged->neighbors[merge_state.last_merged_branch_index] = node;
  }
  else
  {
    merge_state.last_merged->neighbors[merge_state.last_merged_branch_index] = p.next;
    replace_neighbor(p.next, p.prev, merge_state.last_merged);
  }
}

template <bool a_is_lower>
void merge_closing_vertex(MergeState& merge_state)
{
  Node* a_node = merge_state.a.next;
  Node* b_node = merge_state.b.next;

  a_node->direction = merge_state.direction;
  a_node->type = NodeType::leaf;

  Edge incoming_edge{prev_cyclic(merge_state.vertices, a_node->vertex_it), a_node->vertex_it};
  Edge outgoing_edge{a_node->vertex_it, next_cyclic(merge_state.vertices, a_node->vertex_it)};

  if constexpr (a_is_lower)
  {
    a_node->lower_opp_edge = outgoing_edge;
    a_node->upper_opp_edge = incoming_edge;
  }
  else
  {
    a_node->lower_opp_edge = incoming_edge;
    a_node->upper_opp_edge = outgoing_edge;
  }

  merge_state.last_merged->neighbors[merge_state.last_merged_branch_index] = a_node;
  a_node->neighbors[0] = merge_state.last_merged;
}

void push_merged_node(MergeState& merge_state, Node* node, uint8_t incoming_branch_index, uint8_t outgoing_branch_index)
{
  merge_state.last_merged->neighbors[merge_state.last_merged_branch_index] = node;
  node->neighbors[incoming_branch_index] = merge_state.last_merged;

  merge_state.last_merged = node;
  merge_state.last_merged_branch_index = outgoing_branch_index;
}

void push_opp_node(ChainMergeState& chain_merge_state, Node* node, uint8_t incoming_branch_index,
                   uint8_t outgoing_branch_index)
{
  if (chain_merge_state.opp_last)
  {
    chain_merge_state.opp_last->neighbors[chain_merge_state.opp_last_branch_index] = node;
  }

  node->neighbors[incoming_branch_index] = chain_merge_state.opp_last;

  chain_merge_state.opp_last = node;
  chain_merge_state.opp_last_branch_index = outgoing_branch_index;
}

template <HorizontalDirection direction, bool chain_is_lower>
void advance_edge(MergeState& merge_state, ChainMergeState& chain_merge_state, Point2 point)
{
  if constexpr (chain_is_lower == (direction == HorizontalDirection::right))
  {
    while (lex_less_than_with_direction<direction>(*chain_merge_state.edge.end_vertex_it, point))
    {
      chain_merge_state.edge.start_vertex_it = chain_merge_state.edge.end_vertex_it;
      chain_merge_state.edge.end_vertex_it = next_cyclic(merge_state.vertices, chain_merge_state.edge.end_vertex_it);
    }
  }
  else
  {
    while (lex_less_than_with_direction<direction>(*chain_merge_state.edge.start_vertex_it, point))
    {
      chain_merge_state.edge.end_vertex_it = chain_merge_state.edge.start_vertex_it;
      chain_merge_state.edge.start_vertex_it =
          prev_cyclic(merge_state.vertices, chain_merge_state.edge.start_vertex_it);
    }
  }
}

template <HorizontalDirection direction, bool chain_is_lower>
void advance_opp_edge(MergeState& merge_state, ChainMergeState& chain_merge_state, Point2 point)
{
  if constexpr (chain_is_lower == (direction == HorizontalDirection::right))
  {
    while (lex_less_than_with_direction<direction>(*chain_merge_state.opp_edge.start_vertex_it, point))
    {
      chain_merge_state.opp_edge.end_vertex_it = chain_merge_state.opp_edge.start_vertex_it;
      chain_merge_state.opp_edge.start_vertex_it =
          prev_cyclic(merge_state.vertices, chain_merge_state.opp_edge.start_vertex_it);
    }
  }
  else
  {
    while (lex_less_than_with_direction<direction>(*chain_merge_state.opp_edge.end_vertex_it, point))
    {
      chain_merge_state.opp_edge.start_vertex_it = chain_merge_state.opp_edge.end_vertex_it;
      chain_merge_state.opp_edge.end_vertex_it =
          next_cyclic(merge_state.vertices, chain_merge_state.opp_edge.end_vertex_it);
    }
  }
}

uint8_t neighbor_branch_index(const Node* node, const Node* neighbor)
{
  return node->neighbors[0] == neighbor ? 0 : (node->neighbors[1] == neighbor ? 1 : 2);
}

void replace_neighbor(Node* node, Node* neighbor, Node* new_neighbor)
{
  if (node->neighbors[0] == neighbor)
  {
    node->neighbors[0] = new_neighbor;
  }
  else if (node->neighbors[1] == neighbor)
  {
    node->neighbors[1] = new_neighbor;
  }
  else
  {
    DIDA_DEBUG_ASSERT(node->neighbors[2] == neighbor);
    node->neighbors[2] = new_neighbor;
  }
}

#if 0

VertexIt select_edge_range_start_vertex(VertexIt candidate_0, VertexIt candidate_1, VertexIt end_vertex_it)
{
  if (!candidate_1)
  {
    return candidate_0;
  }

  return cyclic_order(candidate_0, candidate_1, end_vertex_it) ? candidate_1 : candidate_0;
}

VertexIt select_edge_range_end_vertex(VertexIt start_vertex_it, VertexIt candidate_0, VertexIt candidate_1)
{
  if (!candidate_1)
  {
    return candidate_0;
  }

  return cyclic_order(start_vertex_it, candidate_0, candidate_1) ? candidate_0 : candidate_1;
}

Edge edge_for_point(MergeState& merge_state, ChainMergeState& chain_merge_state, bool is_lower_chain, Point2 point)
{
  Node* next = chain_merge_state.next;

  EdgeRange edge_range;

  if (merge_state.direction == HorizontalDirection::left)
  {
    if (is_lower_chain)
    {
      edge_range.start_vertex_it = select_edge_range_start_vertex(next->vertex_it, next->lower_opp_edge.start_vertex_it,
                                                                  chain_merge_state.edge_range_it);
      edge_range.end_vertex_it = chain_merge_state.edge_range_it;
    }
    else
    {
      edge_range.start_vertex_it = chain_merge_state.edge_range_it;
      edge_range.end_vertex_it = select_edge_range_end_vertex(chain_merge_state.edge_range_it, next->vertex_it,
                                                              next->upper_opp_edge.end_vertex_it);
    }

    return edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(merge_state.vertices, edge_range, point);
  }
  else
  {
    if (is_lower_chain)
    {
      edge_range.start_vertex_it = chain_merge_state.edge_range_it;
      edge_range.end_vertex_it = select_edge_range_end_vertex(chain_merge_state.edge_range_it, next->vertex_it,
                                                              next->lower_opp_edge.end_vertex_it);
    }
    else
    {
      edge_range.start_vertex_it = select_edge_range_start_vertex(next->vertex_it, next->upper_opp_edge.start_vertex,
                                                                  chain_merge_state.edge_range_it);
      edge_range.end_vertex_it = chain_merge_state.edge_range_it;
    }

    return edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(merge_state.vertices, edge_range, point);
  }
}

Edge opp_edge_for_point(MergeState& merge_state, ChainMergeState& chain_merge_state, bool is_lower_chain, Point2 point)
{
  if (!chain_merge_state.opp_last)
  {
    return Edge::invalid();
  }

  EdgeRange opp_edge_range;
  if (merge_state.direction == HorizontalDirection::left)
  {
    if (is_lower_chain)
    {
      opp_edge_range.start_vertex_it = chain_merge_state.next->lower_opp_edge.start_vertex_it;
      opp_edge_range.end_vertex_it = merge_state.last_merged->lower_opp_edge.end_vertex_it;
    }
    else
    {
      opp_edge_range.start_vertex_it = merge_state.last_merged->upper_opp_edge.start_vertex_it;
      opp_edge_range.end_vertex_it = chain_merge_state.next->upper_opp_edge.end_vertex;
    }

    return edge_for_point_with_monotone_edge_range<HorizontalDirection::left>(merge_state.vertices, edge_range, point);
  }
  else
  {
    if (is_lower_chain)
    {
      opp_edge_range.start_vertex_it = chain_merge_state.next->upper_opp_edge.start_vertex_it;
      opp_edge_range.end_vertex_it = chain_merge_state.opp_last->upper_opp chain_merge_state.opp_last
                                         ->upper_opp_edge merge_state.last_merged->lower_opp_edge.start_vertex_it;
      opp_edge_range.end_vertex_it = chain_merge_state.next->lower_opp_edge.end_vertex_it;
    }
    else
    {
      opp_edge_range.start_vertex_it = chain_merge_state.next->upper_opp_edge.start_vertex_it;
      opp_edge_range.end_vertex_it = merge_state.last_merged->upper_opp_edge.end_vertex_it;
    }

    return edge_for_point_with_monotone_edge_range<HorizontalDirection::right>(merge_state.vertices, edge_range, point);
  }
}

#endif

} // namespace

ChainDecomposition merge_chain_decompositions(VerticesView vertices, NodePool& node_pool, const ChainDecomposition& a,
                                              const ChainDecomposition& b)
{
  DIDA_DEBUG_ASSERT(a.last_node->vertex_it == b.first_node->vertex_it);

  MergeState merge_state{vertices, node_pool};

  init_merge(merge_state, a.last_node, b.first_node);

  while (true)
  {
    if (merge_state.direction == HorizontalDirection::left)
    {
      if (!merge_iteration<HorizontalDirection::left>(merge_state))
      {
        break;
      }
    }
    else
    {
      if (!merge_iteration<HorizontalDirection::right>(merge_state))
      {
        break;
      }
    }
  }

  return ChainDecomposition{a.first_node, b.last_node};
}

} // namespace dida::detail::vertical_decomposition