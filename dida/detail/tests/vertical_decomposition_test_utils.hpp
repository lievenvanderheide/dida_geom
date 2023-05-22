#pragma once

#include "dida/detail/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// A range of the boundary of a polygon.
///
/// The start and end points of the range are considered part of the range (ie. it's a closed set).
struct PolygonRange
{
  /// The first of the range.
  ///
  /// The first edge should not be a vertical edge.
  size_t first_edge_index;

  /// The number of edges (full or partial) in this range.
  ///
  /// The edge at 'first_edge_index + num_edges - 1' should not be a vertical edge.
  size_t num_edges;

  /// The x-coordinate of the point on the first edge which is the start point of the range.
  ScalarDeg1 start_point_x;

  /// The x-coordinate of the point on the last edge which is the end point of the range.
  ScalarDeg1 end_point_x;
};

/// Casts a ray upwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then an iterator pointing to the start vertex of that edge is returned, if it hits an edge from
/// the exterior side, or if no edge is hit, then @c nullptr is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the right.
///
/// @param vertices The vertices of the polygon.
/// @param range The range.
/// @param ray_origin The point the ray originates from.
/// @return An iterator pointing to the start vertex of the edge.
VertexIt ray_cast_up(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

/// Casts a ray downwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then an iterator pointing to the start vertex of that edge is returned, if it hits an edge from
/// the exterior side, or if no edge is hit, then @c nullptr is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the left.
///
/// @param vertices The vertices of the polygon.
/// @param range The range.
/// @param ray_origin The point the ray originates from.
/// @return An iterator pointing to the start vertex of the edge.
VertexIt ray_cast_down(VerticesView vertices, const PolygonRange& range, Point2 ray_origin);

} // namespace dida::detail::vertical_decomposition
