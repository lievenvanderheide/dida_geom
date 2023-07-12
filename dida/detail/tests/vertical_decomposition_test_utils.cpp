#include "dida/detail/tests/vertical_decomposition_test_utils.hpp"

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

void gather_nodes_rec(const Node* node, const Node* parent_node, std::vector<const Node*>& nodes)
{
  nodes.push_back(node);

  if (node->is_leaf)
  {
    return;
  }

  for (const Node* neighbor : node->neighbors)
  {
    if (neighbor && neighbor != parent_node)
    {
      gather_nodes_rec(neighbor, node, nodes);
    }
  }
}

std::vector<const Node*> gather_nodes(const Node* root)
{
  std::vector<const Node*> result;
  gather_nodes_rec(root, nullptr, result);
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

} // namespace

bool validate_chain_decomposition(VerticesView vertices, const ChainDecomposition& chain_decomposition)
{
  PolygonRange range{
      static_cast<size_t>(chain_decomposition.first_node->vertex_it - vertices.begin()),
      distance_cyclic(vertices, chain_decomposition.first_node->vertex_it, chain_decomposition.last_node->vertex_it),
      chain_decomposition.first_node->vertex_it->x(),
      chain_decomposition.last_node->vertex_it->x(),
  };

  std::vector<const Node*> nodes = gather_nodes(chain_decomposition.first_node);

  for (const Node* node : nodes)
  {
    if (!validate_node_opp_edges(vertices, range, node))
    {
      return false;
    }
  }

  return true;
}

} // namespace dida::detail::vertical_decomposition