#include "dida/detail/polygon_sweep.hpp"

#include <catch2/catch.hpp>

#include "dida/math/fraction.hpp"

namespace dida::detail::polygon_sweep
{

struct EmptyActiveEdgeBase
{
};

namespace
{

/// Returns the y-coordinate of the point on @c edge with the given x-coordinate.
///
/// @param edge The edge.
/// @param x The x-coordinate.
/// @return The y-coordinate.
math::Fraction<ScalarDeg2, ScalarDeg1> y_on_edge_for_x(ActiveEdge<EmptyActiveEdgeBase> edge, ScalarDeg1 x)
{
  Point2 left = *edge.left_vertex_it;
  Vector2 dir = *edge.right_vertex_it - *edge.left_vertex_it;
  return math::Fraction<ScalarDeg2, ScalarDeg1>(left.y() * dir.x() + (x - left.x()) * dir.y(), dir.x());
}

/// Initializes the @c active_edges list of @c sweep_state to the state it should have when the sweepline has
/// x-coordinate  @c sweep_x.
///
/// This function assumes general position.
///
/// @param sweep_state The sweep_state to initialize. The @c vertices member should be set, while the @c active_edges
/// list should be empty.
/// @param sweep_position The x-coordinate of the sweepline.
void init_active_edges(SweepState<EmptyActiveEdgeBase>& sweep_state, ScalarDeg1 sweep_x)
{
  DIDA_ASSERT(sweep_state.active_edges.empty());

  VertexIt prev_vertex_it = sweep_state.vertices.end() - 1;
  for (VertexIt vertex_it = sweep_state.vertices.begin(); vertex_it != sweep_state.vertices.end(); ++vertex_it)
  {
    if (prev_vertex_it->x() < sweep_x && sweep_x < vertex_it->x())
    {
      sweep_state.active_edges.push_back(ActiveEdge<EmptyActiveEdgeBase>{{}, prev_vertex_it, vertex_it});
    }
    else if (vertex_it->x() < sweep_x && sweep_x < prev_vertex_it->x())
    {
      sweep_state.active_edges.push_back(ActiveEdge<EmptyActiveEdgeBase>{{}, vertex_it, prev_vertex_it});
    }

    prev_vertex_it = vertex_it;
  }

  std::sort(sweep_state.active_edges.begin(), sweep_state.active_edges.end(),
            [sweep_x](const ActiveEdge<EmptyActiveEdgeBase>& a, const ActiveEdge<EmptyActiveEdgeBase>& b)
            {
              math::Fraction<ScalarDeg2, ScalarDeg1> a_y = y_on_edge_for_x(a, sweep_x);
              math::Fraction<ScalarDeg2, ScalarDeg1> b_y = y_on_edge_for_x(b, sweep_x);
              return a_y < b_y;
            });
}

/// Validates whether the active edges in @c sweep_state have the state they should have when the sweepline has
/// x-coordinate @c sweep_x.
///
/// This function assumes general position.
///
/// @param sweep_state The sweep state with the active edges to validate.
/// @param sweep_x The x-coordinate of the sweepline.
void validate_active_edges(const SweepState<EmptyActiveEdgeBase>& sweep_state, ScalarDeg1 sweep_x)
{
  SweepState<EmptyActiveEdgeBase> ref_sweep_state{sweep_state.vertices, {}};
  init_active_edges(ref_sweep_state, sweep_x);

  REQUIRE(sweep_state.active_edges.size() == ref_sweep_state.active_edges.size());

  for (size_t i = 0; i < sweep_state.active_edges.size(); i++)
  {
    CHECK(sweep_state.active_edges[i].left_vertex_it == ref_sweep_state.active_edges[i].left_vertex_it);
    CHECK(sweep_state.active_edges[i].right_vertex_it == ref_sweep_state.active_edges[i].right_vertex_it);
  }
}

} // namespace

TEST_CASE("Events constructor")
{
  SECTION("General")
  {
    std::vector<Point2> vertices_storage{
        {-2.22, 2.50}, {1.82, 3.78}, {6.74, 1.56},  {4.56, 4.56},
        {6.56, 6.74},  {1.40, 4.98}, {-4.02, 7.12}, {-1.12, 4.56},
    };
    ArrayView<const Point2> vertices(vertices_storage);

    Events events(vertices);

    REQUIRE(events.events.size() == 8);

    CHECK(events.events[0].vertex_it == vertices.begin() + 6);
    CHECK(!events.events[0].incoming_towards_right);
    CHECK(events.events[0].outgoing_towards_right);
    CHECK(!events.events[0].is_concave_corner);

    CHECK(events.events[1].vertex_it == vertices.begin());
    CHECK(!events.events[1].incoming_towards_right);
    CHECK(events.events[1].outgoing_towards_right);
    CHECK(!events.events[1].is_concave_corner);

    CHECK(events.events[2].vertex_it == vertices.begin() + 7);
    CHECK(events.events[2].incoming_towards_right);
    CHECK(!events.events[2].outgoing_towards_right);
    CHECK(events.events[2].is_concave_corner);

    CHECK(events.events[3].vertex_it == vertices.begin() + 5);
    CHECK(!events.events[3].incoming_towards_right);
    CHECK(!events.events[3].outgoing_towards_right);

    CHECK(events.events[4].vertex_it == vertices.begin() + 1);
    CHECK(events.events[4].incoming_towards_right);
    CHECK(events.events[4].outgoing_towards_right);

    CHECK(events.events[5].vertex_it == vertices.begin() + 3);
    CHECK(!events.events[5].incoming_towards_right);
    CHECK(events.events[5].outgoing_towards_right);
    CHECK(events.events[5].is_concave_corner);

    CHECK(events.events[6].vertex_it == vertices.begin() + 4);
    CHECK(events.events[6].incoming_towards_right);
    CHECK(!events.events[6].outgoing_towards_right);
    CHECK(!events.events[6].is_concave_corner);

    CHECK(events.events[7].vertex_it == vertices.begin() + 2);
    CHECK(events.events[7].incoming_towards_right);
    CHECK(!events.events[7].outgoing_towards_right);
    CHECK(!events.events[7].is_concave_corner);
  }

  SECTION("Horizontal edges")
  {
    std::vector<Point2> vertices_storage{{-5, 2}, {2, 2}, {2, 5}, {-5, 5}};
    ArrayView<const Point2> vertices(vertices_storage);

    Events events(vertices);

    REQUIRE(events.events.size() == 4);

    CHECK(events.events[0].vertex_it == vertices.begin() + 0);
    CHECK(!events.events[0].incoming_towards_right);
    CHECK(events.events[0].outgoing_towards_right);
    CHECK(!events.events[0].is_concave_corner);

    CHECK(events.events[1].vertex_it == vertices.begin() + 3);
    CHECK(!events.events[1].incoming_towards_right);
    CHECK(!events.events[1].outgoing_towards_right);

    CHECK(events.events[2].vertex_it == vertices.begin() + 1);
    CHECK(events.events[2].incoming_towards_right);
    CHECK(events.events[2].outgoing_towards_right);

    CHECK(events.events[3].vertex_it == vertices.begin() + 2);
    CHECK(events.events[3].incoming_towards_right);
    CHECK(!events.events[3].outgoing_towards_right);
  }
}

TEST_CASE("ActiveEdge::point_below_edge/point_above_edge")
{
  std::vector<Point2> vertices_storage{{4, 2}, {10, 4}, {8, 6}};
  ArrayView<const Point2> vertices(vertices_storage);

  ActiveEdge<EmptyActiveEdgeBase> active_edge{{}, vertices.begin(), vertices.begin() + 1};

  SECTION("Point below")
  {
    Point2 point(6.72, 1.68);
    CHECK(active_edge.point_below_edge(point));
    CHECK_FALSE(active_edge.point_above_edge(point));
  }

  SECTION("Point on edge")
  {
    Point2 point(7, 3);
    CHECK_FALSE(active_edge.point_below_edge(point));
    CHECK_FALSE(active_edge.point_above_edge(point));
  }

  SECTION("Point above")
  {
    Point2 point(7.48, 4.7);
    CHECK_FALSE(active_edge.point_below_edge(point));
    CHECK(active_edge.point_above_edge(point));
  }
}

TEST_CASE("insert_location")
{
  std::vector<Point2> vertices_storage{{3, 2}, {9, 4}, {4, 6}, {9, 8}, {1, 8}, {2, 5}, {6, 4}, {0, 2}, {7, 0}};
  ArrayView<const Point2> vertices(vertices_storage);

  SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
  init_active_edges(sweep_state, ScalarDeg1(5));

  SECTION("Between edges")
  {
    InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, Point2(5, 2));
    CHECK(result.location_it == sweep_state.active_edges.begin() + 2);
    CHECK(!result.on_edge);
  }

