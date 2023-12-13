#pragma once

#include <set>

#include "dida/detail/vertical_decomposition/vertical_decomposition.hpp"

namespace dida::detail::vertical_decomposition
{

/// A location on the boundary of a polygon.
struct PolygonLocation
{
  /// The index of the edge containing the location.
  ///
  /// The correct edge index for a location which coincides with a vertex depends on the winding of the polygon. If the
  /// winding is @c Winding::ccw, then @c edge_index should be the index of the vertex' outgoing edge, if winding is @c
  /// Winding::cw, it should be the index of the vertex' incoming edge.
  ///
  /// The edge cannot be a vertical edge.
  size_t edge_index;

  /// The x-coordinate of the location.
  ScalarDeg1 x;
};

/// A range of the boundary of a polygon.
///
/// The end points are included in the range.
struct PolygonRange
{
  /// The location of the begining of the range.
  PolygonLocation begin;

  /// The location of the end of the range.
  PolygonLocation end;
};

/// @{
/// Casts a ray in the upwards/downwards direction from @c ray_origin, until it hits an edge in the given polygon or
/// polygon range. If it hits an edge from the interior side, then that edge is returned, if it hits an edge from the
/// exterior side, or if no edge is hit, then @c Edge::invalid() is returned.
///
/// Corner cases are resolved as follows:
///
///  - An edge can only be hit if <tt>lex_less_than(edge_left, ray_origin) && lex_less_than(ray_origin,
///    edge_right)</tt>.
///  - If @c ray_origin lies on an edge, then this edge is ignored.
///  - If a range is provided, and the ray hits the first or last edge of the range, then it's condered a hit iff there
///    would be a hit with the full edge according to the previous two conditions, and the point of intersection does
///    not strictly lie outside the range. Note that this is different from simply clamping the edge and then testing
///    against the clamped edge using the above conditions.
///
Edge ray_cast_up(VerticesView vertices, Winding winding, std::optional<PolygonRange> range, Point2 ray_origin);
Edge ray_cast_down(VerticesView vertices, Winding winding, std::optional<PolygonRange> range, Point2 ray_origin);
/// *}

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