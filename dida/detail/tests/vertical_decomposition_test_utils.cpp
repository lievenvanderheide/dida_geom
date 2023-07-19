#include "dida/detail/tests/vertical_decomposition_test_utils.hpp"

#include <iostream>

#include "dida/utils.hpp"

namespace dida::detail::vertical_decomposition
{

Edge ray_cast_up(VerticesView vertices, const PolygonRange& range, Point2 ray_origin)
{
  YOnEdge result_y = YOnEdge::infinity();
  Edge result = Edge::invalid();

  VertexIt edge_start_it = vertices.begin() + range.first_edge_index;
  for (size_t i = 0; i < range.num_edges; i++)
  {
    VertexIt edge_end_it = next_cyclic(vertices, edge_start_it);

    ScalarDeg1 edge_start_x = i == 0 ? range.start_point_x : edge_start_it->x();
    ScalarDeg1 edge_end_x = i == range.num_edges - 1 ? range.end_point_x : edge_end_it->x();

    bool edge_start_on_left = edge_start_x < ray_origin.x();
    bool edge_end_on_left = edge_end_x < ray_origin.x();
    if (edge_start_on_left != edge_end_on_left)
    {
      YOnEdge cur_y = y_on_edge_for_x(Segment2(*edge_start_it, *edge_end_it), ray_origin.x());
      if (cur_y > ray_origin.y() && cur_y < result_y)
      {
        result = edge_end_on_left ? Edge{edge_start_it, edge_end_it} : Edge::invalid();
        result_y = cur_y;
      }
    }

    edge_start_it = edge_end_it;
  }

  return result;
}

Edge ray_cast_down(VerticesView vertices, const PolygonRange& range, Point2 ray_origin)
{
  YOnEdge result_y = YOnEdge::negative_infinity();
  Edge result = Edge::invalid();

  VertexIt edge_start_it = vertices.begin() + range.first_edge_index;
  for (size_t i = 0; i < range.num_edges; i++)
  {
    VertexIt edge_end_it = next_cyclic(vertices, edge_start_it);

    ScalarDeg1 edge_start_x = i == 0 ? range.start_point_x : edge_start_it->x();
    ScalarDeg1 edge_end_x = i == range.num_edges - 1 ? range.end_point_x : edge_end_it->x();

    bool edge_start_on_left = edge_start_x <= ray_origin.x();
    bool edge_end_on_left = edge_end_x <= ray_origin.x();
    if (edge_start_on_left != edge_end_on_left)
    {
      YOnEdge cur_y = y_on_edge_for_x(Segment2(*edge_start_it, *edge_end_it), ray_origin.x());
      if (cur_y < ray_origin.y() && cur_y > result_y)
      {
        result = edge_start_on_left ? Edge{edge_start_it, edge_end_it} : Edge::invalid();
        result_y = cur_y;
      }
    }

    edge_start_it = edge_end_it;
  }

  return result;
}

namespace
{

void gather_nodes_rec(const Node* node, std::set<const Node*>& result)
{
  if (result.insert(node).second)
  {
    uint8_t num_neighbors = node->is_leaf ? 1 : 3;
    for (uint8_t i = 0; i < num_neighbors; i++)
    {
      if (node->neighbors[i])
      {
        gather_nodes_rec(node->neighbors[i], result);
      }
    }
  }
}

} // namespace

std::set<const Node*> gather_nodes(const Node* node)
{
  std::set<const Node*> result;
  gather_nodes_rec(node, result);
  return result;
}

bool validate_node_opp_edges(VerticesView vertices, const PolygonRange& range, const Node* node)
{
  if (node->is_leaf)
  {
    Edge incoming_edge{prev_cyclic(vertices, node->vertex_it), node->vertex_it};
    Edge outgoing_edge{node->vertex_it, next_cyclic(vertices, node->vertex_it)};
    if (node->direction == HorizontalDirection::right)
    {
      return node->lower_opp_edge == incoming_edge && node->upper_opp_edge == outgoing_edge;
    }
    else
    {
      return node->lower_opp_edge == outgoing_edge && node->upper_opp_edge == incoming_edge;
    }
  }
  else
  {
    return node->lower_opp_edge == ray_cast_down(vertices, range, *node->vertex_it) &&
           node->upper_opp_edge == ray_cast_up(vertices, range, *node->vertex_it);
  }
}

namespace
{

/// Validates whether the edge range between @c start_vertex_it and @c end_vertex_it is monotone in the given direction.
///
/// @tparam direction The direction.
/// @param vertices The vertices of the polygon the edge range belongs to.
/// @param start_vertex_it The first vertex in the edge range.
/// @param end_vertex_it The last vertex in the edge range.
/// @return True iff the edge range is monotone in the given direction.
template <HorizontalDirection direction>
bool validate_boundary_is_monotone(VerticesView vertices, VertexIt start_vertex_it, VertexIt end_vertex_it)
{
  VertexIt it = start_vertex_it;
  while (it != end_vertex_it)
  {
    VertexIt next_it = next_cyclic(vertices, it);
    if (!lex_less_than_with_direction<direction>(*it, *next_it))
    {
      return false;
    }

    it = next_cyclic(vertices, it);
  }

  return true;
}

} // namespace

bool validate_neighboring_nodes_pair(VerticesView vertices, const Node* left_node, uint8_t left_node_branch_index,
                                     const Node* right_node, uint8_t right_node_branch_index)
{
  DIDA_ASSERT(lex_less_than(*left_node->vertex_it, *right_node->vertex_it));
  DIDA_ASSERT(left_node->neighbors[left_node_branch_index] == right_node);
  DIDA_ASSERT(right_node->neighbors[right_node_branch_index] == left_node);

  // Verify that the outgoing direction of the branch of 'left_node' is 'right' and the outgoing direction of the branch
  // of 'right_node' is 'left'.
  //
  // Note that if a branch index is 0, then the outgoing direction is opposite to direction of the node, if a branch
  // index is 1 or 2, then the outgoing direction is equal to the direction of the node.
  if ((left_node_branch_index == 0) != (left_node->direction == HorizontalDirection::left) ||
      (right_node_branch_index == 0) != (right_node->direction == HorizontalDirection::right))
  {
    return false;
  }

  VertexIt lower_boundary_left_vertex_it =
      left_node_branch_index == 2 ? left_node->vertex_it : left_node->lower_opp_edge.start_vertex_it;
  VertexIt upper_boundary_left_vertex_it =
      left_node_branch_index == 1 ? left_node->vertex_it : left_node->upper_opp_edge.end_vertex_it;

  VertexIt lower_boundary_right_vertex_it =
      right_node_branch_index == 2 ? right_node->vertex_it : right_node->lower_opp_edge.end_vertex_it;
  VertexIt upper_boundary_right_vertex_it =
      right_node_branch_index == 1 ? right_node->vertex_it : right_node->upper_opp_edge.start_vertex_it;

  // Verify that at least one of the lower and upper boundaries exists.
  if (!lower_boundary_left_vertex_it && !upper_boundary_left_vertex_it)
  {
    return false;
  }

  // If the left node has an lower boundary then should should the right node, and vice versa.
  if ((lower_boundary_left_vertex_it != nullptr) != (lower_boundary_right_vertex_it != nullptr))
  {
    return false;
  }

  // If there's a lower boundary, then validate that it's monotone.
  if (lower_boundary_left_vertex_it)
  {
    if (!validate_boundary_is_monotone<HorizontalDirection::right>(vertices, lower_boundary_left_vertex_it,
                                                                   lower_boundary_right_vertex_it))
    {
      return false;
    }
  }

  // If the left node has an upper boundary then should should the right node, and vice versa.
  if ((upper_boundary_left_vertex_it != nullptr) != (upper_boundary_right_vertex_it != nullptr))
  {
    return false;
  }

  // If there's an upper boundary, then validate that it's monotone.
  if (upper_boundary_left_vertex_it)
  {
    if (!validate_boundary_is_monotone<HorizontalDirection::left>(vertices, upper_boundary_right_vertex_it,
                                                                  upper_boundary_left_vertex_it))
    {
      return false;
    }
  }

  return true;
}

bool node_should_have_neighbor(const Node* node, uint8_t branch_index, bool is_chain_first_node,
                               bool is_chain_last_node)
{
  if (node->is_leaf)
  {
    DIDA_ASSERT(branch_index == 0);
    return true;
  }

  switch (branch_index)
  {
  case 0:
    return node->lower_opp_edge.is_valid() || node->upper_opp_edge.is_valid();

  case 1:
    if (node->lower_opp_edge.is_valid())
    {
      return true;
    }

    return node->direction == HorizontalDirection::right ? !is_chain_first_node : !is_chain_last_node;

  case 2:
    if (node->upper_opp_edge.is_valid())
    {
      return true;
    }

    return node->direction == HorizontalDirection::left ? !is_chain_first_node : !is_chain_last_node;

  default:
    DIDA_ASSERT(!"Invalid branch index");
    return false;
  }
}

bool validate_node_neighbors(VerticesView vertices, const Node* node, bool is_chain_first_node, bool is_chain_last_node)
{
  uint8_t num_branches = node->is_leaf ? 1 : 3;
  for (uint8_t i = 0; i < num_branches; i++)
  {
    if (node_should_have_neighbor(node, i, is_chain_first_node, is_chain_last_node))
    {
      Node* neighbor = node->neighbors[i];
      if (!neighbor)
      {
        return false;
      }

      uint8_t neighbor_num_branches = neighbor->is_leaf ? 1 : 3;
      uint8_t neighbor_to_node_branch_index = 0;
      for (; neighbor_to_node_branch_index < neighbor_num_branches; neighbor_to_node_branch_index++)
      {
        if (neighbor->neighbors[neighbor_to_node_branch_index] == node)
        {
          break;
        }
      }

      if (neighbor_to_node_branch_index == neighbor_num_branches)
      {
        return false;
      }

      if (lex_less_than(*node->vertex_it, *neighbor->vertex_it) &&
          !validate_neighboring_nodes_pair(vertices, node, i, neighbor, neighbor_to_node_branch_index))
      {
        return false;
      }
    }
    else
    {
      if (node->neighbors[i])
      {
        return false;
      }
    }
  }

  return true;
}

bool validate_chain_decomposition(VerticesView vertices, const ChainDecomposition& chain_decomposition)
{
  PolygonRange range{
      static_cast<size_t>(chain_decomposition.first_node->vertex_it - vertices.begin()),
      distance_cyclic(vertices, chain_decomposition.first_node->vertex_it, chain_decomposition.last_node->vertex_it),
      chain_decomposition.first_node->vertex_it->x(),
      chain_decomposition.last_node->vertex_it->x(),
  };

  std::set<const Node*> nodes = gather_nodes(chain_decomposition.first_node);

  for (const Node* node : nodes)
  {
    if (!validate_node_opp_edges(vertices, range, node))
    {
      return false;
    }
  }

  for (const Node* node : nodes)
  {
    if (!validate_node_neighbors(vertices, node, node == chain_decomposition.first_node,
                                 node == chain_decomposition.last_node))
    {
      return false;
    }
  }

  return true;
}

void print_nodes(VerticesView vertices, ArrayView<const Node> nodes)
{
  std::cout << "std::vector<Node> nodes(" << nodes.size() << ");" << std::endl;
  for (size_t i = 0; i < nodes.size(); i++)
  {
    std::cout << "nodes[" << i << "].direction = "
              << (nodes[i].direction == HorizontalDirection::left ? "HorizontalDirection::left;"
                                                                  : "HorizontalDirection::right;")
              << std::endl;
    std::cout << "nodes[" << i << "].is_leaf = " << (nodes[i].is_leaf ? "true;" : "false;") << std::endl;
    std::cout << "nodes[" << i << "].vertex_it = vertices.begin() + " << (nodes[i].vertex_it - vertices.begin()) << ";"
              << std::endl;

    std::cout << "nodes[" << i << "].lower_opp_edge = ";
    if (nodes[i].lower_opp_edge.is_valid())
    {
      std::cout << "Edge::edge_from_index(vertices, " << nodes[i].lower_opp_edge.start_vertex_it - vertices.begin()
                << ");" << std::endl;
    }
    else
    {
      std::cout << "Edge::invalid();" << std::endl;
    }

    std::cout << "nodes[" << i << "].upper_opp_edge = ";
    if (nodes[i].upper_opp_edge.is_valid())
    {
      std::cout << "Edge::edge_from_index(vertices, " << nodes[i].upper_opp_edge.start_vertex_it - vertices.begin()
                << ");" << std::endl;
    }
    else
    {
      std::cout << "Edge::invalid();" << std::endl;
    }

    size_t num_neighbors = nodes[i].is_leaf ? 1 : 3;
    for (size_t j = 0; j < num_neighbors; j++)
    {
      std::cout << "nodes[" << i << "].neighbors[" << j << "] = ";
      if (nodes[i].neighbors[j])
      {
        std::cout << "&nodes[" << nodes[i].neighbors[j] - nodes.begin() << "];" << std::endl;
      }
      else
      {
        std::cout << "nullptr;" << std::endl;
      }
    }

    std::cout << std::endl;
  }
}

} // namespace dida::detail::vertical_decomposition