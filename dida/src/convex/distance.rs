use crate::{ConvexPolygonView, Point2, Vec2};
use crate::convex::convex_arc_bisector::ConvexArcBisector;

/// Returns the squared distance between convex polygons `a` and `b`.
pub fn distance_squared(a: ConvexPolygonView, b: ConvexPolygonView) -> f64 {
    let mut a_bisector = ConvexArcBisector::new_with_lower_then_upper_arc(a);
    let mut b_bisector = ConvexArcBisector::new_with_upper_then_lower_arc(b);

    // The following invariant is maintained: Given the Minkowski difference of polygons `a` and `b`, and the diagonal
    // of the Minkowski difference from `v0` to v2`, the origin is contained in the `DiagonalVoronoiRegion::Right`
    // region.

    let mut v0 = Point2::from_vec2(a_bisector.begin_vertex() - b_bisector.begin_vertex());
    let mut v2 = Point2::from_vec2(a_bisector.mid_vertex() - b_bisector.mid_vertex());
    match diagonal_voronoi_region(v0, v2, Point2::new(0.0, 0.0)) {
        DiagonalVoronoiRegion::Right => {
            a_bisector.move_cw();
            b_bisector.move_cw();
        },
        DiagonalVoronoiRegion::Left => {
            a_bisector.move_ccw();
            b_bisector.move_ccw();
            std::mem::swap(&mut v0, &mut v2);
        },
    }

    while a_bisector.num_edges() + b_bisector.num_edges() > 1 {
        let bisect_direction;
        if a_bisector.num_edges() == 1 && b_bisector.num_edges() == 1 {
            // If both bisectors have 1 edge left, then there are 2 edges left in the Minkowski difference, so we still
            // need to bisect. We need to treat this as a special case though, because neither of the other 2 branches
            // of this if statement will be able to guarantee that the selected mid vertices result in a v1 different
            // from v0 or v2, so they wouldn't be able to guarantee that the function will terminate.
            if Vec2::cross(a_bisector.begin_vertex_outgoing(), b_bisector.begin_vertex_outgoing()) > 0.0 {
                bisect_direction = b_bisector.begin_vertex_outgoing().left_perpendicular();

                a_bisector.bisect_at(a_bisector.num_edges());
                b_bisector.bisect_at(0);
            } else {
                bisect_direction = a_bisector.begin_vertex_outgoing().right_perpendicular();

                a_bisector.bisect_at(0);
                b_bisector.bisect_at(b_bisector.num_edges());
            }
        } else if a_bisector.num_edges() > b_bisector.num_edges() {
            a_bisector.bisect();
            bisect_direction = a_bisector.mid_vertex_outgoing().right_perpendicular();
            b_bisector.bisect_at_support_vertex(-bisect_direction);
        } else {
            b_bisector.bisect();
            bisect_direction = b_bisector.mid_vertex_outgoing().left_perpendicular();
            a_bisector.bisect_at_support_vertex(bisect_direction);
        }

        let v1 = Point2::from_vec2(a_bisector.mid_vertex() - b_bisector.mid_vertex());
        match triangle_voronoi_region(v0, v1, v2, bisect_direction, Point2::new(0.0, 0.0)) {
            TriangleVoronoiRegion::EdgeV0V1 => {
                a_bisector.move_cw();
                b_bisector.move_cw();
                v2 = v1;
            },
            TriangleVoronoiRegion::EdgeV1V2 => {
                a_bisector.move_ccw();
                b_bisector.move_ccw();
                v0 = v1;
            },
            TriangleVoronoiRegion::Interior => return 0.0,
        }
    }  

    let origin = Point2::new(0.0, 0.0);
    let edge_sqr_len = (v2 - v0).sqr_len();
    let origin_onto_edge = Vec2::dot(v2 - v0, origin - v0);
    if origin_onto_edge <= 0.0 {
        (v0 - origin).sqr_len().as_f64()
    } else if origin_onto_edge >= edge_sqr_len {
        (v2 - origin).sqr_len().as_f64()
    } else {
        let origin_onto_edge_normal = Vec2::cross(v2 - v0, origin - v0);
        origin_onto_edge_normal.as_f64() * origin_onto_edge_normal.as_f64() / edge_sqr_len.as_f64()
    }
}

