#include "dida/detail/vertical_decomposition/tests/vertical_extension_validation.hpp"

#include <catch2/catch_test_macros.hpp>

#include "dida/detail/vertical_decomposition/tests/test_utils.hpp"

namespace dida::detail::vertical_decomposition
{

Edge ray_cast_up(VerticesView vertices, Winding winding, std::optional<PolygonRange> range, Point2 ray_origin)
{
  YOnEdge result_y = YOnEdge::infinity();
  Edge result = Edge::invalid();

  VertexIt edge_start_it;
  size_t num_edges;
  if (range)
  {
    edge_start_it = vertices.begin() + range->begin.edge_index;
    num_edges = sub_modulo(range->end.edge_index, range->begin.edge_index, vertices.size());
    if (range->end.x != vertices[range->end.edge_index].x())
    {
      num_edges++;
    }
  }
  else
  {
    edge_start_it = vertices.begin();
    num_edges = vertices.size();
  }

  for (size_t i = 0; i < num_edges; i++)
  {
    VertexIt edge_end_it = next_cyclic(vertices, edge_start_it);

    bool edge_start_on_left = edge_start_it->x() < ray_origin.x();
    bool edge_end_on_left = edge_end_it->x() < ray_origin.x();
    if (edge_start_on_left != edge_end_on_left || (range && i == 0 && ray_origin.x() == range->begin.x) ||
        (range && i == num_edges - 1 && ray_origin.x() == range->end.x))
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

Edge ray_cast_down(VerticesView vertices, Winding winding, std::optional<PolygonRange> range, Point2 ray_origin)
{
  YOnEdge result_y = YOnEdge::negative_infinity();
  Edge result = Edge::invalid();

  VertexIt edge_start_it;
  size_t num_edges;
  if (range)
  {
    edge_start_it = vertices.begin() + range->begin.edge_index;
    num_edges = sub_modulo(range->end.edge_index, range->begin.edge_index, vertices.size());
    if (range->end.x != vertices[range->end.edge_index].x())
    {
      num_edges++;
    }
  }
  else
  {
    edge_start_it = vertices.begin();
    num_edges = vertices.size();
  }

  for (size_t i = 0; i < num_edges; i++)
  {
    VertexIt edge_end_it = next_cyclic(vertices, edge_start_it);

    bool edge_start_on_left = edge_start_it->x() <= ray_origin.x();
    bool edge_end_on_left = edge_end_it->x() <= ray_origin.x();
    if (edge_start_on_left != edge_end_on_left || (range && i == 0 && ray_origin.x() == range->begin.x) ||
        (range && i == num_edges - 1 && ray_origin.x() == range->end.x))
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
  case VerticalExtensionContactPoint::Type::outer_branch_lower_opp_edge:
    return "outer_branch_lower_opp_edge"sv;
  case VerticalExtensionContactPoint::Type::leaf:
    return "leaf"sv;
  case VerticalExtensionContactPoint::Type::vertex_downwards_to_infinity:
    return "vertex_downwards_to_infinity"sv;
  case VerticalExtensionContactPoint::Type::vertex_upwards_to_infinity:
    return "vertex_upwards_to_infinity"sv;
  case VerticalExtensionContactPoint::Type::lower_opp_edge_to_infinity:
    return "lower_opp_edge_to_infinity"sv;
  case VerticalExtensionContactPoint::Type::lower_opp_edge_to_vertex_exterior_side:
    return "lower_opp_edge_to_vertex_exterior_side"sv;
  case VerticalExtensionContactPoint::Type::upper_opp_edge_to_infinity:
    return "upper_opp_edge_to_infinity"sv;
  case VerticalExtensionContactPoint::Type::upper_opp_edge_to_vertex_exterior_side:
    return "upper_opp_edge_to_vertex_exterior_side"sv;

  default:
    DIDA_ASSERT(!"Invalid type");
    return "<invalid>"sv;
  }
}

std::vector<VerticalExtensionContactPoint>
vertical_extension_contact_points(const ChainDecomposition& chain_decomposition, Winding winding)
{
  /// The horizontal direction of a boundary which has the interior above it.
  HorizontalDirection lower_boundary_direction =
      winding == winding ? HorizontalDirection::right : HorizontalDirection::left;

  std::vector<VerticalExtensionContactPoint> contact_points;

  const Node* prev;
  const Node* node;

  const Node* first_node = chain_decomposition.first_node;
  if (first_node->direction == lower_boundary_direction)
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        first_node->upper_opp_edge.is_valid() ? VerticalExtensionContactPoint::Type::vertex_upwards
                                              : VerticalExtensionContactPoint::Type::vertex_upwards_to_infinity,
        first_node,
    });

    prev = first_node;
    node = first_node->neighbors[2];
  }
  else
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        first_node->lower_opp_edge.is_valid() ? VerticalExtensionContactPoint::Type::vertex_downwards
                                              : VerticalExtensionContactPoint::Type::vertex_downwards_to_infinity,
        first_node,
    });

    prev = first_node;
    node = first_node->neighbors[1];
  }

  while (true)
  {
    if (node->type == NodeType::leaf)
    {
      contact_points.push_back(VerticalExtensionContactPoint{
          VerticalExtensionContactPoint::Type::leaf,
          node,
      });

      std::swap(node, prev);
    }
    else
    {
      if (node->direction == lower_boundary_direction)
      {
        if (node->neighbors[0] == prev)
        {
          if (node == chain_decomposition.first_node)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge_to_vertex_exterior_side,
                node,
            });
          }
          else if (node->type == NodeType::branch && !node->upper_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge_to_infinity,
                node,
            });
          }
          else if (node->type == NodeType::outer_branch)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::outer_branch_lower_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[1];
        }
        else if (node->neighbors[1] == prev)
        {
          if (node == chain_decomposition.last_node)
          {
            break;
          }

          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_downwards,
              node,
          });

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

          if (node == chain_decomposition.last_node)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge_to_vertex_exterior_side,
                node,
            });
          }
          else if (node->type == NodeType::branch && !node->lower_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge_to_infinity,
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
          if (node == chain_decomposition.first_node)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge_to_vertex_exterior_side,
                node,
            });
          }
          else if (node->type == NodeType::branch && !node->lower_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::upper_opp_edge_to_infinity,
                node,
            });
          }

          prev = node;
          node = node->neighbors[2];
        }
        else if (node->neighbors[1] == prev)
        {
          if (node == chain_decomposition.last_node)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge_to_vertex_exterior_side,
                node,
            });
          }
          else if (node->type == NodeType::branch && !node->upper_opp_edge.is_valid())
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::lower_opp_edge_to_infinity,
                node,
            });
          }
          else if (node->type == NodeType::outer_branch)
          {
            contact_points.push_back(VerticalExtensionContactPoint{
                VerticalExtensionContactPoint::Type::outer_branch_lower_opp_edge,
                node,
            });
          }

          prev = node;
          node = node->neighbors[0];
        }
        else
        {
          DIDA_ASSERT(node->neighbors[2] == prev);

          if (node == chain_decomposition.last_node)
          {
            break;
          }

          contact_points.push_back(VerticalExtensionContactPoint{
              VerticalExtensionContactPoint::Type::vertex_upwards,
              node,
          });

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

  const Node* last_node = chain_decomposition.last_node;
  if (last_node->direction != lower_boundary_direction)
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        last_node->upper_opp_edge.is_valid() ? VerticalExtensionContactPoint::Type::vertex_upwards
                                             : VerticalExtensionContactPoint::Type::vertex_upwards_to_infinity,
        last_node,
    });
  }
  else
  {
    contact_points.push_back(VerticalExtensionContactPoint{
        last_node->upper_opp_edge.is_valid() ? VerticalExtensionContactPoint::Type::vertex_downwards
                                             : VerticalExtensionContactPoint::Type::vertex_downwards_to_infinity,
        last_node,
    });
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

  ContactPointsIt island_begin = contact_points.begin();

  for (ContactPointsIt it = contact_points.begin(); it != contact_points.end(); ++it)
  {
    const Node* node = it->node;

    bool should_split = false;
    PolygonLocation split_location;

    switch (it->type)
    {
    case VerticalExtensionContactPoint::Type::vertex_downwards_to_infinity:
      should_split = ray_cast_down(vertices, winding, range, *node->vertex_it) == Edge::invalid();
      split_location = PolygonLocation{
          static_cast<size_t>(node->vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;
    case VerticalExtensionContactPoint::Type::vertex_upwards_to_infinity:
      should_split = ray_cast_up(vertices, winding, range, *node->vertex_it) == Edge::invalid();
      split_location = PolygonLocation{
          static_cast<size_t>(node->vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;
    case VerticalExtensionContactPoint::Type::lower_opp_edge_to_infinity:
      should_split = ray_cast_down(vertices, winding, range, *node->vertex_it) == node->lower_opp_edge &&
                     ray_cast_up(vertices, winding, range, *node->vertex_it) == Edge::invalid();
      split_location = PolygonLocation{
          static_cast<size_t>(node->lower_opp_edge.start_vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;
    case VerticalExtensionContactPoint::Type::lower_opp_edge_to_vertex_exterior_side:
      should_split = ray_cast_down(vertices, winding, range, *node->vertex_it) == node->lower_opp_edge;
      split_location = PolygonLocation{
          static_cast<size_t>(node->lower_opp_edge.start_vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;
    case VerticalExtensionContactPoint::Type::upper_opp_edge_to_infinity:
      should_split = ray_cast_up(vertices, winding, range, *node->vertex_it) == node->upper_opp_edge &&
                     ray_cast_down(vertices, winding, range, *node->vertex_it) == Edge::invalid();
      split_location = PolygonLocation{
          static_cast<size_t>(node->upper_opp_edge.start_vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;
    case VerticalExtensionContactPoint::Type::upper_opp_edge_to_vertex_exterior_side:
      should_split = ray_cast_up(vertices, winding, range, *node->vertex_it) == node->upper_opp_edge;
      split_location = PolygonLocation{
          static_cast<size_t>(node->upper_opp_edge.start_vertex_it - vertices.begin()),
          node->vertex_it->x(),
      };
      break;

    default:
      // All other contact point types correspond to finite vertical extensions, so they don't split the decomposition
      // into islands.
      break;
    }

    if (should_split)
    {
      if (it != island_begin)
      {
        ArrayView<const VerticalExtensionContactPoint> rec_contact_points(island_begin,
                                                                          static_cast<size_t>(it - island_begin));
        split_chain_decomposition_into_islands_rec(vertices, winding, chain_decomposition, rec_contact_points,
                                                   PolygonRange{range.begin, split_location}, result);
      }

      range = PolygonRange{split_location, range.end};
      island_begin = it + 1;
    }
  }

  if (island_begin != contact_points.end())
  {
    result.push_back({
        ArrayView<const VerticalExtensionContactPoint>(island_begin, contact_points.end() - island_begin),
        range,
    });
  }
}

} // namespace

std::vector<ChainDecompositionIsland>
split_chain_decomposition_into_islands(VerticesView vertices, Winding winding,
                                       const ChainDecomposition& chain_decomposition,
                                       ArrayView<const VerticalExtensionContactPoint> contact_points)
{
  PolygonRange chain_range{
      PolygonLocation{
          static_cast<size_t>(chain_decomposition.first_node->vertex_it - vertices.begin()),
          chain_decomposition.first_node->vertex_it->x(),
      },
      PolygonLocation{
          static_cast<size_t>(chain_decomposition.last_node->vertex_it - vertices.begin()),
          chain_decomposition.last_node->vertex_it->x(),
      },
  };

  std::vector<ChainDecompositionIsland> result;
  split_chain_decomposition_into_islands_rec(vertices, winding, chain_decomposition, contact_points, chain_range,
                                             result);

  return result;
}

namespace
{

bool validate_vertical_extension_island(VerticesView vertices, Winding winding, const ChainDecompositionIsland& island)
{
  for (const VerticalExtensionContactPoint& contact_point : island.contact_points)
  {
    if (contact_point.type == VerticalExtensionContactPoint::Type::vertex_downwards ||
        contact_point.type == VerticalExtensionContactPoint::Type::outer_branch_lower_opp_edge)
    {
      Edge expected_lower_opp_edge = ray_cast_down(vertices, winding, island.range, *contact_point.node->vertex_it);
      if (expected_lower_opp_edge != contact_point.node->lower_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.lower_opp_edge should be "
                                      << expected_lower_opp_edge << ", but is " << contact_point.node->lower_opp_edge
                                      << ".");
        return false;
      }
    }

    if (contact_point.type == VerticalExtensionContactPoint::Type::vertex_upwards ||
        contact_point.type == VerticalExtensionContactPoint::Type::outer_branch_lower_opp_edge)
    {
      Edge expected_upper_opp_edge = ray_cast_up(vertices, winding, island.range, *contact_point.node->vertex_it);
      if (expected_upper_opp_edge != contact_point.node->upper_opp_edge)
      {
        UNSCOPED_INFO("Node{vertex: " << *contact_point.node->vertex_it << "}.upper_opp_edge should be "
                                      << expected_upper_opp_edge << ", but is " << contact_point.node->upper_opp_edge
                                      << ".");
        return false;
      }
    }

    if (contact_point.type == VerticalExtensionContactPoint::Type::leaf)
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
  }

  return true;
}

} // namespace

bool validate_vertical_extensions(VerticesView vertices, Winding winding,
                                  ArrayView<const ChainDecompositionIsland> islands)
{
  for (const ChainDecompositionIsland& island : islands)
  {
    if (!validate_vertical_extension_island(vertices, winding, island))
    {
      return false;
    }
  }

  return true;
}

bool validate_vertical_extensions(VerticesView vertices, Winding winding, const std::set<const Node*>& nodes)
{
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
      expected_lower_opp_edge = ray_cast_down(vertices, winding, std::nullopt, *node->vertex_it);
      expected_upper_opp_edge = ray_cast_up(vertices, winding, std::nullopt, *node->vertex_it);
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