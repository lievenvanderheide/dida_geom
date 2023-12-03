#include "dida/detail/vertical_decomposition/tests/vertical_extension_validation.hpp"

#include <catch2/catch_test_macros.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"

namespace dida::detail::vertical_decomposition
{

bool PolygonLocationLessThan::operator()(const PolygonLocation& a, const PolygonLocation& b) const
{
  if (a.edge_index != b.edge_index)
  {
    return a.edge_index < b.edge_index;
  }

  if (lex_less_than(vertices[a.edge_index], vertices[succ_modulo(a.edge_index, vertices.size())]))
  {
    return a.x < b.x;
  }
  else
  {
    return a.x > b.x;
  }
}

std::pair<PolygonRange, PolygonRange> PolygonRange::split(VerticesView vertices, PolygonLocation location) const
{
  // There's no support yet for the case when num_edges == vertices.size() + 1 (which can happen when we have a range
  // whose start and end point are strictly on the interior of the same edge).
  DIDA_ASSERT(num_edges <= vertices.size());

  size_t a_num_edges = sub_modulo(location.edge_index, first_edge_index, vertices.size());
  size_t b_num_edges = num_edges - a_num_edges;

  if (location.x != vertices[location.edge_index].x())
  {
    a_num_edges++;
  }

  return std::make_pair(
      PolygonRange{
          first_edge_index,
          a_num_edges,
          start_point_x,
          location.x,
      },
      PolygonRange{
          location.edge_index,
          b_num_edges,
          location.x,
          end_point_x,
      });
}

Edge ray_cast_up(VerticesView vertices, Winding winding, const PolygonRange& range, Point2 ray_origin)
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
        bool on_interior_side = edge_end_on_left == (winding == Winding::ccw);
        result = on_interior_side ? Edge{edge_start_it, edge_end_it} : Edge::invalid();
        result_y = cur_y;
      }
    }

    edge_start_it = edge_end_it;
  }

  return result;
}

Edge ray_cast_down(VerticesView vertices, Winding winding, const PolygonRange& range, Point2 ray_origin)
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
        bool on_interior_side = edge_start_on_left == (winding == Winding::ccw);
        result = on_interior_side ? Edge{edge_start_it, edge_end_it} : Edge::invalid();
        result_y = cur_y;
      }
    }

    edge_start_it = edge_end_it;
  }

  return result;
}

bool VerticalExtensionContactPoint::operator==(const VerticalExtensionContactPoint& b) const
{
  return type == b.type && node == b.node;
}

bool VerticalExtensionContactPoint::operator<(const VerticalExtensionContactPoint& b) const
{
  return type < b.type || (type == b.type && node < b.node);
}

std::string_view contact_point_type_to_string(VerticalExtensionContactPoint::Type type)
{
  using namespace std::literals;

  switch (type)
  {
  case VerticalExtensionContactPoint::Type::vertex_downwards:
    return "vertex_downwards"sv;
  case VerticalExtensionContactPoint::Type::vertex_upwards:
    return "vertex_upwards"sv;
  case VerticalExtensionContactPoint::Type::lower_opp_edge:
    return "lower_opp_edge"sv;
  case VerticalExtensionContactPoint::Type::upper_opp_edge:
    return "upper_opp_edge"sv;
  case VerticalExtensionContactPoint::Type::leaf:
    return "leaf"sv;
  default:
    DIDA_ASSERT(!"Invalid type");
    return "<invalid>"sv;
  }
};