  SECTION("Below lowest edge")
  {
    InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, Point2(5, -1));
    CHECK(result.location_it == sweep_state.active_edges.begin());
    CHECK(!result.on_edge);
  }

  SECTION("Above highest edge")
  {
    InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, Point2(5, 9));
    CHECK(result.location_it == sweep_state.active_edges.end());
    CHECK(!result.on_edge);
  }

  SECTION("On edge")
  {
    InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, Point2(5, 1));
    CHECK(result.location_it == sweep_state.active_edges.begin() + 2);
    CHECK(result.on_edge);
  }

  SECTION("With limited range")
  {
    SECTION("Below lowest edge in range")
    {
      InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, 2, 5, Point2(5, 0));
      CHECK(result.location_it == sweep_state.active_edges.begin() + 2);
      CHECK(!result.on_edge);
    }

    SECTION("Between edges")
    {
      InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, 2, 5, Point2(5, 4));
      CHECK(result.location_it == sweep_state.active_edges.begin() + 4);
      CHECK(!result.on_edge);
    }

    SECTION("Above lowest edge in range")
    {
      InsertLocation<EmptyActiveEdgeBase> result = insert_location(sweep_state, 2, 5, Point2(5, 9));
      CHECK(result.location_it == sweep_state.active_edges.begin() + 5);
      CHECK(!result.on_edge);
    }
  }
}

