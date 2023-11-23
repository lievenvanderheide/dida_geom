#include "dida/convex_polygon2_utils.hpp"

#include <algorithm>

namespace dida
{

namespace
{

/// Returns the support vertex of @c polygon in the direction specified using a @c compare_points function. If there's a
/// tie, then the vertex at the end of the edge connecting the two candidates is returned.
///
/// The user specified <tt>bool compare_points(Point2 a, Point2 b)</tt> function should return true iff the projection
/// of point
/// @c a onto the direction of interest is less than or equal to the projection of point @c b onto that direction. For
/// example, if we want to find the support vertex in the +X direction, then the following function can be used:
///
///  bool compare_points(Point2 a, Point2 b)
///  {
///    return a.x() <= b.x();
///  }
///
/// @tparam ComparePoints The type of the @c compare_points function.
/// @param polygon The polygon.
/// @param compare_points The @c compare_points function.
/// @return An iterator pointing to
template <class ComparePoints>
ConvexPolygonView2::const_iterator support_vertex(ConvexPolygonView2 polygon, ComparePoints compare_points)
{
  ConvexPolygonView2::const_iterator range_begin_it = polygon.begin();
  ConvexPolygonView2::const_iterator range_begin_prev_it = polygon.end() - 1;
  size_t range_size = polygon.size();

  while (range_size != 1)
  {
    size_t range_mid_offset = range_size / 2;
    ConvexPolygonView2::const_iterator range_mid_it = range_begin_it + range_mid_offset;
    if (compare_points(*range_begin_it, *range_mid_it))
    {
      if (compare_points(*(range_mid_it - 1), *range_mid_it))
      {
        // Consider the cone with apex 'range_mid', bounded by the rays in the directions 'range_begin - range_mid' and
        // 'prev(range_mid) - range_mid'. This cone fully contains the first half of the range, while the apex
        // ('range_mid') is the cone's farthest point in the direction of interest.
        //
        // Since 'range_mid' is part of the second half, we can fully discard the first half.
        //
        // A special case is the case when one or both rays are perpendicular to the direction of interest. In that
        // case, 'apex' is tied with other points in the cone for being the farthest point in the direction of interest.
        // The vertex we want to find is either 'range_mid', 'next(range_mid)', or another vertex not in the cone. In
        // the first and last case, the vertex must be in the second half, so we're fine. The only potential problematic
        // case is if 'next(range_mid)' were in the first half, however, for this to be the case, 'next(range_mid)' must
        // be the first vertex of the current range, 'range_mid' the last vertex of the current range, and since a
        // polygon has at least 3 vertices, there must be at least one other vertex between those two, however, this
        // means that a different 'range_mid' vertex should have been selected, so we have a contradiction, and the
        // assumption that 'next(range_mid)' lies in the first half must be false.

        range_begin_it = range_mid_it;
        range_begin_prev_it = range_mid_it - 1;
        range_size -= range_mid_offset;
      }
      else
      {
        // Consider the cone with apex 'range_mid', bounded by the rays in the direction 'range_begin - range_mid' and
        // 'range_mid - prev(range_mid)'. This cone fully contains the second half of the range, while the apex
        // ('range_mid') is the cone's point farthest in the direction of interest (or a tied fathest point in the case
        // where there's a ray perpendicular to the direction of interest).
        //
        // Since 'prev(range_mid)' is even farther in the direction of interest than 'range_mid', and 'range_mid' was
        // already (one of) the farthest point(s) of the cone, we can fully discard the half contained in the cone.

        range_size = range_mid_offset;
      }
    }
    else
    {
      if (compare_points(*range_begin_prev_it, *range_begin_it))
      {
        // Consider the cone with apex 'range_begin' bounded by the rays in the directions 'range_mid - range_begin' and
        // 'prev(range_begin) - range_begin'. This cone fully contains the first half of the range, while the apex
        // ('range_begin') is the cone's farthest point in the direction of interest.
        //
        // Since 'range_begin' is part of the first half, this means we can fully discard the second half.
        //
        // A special case is the case when the second ray is perpendicular to the direction of interest. In that case,
        // 'apex' is tied with other points in the cone for being the farthest point in the direction of interest. The
        // vertex we're trying to find is either 'range_begin' 'next(range_begin)' or another vertex not contained in
        // the cone. In the first and last case, the vertex must be in the first half, so we're fine. The only potential
        // problematic case is if 'next(range_mid)' were equal to 'range_mid', however, since we know that the ray from
        // 'range_begin' to 'range_mid' is not perpendicular to the direction of interest, we know this can't be the
        // case, so 'next(range_begin)' is in the first half too.

        range_size = range_mid_offset;
      }
      else
      {
        // Consider the cone with apex 'range_begin' bounded by the rays in the directions 'range_mid - range_begin' and
        // 'range_begin - prev(range_begin)'. This cone fully contains the second half of the range, while the apex
        // ('range_begin') is the cone's farthest point in the direction of interest.
        //
        // Since 'prev(range_begin)' is even farther in the direction of interest than 'range_begin', and 'range_begin'
        // was already the farthest point of the cone, we can fully discard the first half contained in the cone.
        //
        // It's not possible in this block that either of the rays are perpendicular to the direction of interest, so
        // there are no special cases to consider.

        range_begin_it = range_mid_it;
        range_begin_prev_it = range_mid_it - 1;
        range_size -= range_mid_offset;
      }
    }
  }

  return range_begin_it;
}

} // namespace

ConvexPolygonView2::const_iterator leftmost_vertex(ConvexPolygonView2 polygon)
{
  return support_vertex(polygon, [](Point2 a, Point2 b) { return a.x() >= b.x(); });
}

ConvexPolygonView2::const_iterator rightmost_vertex(ConvexPolygonView2 polygon)
{
  return support_vertex(polygon, [](Point2 a, Point2 b) { return a.x() <= b.x(); });
}

} // namespace dida