std::vector<VerticalExtensionContactPoint>
vertical_extension_contact_points(const ChainDecomposition& chain_decomposition, Winding winding)
{
  /// The horizontal direction of a boundary which has the interior above it.
  HorizontalDirection lower_boundary_direction =
      winding == Winding::ccw ? HorizontalDirection::right : HorizontalDirection::left;

  std::vector<VerticalExtensionContactPoint> contact_points;

  const Node* prev;
  const Node* node;

  const Node* first_node = chain_decomposition.first_node;
  if (first_node->direction == lower_boundary_direction)
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        VerticalExtensionContactPoint::Type::vertex_upwards,
        first_node,
    });

    prev = first_node;
    node = first_node->neighbors[2];
  }
  else
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        VerticalExtensionContactPoint::Type::vertex_downwards,
        first_node,
    });

    prev = first_node;
    node = first_node->neighbors[1];
  }

  while (true)
  {
    if (node->type == NodeType::leaf)
    {
      VerticalExtensionContactPoint contact_point;
      contact_point.type = VerticalExtensionContactPoint::Type::leaf;
      contact_point.node = node;
      contact_points.push_back(contact_point);

      std::swap(node, prev);
    }
    else
    {
      if (node->direction == lower_boundary_direction)
      {
        if (node->neighbors[0] == prev)
        {
          // Note: in a valid vertical decomposition, this branch is always taken, because if lower_opp_edge is not
          // valid, then there's no boundary to traverse either, however, since the purpose of this function is
          // validation, we should handle invalid decompositions as gracefully as possible.
          if (node->lower_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[1];
        }
        else if (node->neighbors[1] == prev)
        {
          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_downwards,
              node,
          });

          if (node == chain_decomposition.last_node)
          {
            break;
          }

          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_upwards,
              node,
          });

          prev = node;
          node = node->neighbors[2];
        }
        else
        {
          DIDA_ASSERT(node->neighbors[2] == prev);

          if (node->upper_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[0];
        }
      }
      else
      {
        if (node->neighbors[0] == prev)
        {
          if (node->upper_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[2];
        }
        else if (node->neighbors[1] == prev)
        {
          if (node->lower_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[0];
        }
        else
        {
          DIDA_ASSERT(node->neighbors[2] == prev);

          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_upwards,
              node,
          });

          if (node == chain_decomposition.last_node)
          {
            break;
          }

          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_downwards,
              node,
          });

          prev = node;
          node = node->neighbors[1];
        }
      }
    }
  }

  return contact_points;
}

namespace
{

void split_chain_decomposition_into_islands_rec(VerticesView vertices, Winding winding,
                                                const ChainDecomposition& chain_decomposition,
                                                ArrayView<const VerticalExtensionContactPoint> contact_points,
                                                PolygonRange range, std::vector<ChainDecompositionIsland>& result)
{
  using ContactPointsIt = ArrayView<const VerticalExtensionContactPoint>::iterator;

  /// The horizontal direction of a boundary which has the interior above it.
  HorizontalDirection lower_boundary_direction =
      winding == Winding::ccw ? HorizontalDirection::right : HorizontalDirection::left;

  size_t island_begin = 0;

  for (size_t i = 0; i < contact_points.size(); i++)
  {
    VerticalExtensionContactPoint contact_point =
        winding == Winding::ccw ? contact_points[i] : contact_points[contact_points.size() - i - 1];
    const Node* node = contact_point.node;

    bool should_split = false;
    PolygonLocation split_location;

    switch (contact_point.type)
    {
    case VerticalExtensionContactPoint::Type::vertex_downwards:
      if (!node->lower_opp_edge.is_valid())
      {
        should_split = ray_cast_down(vertices, winding, range, *node->vertex_it) == Edge::invalid();
        split_location = PolygonLocation{
            static_cast<size_t>(node->vertex_it - vertices.begin()),
            node->vertex_it->x(),
        };
      }
      break;
    case VerticalExtensionContactPoint::Type::vertex_upwards:
      if (!node->upper_opp_edge.is_valid())
      {
        should_split = ray_cast_up(vertices, winding, range, *node->vertex_it) == Edge::invalid();
        split_location = PolygonLocation{
            static_cast<size_t>(node->vertex_it - vertices.begin()),
            node->vertex_it->x(),
        };
      }
      break;

    case VerticalExtensionContactPoint::Type::lower_opp_edge:
      if ((node == chain_decomposition.first_node && node->direction == lower_boundary_direction) ||
          (node == chain_decomposition.last_node && node->direction != lower_boundary_direction) ||
          node->type == NodeType::outer_branch)
      {
        should_split = ray_cast_down(vertices, winding, range, *node->vertex_it) == node->lower_opp_edge;
        split_location = PolygonLocation{
            static_cast<size_t>(node->lower_opp_edge.start_vertex_it - vertices.begin()),
            node->vertex_it->x(),
        };
      }
      break;

    case VerticalExtensionContactPoint::Type::upper_opp_edge:
      if ((node == chain_decomposition.first_node && node->direction != lower_boundary_direction) ||
          (node == chain_decomposition.last_node && node->direction == lower_boundary_direction) ||
          node->type == NodeType::outer_branch)
      {
        should_split = ray_cast_up(vertices, winding, range, *node->vertex_it) == node->upper_opp_edge;
        split_location = PolygonLocation{
            static_cast<size_t>(node->upper_opp_edge.start_vertex_it - vertices.begin()),
            node->vertex_it->x(),
        };
      }
      break;

    case VerticalExtensionContactPoint::Type::leaf:
      break;
    }

    if (should_split)
    {
      std::pair<PolygonRange, PolygonRange> sub_ranges = range.split(vertices, split_location);

      if (winding == Winding::ccw)
      {
        if (i != island_begin)
        {
          ArrayView<const VerticalExtensionContactPoint> rec_contact_points(contact_points.begin() + island_begin,
                                                                            i - island_begin);
          split_chain_decomposition_into_islands_rec(vertices, winding, chain_decomposition, rec_contact_points,
                                                     sub_ranges.first, result);
        }

        range = sub_ranges.second;
        island_begin = i + 1;
      }
      else
      {
        if (i != island_begin)
        {
          ArrayView<const VerticalExtensionContactPoint> rec_contact_points(
              contact_points.begin() + contact_points.size() - i, i - island_begin);
          split_chain_decomposition_into_islands_rec(vertices, winding, chain_decomposition, rec_contact_points,
                                                     sub_ranges.second, result);
        }

        range = sub_ranges.first;
        island_begin = i + 1;
      }
    }
  }

  if (island_begin != contact_points.size())
  {
    if (winding == Winding::ccw)
    {
      result.push_back({
          ArrayView<const VerticalExtensionContactPoint>(contact_points.begin() + island_begin,
                                                         contact_points.size() - island_begin),
          range,
      });
    }
    else
    {
      result.push_back({
          ArrayView<const VerticalExtensionContactPoint>(contact_points.begin(), contact_points.size() - island_begin),
          range,
      });
    }
  }
}

} // namespace