TEST_CASE("active_edge_with_right_vertex")
{
  std::vector<Point2> vertices_storage{{-1.94, 1.64}, {3.24, 3.28}, {-0.38, 3.72}, {3.74, 5.94}, {-3.26, 5.22}};
  ArrayView<const Point2> vertices(vertices_storage);

  SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
  init_active_edges(sweep_state, ScalarDeg1(0));

  ActiveEdgesIt<EmptyActiveEdgeBase> it = active_edge_with_right_vertex(sweep_state, vertices.begin() + 3);
  CHECK(it == sweep_state.active_edges.begin() + 2);
}

TEST_CASE("handle_appear_event")
{
  std::vector<Point2> vertices_storage{
      {-5.34, 2.08}, {2.92, 1.54}, {1.24, 3.14}, {4.22, 4.90}, {4.14, 0.72}, {0.90, -0.80}, {5.84, -2.10}, {5.68, 8.02},
  };
  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("Convex corner")
  {
    SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
    init_active_edges(sweep_state, vertices[5].x() - ScalarDeg1::from_numerator(1));

    Event appear_event{vertices.begin() + 5, false, true, false};
    ActiveEdgesIt<EmptyActiveEdgeBase> new_edges_it =
        handle_appear_event(sweep_state, insert_location(sweep_state, *appear_event.vertex_it), appear_event);
    CHECK(new_edges_it == sweep_state.active_edges.begin());

    validate_active_edges(sweep_state, vertices[5].x() + ScalarDeg1::from_numerator(1));
  }

  SECTION("Concave corner")
  {
    SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
    init_active_edges(sweep_state, vertices[2].x() - ScalarDeg1::from_numerator(1));

    Event appear_event{vertices.begin() + 2, false, true, true};
    ActiveEdgesIt<EmptyActiveEdgeBase> new_edges_it =
        handle_appear_event(sweep_state, insert_location(sweep_state, *appear_event.vertex_it), appear_event);
    CHECK(new_edges_it == sweep_state.active_edges.begin() + 3);

    validate_active_edges(sweep_state, vertices[2].x() + ScalarDeg1::from_numerator(1));
  }
}

TEST_CASE("handle_transition_event")
{
  std::vector<Point2> vertices_storage{{-4.42, 1.00}, {1.90, 3.06}, {6.98, 1.72}, {-0.66, 5.66}};
  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("Forward")
  {
    SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
    init_active_edges(sweep_state, vertices[1].x() - ScalarDeg1::from_numerator(1));

    Event transition_event{vertices.begin() + 1, true, true, {}};
    handle_transition_event(sweep_state, active_edge_with_right_vertex(sweep_state, transition_event.vertex_it),
                            transition_event);

    validate_active_edges(sweep_state, vertices[1].x() + ScalarDeg1::from_numerator(1));
  }

  SECTION("Reverse")
  {
    SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
    init_active_edges(sweep_state, vertices[3].x() - ScalarDeg1::from_numerator(1));

    Event transition_event{vertices.begin() + 3, false, false, {}};
    handle_transition_event(sweep_state, active_edge_with_right_vertex(sweep_state, transition_event.vertex_it),
                            transition_event);

    validate_active_edges(sweep_state, vertices[3].x() + ScalarDeg1::from_numerator(1));
  }
}

TEST_CASE("handle_vanish_event")
{
  std::vector<Point2> vertices_storage{{-2.64, 2.96}, {5.60, 1.48}, {1.14, 4.12}, {7.30, 5.96}, {0.56, 6.48}};
  ArrayView<const Point2> vertices(vertices_storage);

  SweepState<EmptyActiveEdgeBase> sweep_state{vertices, {}};
  init_active_edges(sweep_state, vertices[1].x() - ScalarDeg1::from_numerator(1));

  {
    Event vanish_event{vertices.begin() + 1, true, false, false};
    handle_vanish_event(sweep_state, active_edge_with_right_vertex(sweep_state, vanish_event.vertex_it));
    validate_active_edges(sweep_state, vertices[1].x() + ScalarDeg1::from_numerator(1));
  }

  {
    Event vanish_event{vertices.begin() + 3, true, false, false};
    handle_vanish_event(sweep_state, active_edge_with_right_vertex(sweep_state, vanish_event.vertex_it));
    validate_active_edges(sweep_state, vertices[3].x() + ScalarDeg1::from_numerator(1));
  }
}

} // namespace dida::detail::polygon_sweep