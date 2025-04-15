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
            &mut [
                Point2::new(-7.56, 0.96),
                Point2::new(-7.2, -1.9),
                Point2::new(-5.6, -4.38),
                Point2::new(-2.62, -5.32),
                Point2::new(4.56, -2.38),
                Point2::new(4.5, 1.46),
                Point2::new(3.2, 4.2),
                Point2::new(-0.58, 5.38),
                Point2::new(-4.8, 3.94),
            ],
            true
        );

        // Triangle
        test(
            &mut [
                Point2::new(0.98, -3.32),
                Point2::new(6.2, -3.4),
                Point2::new(8.58, 2.92),
            ],  
            true
        );

        // Too few vertices
        test(
            &mut [
                Point2::new(-0.24, -2.18),
                Point2::new(9.86, 1.2),
            ],  
            false
        );

        // Duplicated vertices
        test(
            &mut [
                Point2::new(6.86, -7.22),
                Point2::new(6.86, -7.22),
                Point2::new(12.38, -4.54),
                Point2::new(10.46, 2.84),
                Point2::new(4.96, -5.34),
                Point2::new(5.42, -6.64),
            ],
            false
        );

        // Non convex
        test(
            &mut [
                Point2::new(6.58, -2.26),
                Point2::new(5.56, -3.7),
                Point2::new(12.68, -1.16),
                Point2::new(8.98, 3.38),
                Point2::new(0.28, -2.94)
            ],
            false
        );

        // Wrong winding
        test(
            &mut [
                Point2::new(1.4, -1.6),
                Point2::new(4.72, 1.16),
                Point2::new(13.48, -0.34),
                Point2::new(11.52, -7.22),
                Point2::new(5.9, -5.62),
            ],
            false
        );

        // Winds around twice
        test(
            &mut[
                Point2::new(-0.9, -2.86),
                Point2::new(-0.08, -6.14),
                Point2::new(6.22, -7.14),
                Point2::new(10.6, -0.28),
                Point2::new(1.92, 0.46),
                Point2::new(-1.9, -4.84),
                Point2::new(3.22, -10.62),
                Point2::new(11.14, -7.12),
                Point2::new(5.24, 1.62),
            ],
            false
        );

        // All vertices on vertical line
        test(
            &mut [
                Point2::new(3.0, 1.0),
                Point2::new(3.0, -3.0),
                Point2::new(3.0, 6.0),
                Point2::new(3.0, 4.0),
            ],
            false
        );
    }
}