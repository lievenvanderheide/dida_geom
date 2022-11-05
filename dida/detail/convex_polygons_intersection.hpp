#pragma once

#include <algorithm>

#include "dida/convex_polygon2.hpp"
#include "dida/perturbation.hpp"

namespace dida::detail
{

namespace convex_polygons_intersection
{

/// An arc of a convex polygon.
enum class Arc
{
  /// The lower arc of a convex polygon. This arc starts at the lexicographical leftmost vertex and ends in the
  /// lexicographical rightmost vertex with edges are traversed in the usual counter clockwise direction.
  lower,

  /// The lower arc of a convex polygon. This arc starts at the lexicographical rightmost vertex and ends in the
  /// lexicographical leftmost vertex with edges are traversed in the usual counter clockwise direction.
  upper,
};

/// Given an arc, returns the other arc.
///
/// @param arc The arc.
/// @return The other arc.
constexpr Arc other_arc(Arc arc);

/// A struct containing an input polygon and some info related to it.
struct PolygonInfo
{
  /// Constructs a @c PolygonInfo for the given polygon.
  ///
  /// @param _polygon The polygon.
  explicit PolygonInfo(ConvexPolygonView2 _polygon);

  /// The polygon.
  ConvexPolygonView2 polygon;

  /// The leftmost vertex of @c polygon.
  ConvexPolygonView2::const_iterator leftmost_it;

  /// The rightmost vertex of @c polygon.
  ConvexPolygonView2::const_iterator rightmost_it;
};

/// An edge of a polygon, used while iterating over the polygon's edges in the forward direction.
struct ForwardEdge
{
  /// An iterator pointing to the end vertex of this edge.
  ConvexPolygonView2::const_iterator end_it;

  /// The vector from the edge's start point to its end point.
  Vector2 dir;
};

/// An edge of a polygon, used while iterating over the polygon's edges in the reverse direction.
struct ReverseEdge
{
  /// An iterator pointing to the start vertex of this edge.
  ConvexPolygonView2::const_iterator start_it;

  /// The vector from the edge's start point to its end point.
  Vector2 dir;
};

/// Given two sweep positions, returns whether sweep position @c a comes before sweep position @c b, for a sweep over
/// arc @c arc. An infinitesimal offset specified by @c b_perturbation_vector is added to the second sweep line to make,
/// which determines the ordering in case the two sweep positions are otherwise equal.
///
/// @tparam arc The arc.
/// @tparam b_perturbation_vector The perturbation vector to be added to @c b.
/// @param a The first sweep position.
/// @param b The second sweep position.
/// @return True iff sweep position @c a comes before sweep position @c b.
template <Arc arc, PerturbationVector2 b_perturbation_vector>
bool sweep_position_less_than(ScalarDeg1 a, ScalarDeg1 b);

/// Returns an iterator to the first vertex of the given arc in @c polygon_info.
///
/// @tparam arc The arc.
/// @param polygon_info The @c PolygonInfo of the polygon.
/// @return An iterator to the first vertex of the arc.
template <Arc arc>
ConvexPolygonView2::const_iterator arc_first_vertex_it(const PolygonInfo& polygon_info);

/// Returns an iterator to the last vertex of the given arc in @c polygon_info.
///
/// @tparam arc The arc.
/// @param polygon_info The @c PolygonInfo of the polygon.
/// @return An iterator to the last vertex of the arc.
template <Arc arc>
ConvexPolygonView2::const_iterator arc_last_vertex_it(const PolygonInfo& polygon_info);

/// Returns the first edge of a forward traversal of the edges of the given arc of @c polygon_info.
///
/// @tparam arc The arc.
/// @param polygon_info The @c PolygonInfo of the polygon.
/// @return The first forward edge.
template <Arc arc>
ForwardEdge arc_first_forward_edge(const PolygonInfo& polygon_info);

/// Returns the first edge of a reverse traversal of the edges of the given arc of @c polygon_info.
///
/// @tparam arc The arc.
/// @param polygon_info The @c PolygonInfo of the polygon.
/// @return The first reverse edge.
template <Arc arc>
ReverseEdge arc_first_reverse_edge(const PolygonInfo& polygon_info);

/// Returns the @c ForwardEdge of the given arc of @c polygon_info, which intersects the sweep line with x-coordinate @c
/// sweep_position.
///
/// @pre The sweep line should intersect the polygon.
/// @tparam arc The arc.
/// @tparam point_perturbation_vector The perturbation offset added to @c point to resolve cases where the sweep line
/// intersects a vertex of the arc.
/// @param polygon_info The @c PolygonInfo.
/// @param sweep_position The sweep position.
/// @return The edge which intersects the sweep line.
template <Arc arc, PerturbationVector2 point_perturbation_vector>
ForwardEdge forward_edge_for_sweep_position(const PolygonInfo& polygon_info, ScalarDeg1 sweep_position);

/// Returns the @c ReverseEdge of the given arc of @c polygon_info, which intersects the sweep line with x-coordinate @c
/// sweep_position.
///
/// @pre The sweep line should intersect the polygon.
/// @tparam arc The arc.
/// @tparam point_perturbation_vector The perturbation offset added to @c point to resolve cases where the sweep line
/// intersects a vertex of the arc.
/// @param polygon_info The @c PolygonInfo.
/// @param sweep_position The sweep position.
/// @return The edge which intersects the sweep line.
template <Arc arc, PerturbationVector2 point_perturbation_vector>
ReverseEdge reverse_edge_for_sweep_position(const PolygonInfo& polygon_info, ScalarDeg1 sweep_position);

/// Advances @c edge to the next forward edge. If @c edge is the last edge of the arc, @c false is returned.
///
/// @tparam The arc which contains @c edge.
/// @param polygon_info The @c PolygonInfo.
/// @param edge The edge to advance.
/// @return True if @c edge was successfully advanced, false if the end of the arc was reached.
template <Arc arc>
bool advance_forward_edge(const PolygonInfo& polygon_info, ForwardEdge& edge);

/// Advances @c edge to the next reverse edge. If @c edge is the first edge of the arc, @c false is returned.
///
/// @tparam The arc which contains @c edge.
/// @param polygon_info The @c PolygonInfo.
/// @param edge The edge to advance.
/// @return True if @c edge was successfully advanced, false if the beginning of the arc was reached.
template <Arc arc>
bool advance_reverse_edge(const PolygonInfo& polygon_info, ReverseEdge& edge);

} // namespace convex_polygons_intersection

} // namespace dida::detail

#include "dida/detail/convex_polygons_intersection.inl"