std::vector<ChainDecompositionIsland>
split_chain_decomposition_into_islands(VerticesView vertices, Winding winding,
                                       const ChainDecomposition& chain_decomposition,
                                       ArrayView<const VerticalExtensionContactPoint> contact_points)
{
  PolygonRange full_range{
      static_cast<size_t>(chain_decomposition.first_node->vertex_it - vertices.begin()),
      distance_cyclic(vertices, chain_decomposition.first_node->vertex_it, chain_decomposition.last_node->vertex_it),
      chain_decomposition.first_node->vertex_it->x(),
      chain_decomposition.last_node->vertex_it->x(),
  };
  std::vector<ChainDecompositionIsland> result;
  split_chain_decomposition_into_islands_rec(vertices, winding, chain_decomposition, contact_points, full_range,
                                             result);

  if (winding == Winding::cw)
  {
    std::reverse(result.begin(), result.end());
  }

  return result;
}

namespace
{

bool validate_vertical_extension_island(VerticesView vertices, const ChainDecompositionIsland& island)
{
  bool valid = true;

  std::set<VerticalExtensionContactPoint> expected_opp_contact_points;
  std::set<VerticalExtensionContactPoint> actual_opp_contact_points;

  for (const VerticalExtensionContactPoint& contact_point : island.contact_points)
  {
    switch (contact_point.type)
    {
    case VerticalExtensionContactPoint::Type::vertex_downwards:
    {
      Edge expected_lower_opp_edge =
          ray_cast_down(vertices, Winding::ccw, island.range, *contact_point.node->vertex_it);
      if (expected_lower_opp_edge != contact_point.node->lower_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.lower_opp_edge should be "
                                      << expected_lower_opp_edge << ", but is " << contact_point.node->lower_opp_edge
                                      << ".");
        return false;
      }

      expected_opp_contact_points.insert(VerticalExtensionContactPoint{
          VerticalExtensionContactPoint::Type::lower_opp_edge,
          contact_point.node,
      });
    }
    break;

    case VerticalExtensionContactPoint::Type::vertex_upwards:
    {
      Edge expected_upper_opp_edge = ray_cast_up(vertices, Winding::ccw, island.range, *contact_point.node->vertex_it);
      if (expected_upper_opp_edge != contact_point.node->upper_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.upper_opp_edge should be "
                                      << expected_upper_opp_edge << ", but is " << contact_point.node->upper_opp_edge
                                      << ".");
        return false;
      }

      expected_opp_contact_points.insert(VerticalExtensionContactPoint{
          VerticalExtensionContactPoint::Type::upper_opp_edge,
          contact_point.node,
      });
    }
    break;

    case VerticalExtensionContactPoint::Type::lower_opp_edge:
      if (contact_point.node->type == NodeType::outer_branch)

      {
        Edge lower_opp_edge = ray_cast_down(vertices, Winding::ccw, island.range, *contact_point.node->vertex_it);
        Edge upper_opp_edge = ray_cast_up(vertices, Winding::ccw, island.range, *contact_point.node->vertex_it);

        expected_opp_contact_points.insert(contact_point);
        expected_opp_contact_points.insert(VerticalExtensionContactPoint{
            VerticalExtensionContactPoint::Type::upper_opp_edge,
            contact_point.node,
        });
      }

      actual_opp_contact_points.insert(contact_point);
      break;

    case VerticalExtensionContactPoint::Type::upper_opp_edge:
      actual_opp_contact_points.insert(contact_point);
      break;

    case VerticalExtensionContactPoint::Type::leaf:
    {
      const Node* node = contact_point.node;

      Edge incoming_edge{prev_cyclic(vertices, node->vertex_it), node->vertex_it};
      Edge outgoing_edge{node->vertex_it, next_cyclic(vertices, node->vertex_it)};

      Edge expected_lower_opp_edge, expected_upper_opp_edge;
      if (node->direction == HorizontalDirection::right)
      {
        expected_lower_opp_edge = incoming_edge;
        expected_upper_opp_edge = outgoing_edge;
      }
      else
      {
        expected_lower_opp_edge = outgoing_edge;
        expected_upper_opp_edge = incoming_edge;
      }

      if (expected_lower_opp_edge != contact_point.node->lower_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.lower_opp_edge should be "
                                      << expected_lower_opp_edge << ", but is " << contact_point.node->lower_opp_edge
                                      << ".");
        return false;
      }

      if (expected_upper_opp_edge != contact_point.node->upper_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.upper_opp_edge should be "
                                      << expected_upper_opp_edge << ", but is " << contact_point.node->upper_opp_edge
                                      << ".");
        return false;
      }
    }
    break;
    }
  }

  std::set<VerticalExtensionContactPoint>::iterator expected_it = expected_opp_contact_points.begin();
  std::set<VerticalExtensionContactPoint>::iterator actual_it = actual_opp_contact_points.begin();

  while (expected_it != expected_opp_contact_points.end() || actual_it != actual_opp_contact_points.end())
  {
    if (actual_it == actual_opp_contact_points.end() ||
        (expected_it != expected_opp_contact_points.end() && *expected_it < *actual_it))
    {
      UNSCOPED_INFO("Node{vertex: " << *expected_it->node->vertex_it << ", type: "
                                    << node_type_to_string(expected_it->node->type) << "}: contact point of type "
                                    << contact_point_type_to_string(expected_it->type) << " expected, but not found.");

      valid = false;
      expected_it++;
    }
    else if (expected_it == expected_opp_contact_points.end() || *actual_it < *expected_it)
    {
      UNSCOPED_INFO("Node{vertex: " << *actual_it->node->vertex_it << ", type: "
                                    << node_type_to_string(actual_it->node->type) << "}: contact point of type "
                                    << contact_point_type_to_string(actual_it->type) << " found, but not expected.");

      valid = false;
      actual_it++;
    }
    else
    {
      actual_it++;
      expected_it++;
    }
  }

  return valid;
}

} // namespace

