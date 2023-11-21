#include "dida/detail/vertical_decomposition/sweep_line_builder.hpp"

#include <algorithm>

#include "dida/detail/polygon_sweep.hpp"
#include "dida/utils.hpp"

namespace dida::detail::vertical_decomposition
{

namespace
{

/// An edge which currently intersects the sweep line.
struct ActiveEdgeBase
{
  /// The node on the left side of the region above this edge, or @c nullptr
  ///
  /// This value is only defined if the area immediately above this edge belongs to the area (interior/exterior) we're
  /// decomposing.
  Node* region_left_node;

  /// The index of the branch of @c region_left_node corresponding to the region above this edge.
  ///
  /// This value is only defined when @c region_left_node is defined and not equal to @c nullptr.
  uint8_t region_left_node_branch_index;
};

using ActiveEdge = polygon_sweep::ActiveEdge<ActiveEdgeBase>;
using ActiveEdgesIt = polygon_sweep::ActiveEdgesIt<ActiveEdgeBase>;

/// The state of the sweep line builder.
struct SweepState : polygon_sweep::SweepState<ActiveEdgeBase>
{
  /// The type of the decomposition we're building.
  VerticalDecompositionType decomposition_type;

  /// An iterator pointing to the next node to write.
  std::vector<Node>::iterator nodes_it;
};

/// Converts an @c ActiveEdge to a vertical decomposition edge.
Edge to_vd_edge(const ActiveEdge& edge)
{
  // One of the following 2 holds:
  //
  //  1. next_cyclic(vertices, left_vertex_it) == right_vertex_it
  //  2. next_cyclic(vertices, right_vertex_it) == left_vertex_it
  //

  if (edge.left_vertex_it + 1 == edge.right_vertex_it)
  {
    // Case 1 holds, without wrapping.
    return Edge{edge.left_vertex_it, edge.right_vertex_it};
  }
  else if (edge.right_vertex_it + 1 == edge.left_vertex_it)
  {
    // Case 2 holds, without wrapping.
    return Edge{edge.right_vertex_it, edge.left_vertex_it};
  }
  else
  {
    // There was wrapping, so one of the two vertices must be vertices.last() while the other one must be
    // vertices.first(). vertices.last() is the first vertex of the edge, while vertices.last = next_cyclic(vertices,
    // vertices.last()) the second vertex.
    return edge.left_vertex_it < edge.right_vertex_it ? Edge{edge.right_vertex_it, edge.left_vertex_it}
                                                      : Edge{edge.left_vertex_it, edge.right_vertex_it};
  }
}

void handle_appear_event(SweepState& sweep_state, const polygon_sweep::Event& event)
{
  size_t range_begin, range_end;
  if (sweep_state.decomposition_type == VerticalDecompositionType::interior_decomposition)
  {
    // We're computing the interior decomposition, so the insert location can be anywhere.
    range_begin = 0;
    range_end = sweep_state.active_edges.size();
  }
  else
  {
    // We're computing the exterior decomposition, so the insert location can't be below the pseudo edge at negative
    // infinity, nor above the pseudo edge at positive infinity.
    range_begin = 1;
    range_end = sweep_state.active_edges.size() - 1;
  }

  polygon_sweep::InsertLocation<ActiveEdgeBase> appear_location =
      polygon_sweep::insert_location(sweep_state, range_begin, range_end, *event.vertex_it);
  ActiveEdgesIt it = polygon_sweep::handle_appear_event(sweep_state, appear_location, event);

  if (event.is_concave_corner == (sweep_state.decomposition_type == VerticalDecompositionType::interior_decomposition))
  {
    // This current vertex corresponds to a right branch.

    ActiveEdge& lower_opp_edge = *(it - 1);
    ActiveEdge& upper_outgoing_edge = *(it + 1);
    ActiveEdge& upper_opp_edge = *(it + 2);

    Node& new_node = *(sweep_state.nodes_it++);
    new_node.direction = HorizontalDirection::right;
    new_node.type = NodeType::branch;
    new_node.vertex_it = event.vertex_it;
    new_node.lower_opp_edge = to_vd_edge(lower_opp_edge);
    new_node.upper_opp_edge = to_vd_edge(upper_opp_edge);
    new_node.neighbors[0] = lower_opp_edge.region_left_node;
    new_node.neighbors[1] = nullptr;
    new_node.neighbors[2] = nullptr;

    if (lower_opp_edge.region_left_node)
    {
      lower_opp_edge.region_left_node->neighbors[lower_opp_edge.region_left_node_branch_index] = &new_node;
    }

    // The region above lower_opp_edge is now the lower outgoing region.
    lower_opp_edge.region_left_node = &new_node;
    lower_opp_edge.region_left_node_branch_index = 1;

    // The region above the upper outgoing edge is the upper outgoing region.
    upper_outgoing_edge.region_left_node = &new_node;
    upper_outgoing_edge.region_left_node_branch_index = 2;
  }
  else
  {
    // The current vertex corresponds to a left leaf.

    ActiveEdge& lower_outgoing_edge = *it;
    ActiveEdge& upper_outgoing_edge = *(it + 1);

    Node& new_node = *(sweep_state.nodes_it++);
    new_node.direction = HorizontalDirection::left;
    new_node.type = NodeType::leaf;
    new_node.vertex_it = event.vertex_it;
    new_node.lower_opp_edge = to_vd_edge(lower_outgoing_edge);
    new_node.upper_opp_edge = to_vd_edge(upper_outgoing_edge);
    new_node.neighbors[0] = nullptr;
    new_node.neighbors[1] = nullptr;
    new_node.neighbors[2] = nullptr;

    lower_outgoing_edge.region_left_node = &new_node;
    lower_outgoing_edge.region_left_node_branch_index = 0;
  }
}

void handle_transition_event(SweepState& sweep_state, const polygon_sweep::Event& event)
{
  ActiveEdgesIt it = polygon_sweep::active_edge_with_right_vertex(sweep_state, event.vertex_it);
  polygon_sweep::handle_transition_event(sweep_state, it, event);
}

void handle_vanish_event(SweepState& sweep_state, const polygon_sweep::Event& event)
{
  ActiveEdgesIt it = polygon_sweep::active_edge_with_right_vertex(sweep_state, event.vertex_it);

  if (event.is_concave_corner == (sweep_state.decomposition_type == VerticalDecompositionType::interior_decomposition))
  {
    // The current vertex corresponds to a left branch.

    ActiveEdge& lower_opp_edge = *(it - 1);
    ActiveEdge& upper_opp_edge = *(it + 2);
    ActiveEdge& upper_vanishing_edge = *(it + 1);

    Node& node = *(sweep_state.nodes_it++);
    node.direction = HorizontalDirection::left;
    node.type = NodeType::branch;
    node.vertex_it = event.vertex_it;
    node.lower_opp_edge = to_vd_edge(lower_opp_edge);
    node.upper_opp_edge = to_vd_edge(upper_opp_edge);
    node.neighbors[0] = nullptr;
    node.neighbors[1] = lower_opp_edge.region_left_node;
    node.neighbors[2] = upper_vanishing_edge.region_left_node;

    lower_opp_edge.region_left_node->neighbors[lower_opp_edge.region_left_node_branch_index] = &node;
    upper_vanishing_edge.region_left_node->neighbors[upper_vanishing_edge.region_left_node_branch_index] = &node;

    lower_opp_edge.region_left_node = &node;
    lower_opp_edge.region_left_node_branch_index = 0;
  }
  else
  {
    // The current vertex corresponds to a right leaf.

    ActiveEdge& lower_vanishing_edge = *it;
    ActiveEdge& upper_vanishing_edge = *(it + 1);

    Node& node = *(sweep_state.nodes_it++);
    node.direction = HorizontalDirection::right;
    node.type = NodeType::leaf;
    node.vertex_it = event.vertex_it;
    node.lower_opp_edge = to_vd_edge(lower_vanishing_edge);
    node.upper_opp_edge = to_vd_edge(upper_vanishing_edge);
    node.neighbors[0] = lower_vanishing_edge.region_left_node;
    node.neighbors[1] = nullptr;
    node.neighbors[2] = nullptr;

    lower_vanishing_edge.region_left_node->neighbors[lower_vanishing_edge.region_left_node_branch_index] = &node;
  }

  polygon_sweep::handle_vanish_event(sweep_state, it);
}

} // namespace

VerticalDecomposition vertical_decomposition_with_sweep_line_builder(VerticesView vertices,
                                                                     VerticalDecompositionType decomposition_type)
{
  polygon_sweep::Events events(vertices);
  std::vector<Node> nodes(events.num_side_vertices);
  SweepState sweep_state{{vertices, {}}, decomposition_type, nodes.begin()};

  if (decomposition_type == VerticalDecompositionType::exterior_decomposition)
  {
    // If we're decomposing the exterior, then there are regions which extend infinitely upwards and downwards. To make
    // these regions mostly behave like normal regions, we'll add two extra edges which indicate the edges at negative
    // and positive infinity.
    sweep_state.active_edges = {
        {{nullptr, 0}, nullptr, nullptr},
        {{nullptr, 0}, nullptr, nullptr},
    };
  }

  for (const polygon_sweep::Event& event : events.events)
  {
    if (event.incoming_towards_right == event.outgoing_towards_right)
    {
      handle_transition_event(sweep_state, event);
    }
    else if (event.outgoing_towards_right)
    {
      handle_appear_event(sweep_state, event);
    }
    else
    {
      handle_vanish_event(sweep_state, event);
    }
  }

  VerticalDecomposition result;
  result.nodes = std::move(nodes);
  result.leftmost_node = &result.nodes.front();
  result.rightmost_node = &result.nodes.back();
  return result;
}

} // namespace dida::detail::vertical_decomposition