/// Given the diagonal between two vertices of a convex polygon, the `DiagonalVoronoiRegion` of the left/right side of
/// the diagonal is the union of the Voronoi cells of the polygon associated with the edges and vertices on that side,
/// as well as the part of the interior on that side.
///
/// The Voronoi cells corresponding to the vertices used by the diagonal are considered to be part of both
/// `DiagonalVoronoiRegion::Left` and `DiagonalVoronoiRegion::Right`.
#[derive(PartialEq, Debug)]
enum DiagonalVoronoiRegion {
    Right,
    Left,
}

/// If the line segment from `leftmost_vertex` to `rightmost_vertex` is the diagonal between the leftmost and rightmost
/// vertex of a convex polygon, then this function returns the `DiagonalVoronoiRegion` which contains `point`. See
/// `DiagonalVoronoiRegion` for more details.
fn diagonal_voronoi_region(leftmost_vertex: Point2, rightmost_vertex: Point2, point: Point2) -> DiagonalVoronoiRegion {
    if point.x() < leftmost_vertex.x() {
        if point.y() < leftmost_vertex.y() {
            DiagonalVoronoiRegion::Right
        } else {
            DiagonalVoronoiRegion::Left
        }
    } else if point.x() > rightmost_vertex.x() {
        if point.y() < rightmost_vertex.y() {
            DiagonalVoronoiRegion::Right
        } else {
            DiagonalVoronoiRegion::Left
        }
    } else {
        if Vec2::cross(point - leftmost_vertex, rightmost_vertex - leftmost_vertex) > 0.0 {
            DiagonalVoronoiRegion::Right
        } else {
            DiagonalVoronoiRegion::Left
        }
    }
}

#[derive(PartialEq, Debug)]
enum TriangleVoronoiRegion {
    /// The same as `DiagonalVoronoiRegion::Right` for the diagonal from `v0` to `v1`.
    EdgeV0V1,

    /// The same as `DiagonalVoronoiRegion::Right` for the diagonal from `v1` to `v2`.
    EdgeV1V2,

    /// The interior of the triangle.
    Interior,
}

