#include "dida/detail/vertical_decomposition.hpp"

#include <catch2/catch.hpp>

namespace dida::detail::vertical_decomposition
{

TEST_CASE("y_on_edge_for_x")
{
  SECTION("Edge start left of edge end")
  {
    YOnEdge y = y_on_edge_for_x(Segment2({-3, 4}, {13, 0}), ScalarDeg1(9));
    CHECK(y == ScalarDeg1(1));
  }

  SECTION("Edge start right of edge end")
  {
    YOnEdge y = y_on_edge_for_x(Segment2({8, 5}, {2, 2}), ScalarDeg1(4));
    CHECK(y == ScalarDeg1(3));
  }
}

TEST_CASE("Edge::edge_from_index")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("General")
  {
    Edge edge = Edge::edge_from_index(vertices, 1);
    CHECK(edge.start_vertex_it == vertices.begin() + 1);
    CHECK(edge.end_vertex_it == vertices.begin() + 2);
  }

  SECTION("With wrap")
  {
    Edge edge = Edge::edge_from_index(vertices, 3);
    CHECK(edge.start_vertex_it == vertices.begin() + 3);
    CHECK(edge.end_vertex_it == vertices.begin() + 0);
  }
}

TEST_CASE("Edge::invalid")
{
  Edge invalid_edge = Edge::invalid();
  CHECK(invalid_edge.start_vertex_it == nullptr);
  CHECK(invalid_edge.end_vertex_it == nullptr);
}

TEST_CASE("Edge::is_valid")
{
  SECTION("Valid")
  {
    std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
    ArrayView<const Point2> vertices(vertices_storage);

    Edge edge = Edge::edge_from_index(vertices, 1);
    CHECK(edge.is_valid());
  }

  SECTION("Invalid")
  {
    Edge edge = Edge::invalid();
    CHECK(!edge.is_valid());
  }
}

TEST_CASE("Edge::segment")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  ArrayView<const Point2> vertices(vertices_storage);
  Edge edge = Edge::edge_from_index(vertices, 0);
  CHECK(edge.segment() == Segment2(vertices[0], vertices[1]));
}

TEST_CASE("Edge::operator==")
{
  std::vector<Point2> vertices_storage{{1.64, 2.04}, {4.52, 1.74}, {5.92, 4.52}, {0.50, 6.34}};
  ArrayView<const Point2> vertices(vertices_storage);

  SECTION("Equal")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 1};
    CHECK(a == b);
  }

  SECTION("start_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin() + 2, vertices.begin() + 1};
    CHECK_FALSE(a == b);
  }

  SECTION("end_vertex_it different")
  {
    Edge a{vertices.begin(), vertices.begin() + 1};
    Edge b{vertices.begin(), vertices.begin() + 2};
    CHECK_FALSE(a == b);
  }
}

} // namespace dida::detail::vertical_decomposition