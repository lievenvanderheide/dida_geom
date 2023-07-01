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

} // namespace dida::detail::vertical_decomposition