bool validate_vertical_extensions(VerticesView vertices, ArrayView<const ChainDecompositionIsland> islands)
{
  for (const ChainDecompositionIsland& island : islands)
  {
    if (!validate_vertical_extension_island(vertices, island))
    {
      return false;
    }
  }

  return true;
}

bool validate_vertical_extensions(VerticesView vertices, const std::set<const Node*>& nodes)
{
  PolygonRange full_range{0, vertices.size(), vertices[0].x(), vertices[0].x()};

  for (const Node* node : nodes)
  {
    Edge expected_lower_opp_edge, expected_upper_opp_edge;
    if (node->type == NodeType::leaf)
    {
      Edge incoming_edge{prev_cyclic(vertices, node->vertex_it), node->vertex_it};
      Edge outgoing_edge{node->vertex_it, next_cyclic(vertices, node->vertex_it)};

      if (node->direction == HorizontalDirection::right)
      {
        expected_lower_opp_edge = incoming_edge;
        expected_upper_opp_edge = outgoing_edge;
      }
      else
      {
        expected_lower_opp_edge = outgoing_edge;
        expected_upper_opp_edge = incoming_edge;
      }
    }
    else
    {
      expected_lower_opp_edge = ray_cast_down(vertices, Winding::ccw, full_range, *node->vertex_it);
      expected_upper_opp_edge = ray_cast_up(vertices, Winding::ccw, full_range, *node->vertex_it);
    }

    if (node->lower_opp_edge != expected_lower_opp_edge)
    {
      UNSCOPED_INFO("Node{vertex: " << *node->vertex_it << "}.lower_opp_edge should be " << expected_lower_opp_edge
                                    << ", but is " << node->lower_opp_edge << ".");
      return false;
    }

    if (node->upper_opp_edge != expected_upper_opp_edge)
    {
      UNSCOPED_INFO("Node{vertex: " << *node->vertex_it << "}.upper_opp_edge should be " << expected_upper_opp_edge
                                    << ", but is " << node->upper_opp_edge << ".");
      return false;
    }
  }

  return true;
}

} // namespace dida::detail::vertical_decomposition