/// Given the triangle formed by 3 vertices of a convex polygon, such that it's already known that relative to the
/// diagonal from `v0` to `v2`, `point` lies in the in the `DiagonalVoronoiRegion::Right` region, returns the
/// `TriangleVoronoiRegion` containing `point`. See `TriangleVoronoiRegion` for more details.
///
/// `v0` and `v2` must be distinct, while `v1` can be any vertex on the arc form `v0` to `v2` (including the degenerate
/// cases where `v1` is equal to `v0` or `v2`).
///
/// `v1_support_direction` must be some direction for which `v1` is the support vertex of the polygon.  
fn triangle_voronoi_region(
    v0: Point2,
    v1: Point2,
    v2: Point2,
    v1_support_direction: Vec2,
    point: Point2
) -> TriangleVoronoiRegion {
    let v0v1_side = Vec2::cross(point - v1, v1 - v0);
    let v1v2_side = Vec2::cross(point - v1, v2 - v1);

    if v0v1_side >= 0.0 && v1v2_side >= 0.0 {
        if Vec2::cross(point - v1, v1_support_direction) > 0.0 {
            TriangleVoronoiRegion::EdgeV0V1
        } else {
            TriangleVoronoiRegion::EdgeV1V2
        }
    } else if v0v1_side > 0.0 {
        TriangleVoronoiRegion::EdgeV0V1
    } else if v1v2_side > 0.0 {
        TriangleVoronoiRegion::EdgeV1V2
    } else if Vec2::dot(point - v0, v0 - v2) > 0.0 {
        TriangleVoronoiRegion::EdgeV0V1
    } else if Vec2::dot(point - v2, v2 - v0) > 0.0 {
        TriangleVoronoiRegion::EdgeV1V2
    } else {
        TriangleVoronoiRegion::Interior
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ConvexPolygon;
    use std::str::FromStr;

    #[test]
    fn test_distance_squared() {
        let a = ConvexPolygon::from_str(
            "{{2, 6}, {3, 2}, {5, -1}, {12, 3}, {14, 8}, {15, 12}, {14, 14}, {12, 16}, {8, 16}, {4, 13}, {2, 9}}"
        ).unwrap();

        // The closest distance is realized between two vertices.
        let b = ConvexPolygon::from_str("{{15, -1}, {17, -3}, {19, -4}, {20, -2}, {19, 0}, {14, 2}}").unwrap();
        std::assert_eq!(distance_squared(a.as_view(), b.as_view()), 5.0);
        std::assert_eq!(distance_squared(b.as_view(), a.as_view()), 5.0);
        
        // The closest distance is realized between an edge and a vertex.
        let b = ConvexPolygon::from_str("{{-1, 13}, {-2, 16}, {-3, 18}, {-4, 15}, {-3, 11}}").unwrap();
        std::assert_eq!(distance_squared(a.as_view(), b.as_view()), 20.0);
        std::assert_eq!(distance_squared(b.as_view(), a.as_view()), 20.0);
        
        // Intersecting
        let b = ConvexPolygon::from_str("{{3, -3}, {9, -2}, {9, 3}, {6, 6}, {1, 2}}").unwrap();
        std::assert_eq!(distance_squared(a.as_view(), b.as_view()), 0.0);
        std::assert_eq!(distance_squared(b.as_view(), a.as_view()), 0.0);

        // With parallel edges
        let a = ConvexPolygon::from_str("{{-2, 2}, {4, 2}, {4, 5}, {-2, 5}}").unwrap();
        let b = ConvexPolygon::from_str("{{2, 1}, {2, -1}, {6, -1}, {6, 1}}").unwrap();
        std::assert_eq!(distance_squared(a.as_view(), b.as_view()), 1.0);
        //std::assert_eq!(distance_squared(b.as_view(), a.as_view()), 1.0);
    }

    #[test]
    fn test_diagonal_voronoi_region() {
        let v0 = Point2::new(-5.0, 3.0);
        let v1 = Point2::new(4.0, -1.0);

        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(-7.0, 1.0)), DiagonalVoronoiRegion::Right);
        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(-2.0, 1.0)), DiagonalVoronoiRegion::Right);
        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(6.0, -2.0)), DiagonalVoronoiRegion::Right);

        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(-7.0, 5.0)), DiagonalVoronoiRegion::Left);
        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(-4.0, 3.0)), DiagonalVoronoiRegion::Left);
        std::assert_eq!(diagonal_voronoi_region(v0, v1, Point2::new(7.0, 0.0)), DiagonalVoronoiRegion::Left);
    }

    #[test]
    fn test_triangle_voronoi_region() {
        let v0 = Point2::new(3.0, -3.0);
        let v1 = Point2::new(2.0, 3.0);
        let v2 = Point2::new(-4.0, 1.0);
        let v1_support_direction = Vec2::new(1.0, 3.0);

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(5.0, -2.0)),
            TriangleVoronoiRegion::EdgeV0V1
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(3.0, 5.0)),
            TriangleVoronoiRegion::EdgeV0V1
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(2.0, 6.0)),
            TriangleVoronoiRegion::EdgeV1V2
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(-3.0, 2.0)),
            TriangleVoronoiRegion::EdgeV1V2
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(2.0, -6.0)),
            TriangleVoronoiRegion::EdgeV0V1
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(-6.0, -1.0)),
            TriangleVoronoiRegion::EdgeV1V2
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(-1.0, 1.0)),
            TriangleVoronoiRegion::Interior
        );

        // Degenerate cases
        let v0 = Point2::new(1.0, 3.0);
        let v1 = Point2::new(1.0, 3.0);
        let v2 = Point2::new(4.0, 3.0);
        let v1_support_direction = Vec2::new(0.0, -1.0);

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(0.0, 5.0)),
            TriangleVoronoiRegion::EdgeV0V1
        );

        std::assert_eq!(    
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(2.0, -1.0)),
            TriangleVoronoiRegion::EdgeV1V2
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(-2.0, 5.0)),
            TriangleVoronoiRegion::EdgeV0V1
        );

        std::assert_eq!(
            triangle_voronoi_region(v0, v1, v2, v1_support_direction, Point2::new(6.0, 6.0)),
            TriangleVoronoiRegion::EdgeV1V2
        );
    }
}