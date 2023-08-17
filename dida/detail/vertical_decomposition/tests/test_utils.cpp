#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"

#include <catch2/catch.hpp>
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
    uint8_t num_neighbors = node->type == NodeType::leaf ? 1 : 3;
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
  if (node->type == NodeType::leaf)
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

NodeBranchBoundaryVertices node_branch_boundary_vertices(const ChainDecomposition& chain_decomposition,
                                                         const Node* node, uint8_t branch_index)
{
  NodeBranchBoundaryVertices result;

  if (node->type == NodeType::leaf)
  {
    DIDA_ASSERT(branch_index == 0);
    return NodeBranchBoundaryVertices{node->vertex_it, node->vertex_it};
  }
  else
  {
    switch (branch_index)
    {
    case 0:
      return node->direction == HorizontalDirection::right
                 ? NodeBranchBoundaryVertices{node->lower_opp_edge.end_vertex_it, node->upper_opp_edge.start_vertex_it}
                 : NodeBranchBoundaryVertices{node->lower_opp_edge.start_vertex_it, node->upper_opp_edge.end_vertex_it};

    case 1:
    {
      bool has_upper_boundary = true;
      if (node == chain_decomposition.first_node && node->direction == HorizontalDirection::right)
      {
        has_upper_boundary = false;
      }
      else if (node == chain_decomposition.last_node && node->direction == HorizontalDirection::left)
      {
        has_upper_boundary = false;
      }

      return NodeBranchBoundaryVertices{
          node->direction == HorizontalDirection::right ? node->lower_opp_edge.start_vertex_it
                                                        : node->lower_opp_edge.end_vertex_it,
          has_upper_boundary ? node->vertex_it : nullptr,
      };
    }

    case 2:
    {
      bool has_lower_boundary = true;
      if (node == chain_decomposition.first_node && node->direction == HorizontalDirection::left)
      {
        has_lower_boundary = false;
      }
      else if (node == chain_decomposition.last_node && node->direction == HorizontalDirection::right)
      {
        has_lower_boundary = false;
      }

      return NodeBranchBoundaryVertices{
          has_lower_boundary ? node->vertex_it : nullptr,
          node->direction == HorizontalDirection::right ? node->upper_opp_edge.end_vertex_it
                                                        : node->upper_opp_edge.start_vertex_it,
      };
    }

    default:
      DIDA_ASSERT(!"Invalid branch_index");
      return {};
    }
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

bool validate_neighboring_nodes_pair(VerticesView vertices, const Node* left_node, uint8_t left_node_branch_index,
                                     const NodeBranchBoundaryVertices left_node_boundary_vertices,
                                     const Node* right_node, uint8_t right_node_branch_index,
                                     NodeBranchBoundaryVertices right_node_boundary_vertices)
{
  DIDA_ASSERT(lex_less_than(*left_node->vertex_it, *right_node->vertex_it));
  DIDA_ASSERT(left_node->neighbors[left_node_branch_index] == right_node);
  DIDA_ASSERT(right_node->neighbors[right_node_branch_index] == left_node);

  // Verify that the outgoing direction of the branch of 'left_node' is 'right' and the outgoing direction of the branch
  // of 'right_node' is 'left'.
  //
  // Note that if a branch index is 0, then the outgoing direction is opposite to direction of the node, if a branch
  // index is 1 or 2, then the outgoing direction is equal to the direction of the node.
  if ((left_node_branch_index == 0) != (left_node->direction == HorizontalDirection::left))
  {
    UNSCOPED_INFO("The outgoing direction of branch " << static_cast<int>(left_node_branch_index)
                                                      << " of Node{vertex: " << *left_node->vertex_it
                                                      << "} is to the left, but the node it links to is to its right.");
    return false;
  }

  if ((right_node_branch_index == 0) != (right_node->direction == HorizontalDirection::right))
  {
    UNSCOPED_INFO("The outgoing direction of branch " << static_cast<int>(right_node_branch_index)
                                                      << " of Node{vertex: " << *right_node->vertex_it
                                                      << "} is to the right, but the node it links to is to its left.");
    return false;
  }

  // If the left node has a lower boundary then so should should the right node, and vice versa.
  if ((left_node_boundary_vertices.lower_boundary_vertex_it != nullptr) !=
      (right_node_boundary_vertices.lower_boundary_vertex_it != nullptr))
  {
    bool has_boundary_at_left_node = left_node_boundary_vertices.lower_boundary_vertex_it != nullptr;
    UNSCOPED_INFO("The region between left_node{vertex: "
                  << *left_node->vertex_it << "} and right_node{vertex: " << *right_node->vertex_it
                  << "} has a lower boundary according to its " << (has_boundary_at_left_node ? "left" : "right")
                  << " node but not according to its " << (has_boundary_at_left_node ? "right" : "left") << " node.");
    return false;
  }

  // If there's a lower boundary, then validate that it's monotone.
  if (left_node_boundary_vertices.lower_boundary_vertex_it)
  {
    if (!validate_boundary_is_monotone<HorizontalDirection::right>(
            vertices, left_node_boundary_vertices.lower_boundary_vertex_it,
            right_node_boundary_vertices.lower_boundary_vertex_it))
    {
      UNSCOPED_INFO("The lower boundary between left_node{vertex: "
                    << *left_node->vertex_it << "} and right_node{vertex: " << *right_node->vertex_it
                    << "} is not monotone.");
      return false;
    }
  }

  // If the left node has an upper boundary then so should should the right node, and vice versa.
  if ((left_node_boundary_vertices.upper_boundary_vertex_it != nullptr) !=
      (right_node_boundary_vertices.upper_boundary_vertex_it != nullptr))
  {
    bool has_boundary_at_left_node = left_node_boundary_vertices.upper_boundary_vertex_it != nullptr;
    UNSCOPED_INFO("The region between left_node{vertex: "
                  << *left_node->vertex_it << "} and right_node{vertex: " << *right_node->vertex_it
                  << "} has an upper boundary according to its " << (has_boundary_at_left_node ? "left" : "right")
                  << " node but not according to its " << (has_boundary_at_left_node ? "right" : "left") << " node.");
    return false;
  }

  // If there's an upper boundary, then validate that it's monotone.
  if (left_node_boundary_vertices.upper_boundary_vertex_it)
  {
    if (!validate_boundary_is_monotone<HorizontalDirection::left>(vertices,
                                                                  right_node_boundary_vertices.upper_boundary_vertex_it,
                                                                  left_node_boundary_vertices.upper_boundary_vertex_it))
    {
      UNSCOPED_INFO("The upper boundary between left_node{vertex: "
                    << *left_node->vertex_it << "} and right_node{vertex: " << *right_node->vertex_it
                    << "} is not monotone.");
      return false;
    }
  }

  return true;
}

} // namespace

bool validate_node_neighbors(VerticesView vertices, const ChainDecomposition& chain_decomposition, const Node* node)
{
  uint8_t num_branches = node->type == NodeType::leaf ? 1 : 3;
  for (uint8_t i = 0; i < num_branches; i++)
  {
    NodeBranchBoundaryVertices boundary_vertices = node_branch_boundary_vertices(chain_decomposition, node, i);
    if (boundary_vertices.lower_boundary_vertex_it || boundary_vertices.upper_boundary_vertex_it)
    {
      // The current branch has at least one of the two boundaries, so there should be a neighbor.

      const Node* neighbor = node->neighbors[i];
      if (!neighbor)
      {
        UNSCOPED_INFO("Node{vertex: " << *node->vertex_it << "}.neighbors[" << static_cast<int>(i)
                                      << "] should be set but isn't.");
        return false;
      }

      // Find the index in 'neighbor' which links it back to 'node'.
      uint8_t neighbor_num_branches = neighbor->type == NodeType::leaf ? 1 : 3;
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
        UNSCOPED_INFO("a_node{vertex: " << *node->vertex_it << "}.neighbors[" << static_cast<int>(i)
                                        << "] links to b_node{vertex: " << *neighbor->vertex_it
                                        << "}, but b_node doesn't link back to a_node.");
        return false;
      }

      // To avoid calling 'validate_neighboring_nodes_pair' twice. we only call it when 'node' is the left node and
      // 'neighbor' the right node. If it's the other way around, then 'validate_neighboring_nodes_pair' will be called
      // when the current function is called with 'neighbor'.
      if (lex_less_than(*node->vertex_it, *neighbor->vertex_it))
      {
        NodeBranchBoundaryVertices neighbor_boundary_vertices =
            node_branch_boundary_vertices(chain_decomposition, neighbor, neighbor_to_node_branch_index);
        if (!validate_neighboring_nodes_pair(vertices, node, i, boundary_vertices, neighbor,
                                             neighbor_to_node_branch_index, neighbor_boundary_vertices))
        {
          return false;
        }
      }
    }
    else
    {
      if (node->neighbors[i])
      {
        UNSCOPED_INFO("Node{vertex: " << *node->vertex_it << "}.neighbors[" << static_cast<int>(i)
                                      << "] should be nullptr, but isn't.");
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
    if (!validate_node_neighbors(vertices, chain_decomposition, node))
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
    std::cout << "nodes[" << i << "].type = ";
    switch (nodes[i].type)
    {
    case NodeType::branch:
      std::cout << "NodeType::branch;" << std::endl;
      break;
    case NodeType::leaf:
      std::cout << "NodeType::leaf;" << std::endl;
      break;
    case NodeType::outer_branch:
      std::cout << "NodeType::outer_branch;" << std::endl;
      break;
    }

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

    size_t num_neighbors = nodes[i].type == NodeType::leaf ? 1 : 3;
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

std::vector<ChainDecomposition> initial_chain_decompositions(VerticesView vertices, NodePool& node_pool)
{
  // Find the first convex reflex vertex.
  VertexIt it = vertices.begin();
  for (; it != vertices.end(); ++it)
  {
    VertexIt prev_it = prev_cyclic(vertices, it);
    VertexIt next_it = next_cyclic(vertices, it);
    if (lex_less_than(*prev_it, *it) != lex_less_than(*it, *next_it) && cross(*it - *prev_it, *next_it - *it) > 0)
    {
      break;
    }
  }

  DIDA_ASSERT(it != vertices.end());

  std::vector<ChainDecomposition> result;
  Node* prev_node = nullptr;

  for (size_t i = 0; i <= vertices.size(); i++)
  {
    VertexIt prev_it = prev_cyclic(vertices, it);
    VertexIt next_it = next_cyclic(vertices, it);
    bool incoming_towards_right = lex_less_than(*prev_it, *it);
    bool outgoing_towards_right = lex_less_than(*it, *next_it);
    if (incoming_towards_right != outgoing_towards_right)
    {
      bool is_convex_corner = cross(*it - *prev_it, *next_it - *it) > 0;

      if (i != 0)
      {
        // Add a node to the current chain.

        Node* node = node_pool.alloc();
        node->direction = incoming_towards_right ? HorizontalDirection::left : HorizontalDirection::right;
        node->type = NodeType::branch;
        node->vertex_it = it;
        node->lower_opp_edge = Edge::invalid();
        node->upper_opp_edge = Edge::invalid();
        node->neighbors[0] = nullptr;
        node->neighbors[1] = nullptr;
        node->neighbors[2] = nullptr;

        if (incoming_towards_right)
        {
          node->neighbors[2] = prev_node;
          prev_node->neighbors[2] = node;
        }
        else
        {
          node->neighbors[1] = prev_node;
          prev_node->neighbors[1] = node;
        }

        prev_node = node;

        if (is_convex_corner)
        {
          // If it's a convex corner, then the node we just added is the last node of the chain.
          result.back().last_node = node;
        }
      }

      if (is_convex_corner && i != vertices.size())
      {
        // Start a new chain.

        Node* node = node_pool.alloc();
        node->direction = outgoing_towards_right ? HorizontalDirection::right : HorizontalDirection::left;
        node->type = NodeType::branch;
        node->vertex_it = it;
        node->lower_opp_edge = Edge::invalid();
        node->upper_opp_edge = Edge::invalid();
        node->neighbors[0] = nullptr;
        node->neighbors[1] = nullptr;
        node->neighbors[2] = nullptr;

        result.push_back(ChainDecomposition{node, nullptr});

        prev_node = node;
      }
    }

    it = next_it;
  }

  return result;
}

} // namespace dida::detail::vertical_decomposition