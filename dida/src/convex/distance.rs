use crate::{Point2, Vec2};

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
    let in_front_of_v0v1 = Vec2::cross(point - v1, v1 - v0) >= 0.0;
    let in_front_of_v1v2 = Vec2::cross(point - v1, v2 - v1) >= 0.0;

    if in_front_of_v0v1 && in_front_of_v1v2 {
        if Vec2::cross(point - v1, v1_support_direction) > 0.0 {
            TriangleVoronoiRegion::EdgeV0V1
        } else {
            TriangleVoronoiRegion::EdgeV1V2
        }
    } else if in_front_of_v0v1 {
        TriangleVoronoiRegion::EdgeV0V1
    } else if in_front_of_v1v2 {
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
    }
}