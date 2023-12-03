#pragma once

#include <set>

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// A location on the boundary of a polygon.
struct PolygonLocation
{
  /// The index of the edge containing the location.
  size_t edge_index;

  /// The x-coordinate of the location.
  ScalarDeg1 x;
};

/// A function object for comparing polygon locations. The () operator returns true iff the first operand is encountered
/// before the second operand, when traversing the polygon's boundary while starting from its first vertex.
struct PolygonLocationLessThan
{
  /// The vertices of the polygon.
  VerticesView vertices;

  /// Compares two @c PolygonLocation instances.
  bool operator()(const PolygonLocation& a, const PolygonLocation& b) const;
};

/// A range of the boundary of a polygon.
struct PolygonRange
{
  /// The first of the range.
  size_t first_edge_index;

  /// The number of edges (full or partial) in this range.
  size_t num_edges;

  /// The x-coordinate of the point on the first edge which is the start point of the range.
  ScalarDeg1 start_point_x;

  /// The x-coordinate of the point on the last edge which is the end point of the range.
  ScalarDeg1 end_point_x;

  /// Splits this @c PolygonRange at the given location, returning the two sub-ranges.
  std::pair<PolygonRange, PolygonRange> split(VerticesView vertices, PolygonLocation location) const;
};

/// Casts a ray upwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then that edge is returned, if it hits an edge from the exterior side, or if no edge is hit, then
/// @c Edge::invalid() is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the right.
Edge ray_cast_up(VerticesView vertices, Winding winding, const PolygonRange& range, Point2 ray_origin);

/// Casts a ray downwards from @c ray_origin, until it hits an edge in the given polygon range. If it hits an edge from
/// the interior side, then that edge is returned, if it hits an edge from the exterior side, or if no edge is hit, then
/// @c Edge::invalid() is returned.
///
/// The special case where the ray hits the boundary on a vertex is resolved by shifting that vertex an infinitisemal
/// offset to the left.
Edge ray_cast_down(VerticesView vertices, Winding winding, const PolygonRange& range, Point2 ray_origin);

/// A contact point where a vertical extension meets its chain.
struct VerticalExtensionContactPoint
{
  /// The type of contact point.
  enum class Type
  {
    /// The contact point of a downward vertical extension and the vertex it originates from.
    vertex_downwards,

    /// The contact point of an upward vertical extension and the vertex it originates from.
    vertex_upwards,

    /// The contact point of an upward vertical extension and its @c node->lower_opp_edge.
    lower_opp_edge,

    /// The contact point of a downward vertical extension and its @c node->upper_opp_edge.
    upper_opp_edge,

    /// The contact point is the vertex of @c node, and @c node is a leaf node.
    leaf,
  };

  /// The type of this @c contact point.
  Type type;

  /// The node.
  const Node* node;

  /// Compares two contact points for equality.
  bool operator==(const VerticalExtensionContactPoint& b) const;

  /// Returns whether this contact point compares less than contact point @c b.
  ///
  /// This ordering is provided so that contact points can be sorted, but is otherwise not very meaningful.
  bool operator<(const VerticalExtensionContactPoint& b) const;
};

/// Returns the given type as a string.
std::string_view contact_point_type_to_string(VerticalExtensionContactPoint::Type type);

/// Returns the @c VerticalExtensionContactPoint of the given chain decomposition, in the order they're encountered when
/// following the chain from beginning to end.
std::vector<VerticalExtensionContactPoint>
vertical_extension_contact_points(const ChainDecomposition& chain_decomposition, Winding windig);

/// An island of a chain decomposition consists of a subset of the chain's boundary, such that for a valid
/// decomposition, the following conditions must hold:
///
///  - Finite vertical extensions may only connect contact points within the same island.
///  - Finite vertical extensions do not cross the boundary.
///  - No infinite vertical extensions belong to an island (in fact, infinite extensions determine island boundaries).
///
struct ChainDecompositionIsland
{
  /// The contact points of the vertical extensions in this island.
  ArrayView<const VerticalExtensionContactPoint> contact_points;

  /// The part of the polygon's boundary belonging to this island.
  PolygonRange range;
};

/// Splits a chain decomposition into its set of islands.
std::vector<ChainDecompositionIsland>
split_chain_decomposition_into_islands(VerticesView vertices, Winding winding,
                                       const ChainDecomposition& chain_decomposition,
                                       ArrayView<const VerticalExtensionContactPoint> contact_points);

/// Validates the vertical extensions in the given islands.
bool validate_vertical_extensions(VerticesView vertices, ArrayView<const ChainDecompositionIsland> islands);

/// Validates the vertical extensions in the decomposition of the full polygon formed by @c vertices.
///
/// This function assumes there are no infinite vertical extensions, so can't be used to validate chain decompositions.
/// To validate chain decompositions, use the above overload of @c validate_vertical_extensions.
bool validate_vertical_extensions(VerticesView vertices, const std::set<const Node*>& nodes);

} // namespace dida::detail::vertical_decomposition