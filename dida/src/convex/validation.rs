use crate::{Vec2, Point2};

/// Returns whether 'vertices' form a valid convex polygon.
///
/// The vertices are considered to form a valid convex polygon if the following conditions are satisfied:
///
///  1. There are at least 3 vertices.
///  2. All vertices are distinct.
///  3. Each corner is strictly convex, the interior angle at any vertex is less than 180 degrees.
///  4. The polygon winds around its interior exactly once in the counter clockwise direction.
///
pub fn are_valid_convex_polygon_vertices(vertices: &[Point2]) -> bool {
    if vertices.len() < 3 {
        return false;
    }

    let mut left_vertex_found = false;
    let mut right_vertex_found = false;

    let mut cur_vertex = vertices[vertices.len() - 1];
    let mut incoming = cur_vertex - vertices[vertices.len() - 2];
    for &next_vertex in vertices {
        let outgoing = next_vertex - cur_vertex;

        if Vec2::cross(incoming, outgoing) <= 0.0 {
            return false;
        }

        if incoming.x() <= 0.0 && outgoing.x() > 0.0 {
            if left_vertex_found {
                return false;
            }

            left_vertex_found = true;
        }

        if incoming.x() >= 0.0 && outgoing.x() < 0.0 {
            if right_vertex_found {
                return false;
            }

            right_vertex_found = true;
        }

        cur_vertex = next_vertex;
        incoming = outgoing;
    }

    true
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_are_valid_convex_polygon_vertices() {
        fn test(vertices: &mut [Point2], expected_result: bool) {
            for _ in 0..vertices.len() {
                std::assert_eq!(are_valid_convex_polygon_vertices(vertices), expected_result);
                vertices.rotate_left(1);
            }
        }

        // Valid polygon
        test(
            Point2::vec_from_str("{
                {-7.56, 0.96}, {-7.2, -1.9}, {-5.6, -4.38}, {-2.62, -5.32}, {4.56, -2.38}, {4.5, 1.46}, {3.2, 4.2},
                {-0.58, 5.38}, {-4.8, 3.94}
            }").unwrap().as_mut(),
            true
        );

        // Triangle
        test(Point2::vec_from_str("{{0.98, -3.32}, {6.2, -3.4}, {8.58, 2.92}}").unwrap().as_mut(), true);

        // Too few vertices
        test(Point2::vec_from_str("{{-0.24, -2.18}, {9.86, 1.2}}").unwrap().as_mut(), false);

        // Duplicated vertices
        test(
            Point2::vec_from_str(
                "{{6.86, -7.22}, {6.86, -7.22}, {12.38, -4.54}, {10.46, 2.84}, {4.96, -5.34}, {5.42, -6.64}}"
            ).unwrap().as_mut(),
            false
        );

        // Non convex
        test(
            Point2::vec_from_str(
                "{{6.58, -2.26}, {5.56, -3.7}, {12.68, -1.16}, {8.98, 3.38}, {0.28, -2.94}}"
            ).unwrap().as_mut(),
            false
        );

        // Wrong winding
        test(
            Point2::vec_from_str(
                "{{1.4, -1.6}, {4.72, 1.16}, {13.48, -0.34}, {11.52, -7.22}, {5.9, -5.62}}"
            ).unwrap().as_mut(),
            false
        );

        // Winds around twice
        test(
            Point2::vec_from_str("{
                {-0.9, -2.86}, {-0.08, -6.14}, {6.22, -7.14}, {10.6, -0.28}, {1.92, 0.46}, {-1.9, -4.84},
                {3.22, -10.62}, {11.14, -7.12}, {5.24, 1.62}
            }").unwrap().as_mut(),
            false
        );

        // All vertices on vertical line
        test(Point2::vec_from_str("{{3.0, 1.0}, {3.0, -3.0}, {3.0, 6.0}, {3.0, 4.0}}").unwrap().as_mut(), false);
    }
}
