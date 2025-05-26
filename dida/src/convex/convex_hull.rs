use crate::{Vec2, Point2};

/// Computes the convex hull of the points in 'points' in place.
///
/// The return value is `n` the number of vertices of the convex hull, while the first `n` elements of `points` will
/// contain the vertices of the convex hull. If `n >= 3`, then the vertices are guaranteed to form a valid convex
/// polygon, but it's possible that the convex hull degenerated into a line segment (2 vertices) or a point (1 vertex).
pub fn convex_hull_inplace(points: &mut [Point2]) -> usize {
    std::assert!(points.len() >= 1);

    fn is_convex(a: Point2, b: Point2, c: Point2) -> bool {
        Vec2::cross(b - a, c - a) > 0.0
    }

    let mut leftmost_idx = 0;
    let mut leftmost = points[0];
    let mut rightmost = points[0];
    for i in 1..points.len() {
        if Point2::lex_less_than(points[i], leftmost) {
            leftmost_idx = i;
            leftmost = points[i];
        }

        if Point2::lex_greater_than(points[i], rightmost) {
            rightmost = points[i];
        }
    }

    points.swap(0, leftmost_idx);

    if leftmost == rightmost {
        return 1;
    }

    let mid_idx = partition(&mut points[1..], |point| Vec2::cross(rightmost - leftmost, *point - leftmost) <= 0.0) + 1;
    points[1..mid_idx].sort_unstable_by(|a, b| Point2::lex_cmp(*a, *b));
    points[mid_idx..].sort_unstable_by(|a, b| Point2::lex_cmp(*a, *b).reverse());

    let mut write_idx = 2;
    for read_idx in 2..points.len() {
        while write_idx > 1 && !is_convex(points[write_idx - 2], points[write_idx - 1], points[read_idx]) {
            write_idx -= 1;
        }

        points[write_idx] = points[read_idx];
        write_idx += 1;
    }

    while write_idx > 2 && !is_convex(points[write_idx - 2], points[write_idx - 1], points[0]) {
        write_idx -= 1;
    }

    write_idx
}

fn partition<T>(slice: &mut [T], pred: impl Fn(&T) -> bool) -> usize {
    let mut it = slice.iter_mut();
    let mut mid_idx = 0;
    loop {
        // Skip elements on the left for which the predicate is true.
        let left_elem = loop {
            match it.next() {
                None => return mid_idx,
                Some(elem) if pred(elem) => mid_idx += 1,
                Some(elem) => break elem,
            }
        };

        // Skip elements on the right for which the predicate is true.
        let right_elem = loop {
            match it.next_back() {
                None => return mid_idx,
                Some(elem) if pred(elem) => break elem,
                _ => {},
            }
        };

        // The predicate doesn't hold for left_elem, and does hold for right_elem, so we can swap the two elements.
        std::mem::swap(left_elem, right_elem);
        mid_idx += 1;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;
    use crate::convex::validation::are_valid_convex_polygon_vertices;

    fn point_in_convex_polygon(point: Point2, polygon: &[Point2]) -> bool {
        let mut v0 = polygon[polygon.len() - 1];
        for v1 in polygon {
            if Vec2::cross(*v1 - v0, point - v0) < 0.0 {
                return false;
            }

            v0 = *v1;
        }

        true
    }

    #[test]
    fn test_convex_hull_inplace() {
        fn test_general_case(mut points: Vec<Point2>) {
            let points_set = HashSet::<Point2>::from_iter(points.iter().cloned());
            
            let num_vertices = convex_hull_inplace(&mut points);
            std::assert!(points.len() >= 3);
            let vertices = &points[..num_vertices];
            
            std::assert!(are_valid_convex_polygon_vertices(&vertices));

            // All vertices of the resulting convex hull must be in the input point set.
            for vertex in vertices {
                std::assert!(points_set.contains(vertex));
            }

            // All points in the input set must fall inside the convex hull
            for point in points_set {
                std::assert!(point_in_convex_polygon(point, vertices));
            }

            // points[0] should be the leftmost vertex.
            for vertex in &vertices[1..] {
                std::assert!(Point2::lex_less_than(vertices[0], *vertex));
            }
        }

        // General case
        test_general_case(
            Point2::vec_from_str("{
                {-8.56, 6.07}, {8.4, -0.81}, {9.92, 7.23}, {-3.8, 0.15}, {1.16, -6.47}, {3.78, 4.45}, {-10.26, -4.61},
                {-13.54, 0.81}, {-3.14, 9.95}, {13.88, -1.39}, {-2.36, -2.73}
            }").unwrap()
        );

        // Requires pop by first vertex
        test_general_case(
            Point2::vec_from_str("{
                {1.54, 0.48}, {3.08, -2.2}, {8.74, -2.84}, {11.68, -0.36}, {9.44, 4.02}, {5.3, 4.38}, {4.04, 1.38}
            }").unwrap()
        );

        // Points on edge
        test_general_case(
            Point2::vec_from_str("{{3, 3}, {5, 1}, {7, -1}, {10, 1}, {13, 3}, {10, 5}, {7, 7}, {5, 5}}").unwrap()
        );

        // Duplicated points
        test_general_case(
            Point2::vec_from_str("{
                {2.66, -0.58}, {2.66, -0.58}, {4.54, -4.38}, {4.54, -4.38}, {4.54, -4.38}, {11.1, 1.52}, {11.3, -3.4},
                {11.3, -3.4}, {8.66, -1.34}, {8.66, -1.34}, {8.66, -1.34}, {8.66, -1.34}
            }").unwrap()
        );

        // Degenerates into point
        {
            let mut points = Point2::vec_from_str("{{1, 2}, {1, 2}, {1, 2}}").unwrap();
            let num_vertices = convex_hull_inplace(&mut points);
            std::assert_eq!(num_vertices, 1);
            std::assert_eq!(points[0], Point2::new(1.0, 2.0));
        }

        // Degenerates into line segment
        {
            let mut points = Point2::vec_from_str("{{2, -1}, {4, -5}, {3, -3}}").unwrap();
            let num_vertices = convex_hull_inplace(&mut points);
            std::assert_eq!(num_vertices, 2);
            std::assert_eq!(points[0], Point2::new(2.0, -1.0));
            std::assert_eq!(points[1], Point2::new(4.0, -5.0));
        }
    }

    #[test]
    fn test_partition() {
        fn test(mut elems: Vec<i32>) {
            let mut elems_sorted = elems.clone();
            elems_sorted.sort();

            let mid_idx = partition(&mut elems, |a| *a < 0);
            std::assert!(mid_idx <= elems.len());

            elems[..mid_idx].sort();
            elems[mid_idx..].sort();
            std::assert_eq!(elems, elems_sorted);

            if !elems.is_empty() {
                std::assert!(mid_idx == 0 || elems[mid_idx - 1] < 0);
                std::assert!(mid_idx == elems.len() || elems[mid_idx] >= 0);
            }
        }

        // General case
        test(vec![76, -68, 64, -31, 83, 39, -74, 34, 93, -62]);

        // Empty
        test(vec![]);

        // All negative
        test(vec![-86, -95, -75, -79, -40, -16]);

        // All positive
        test(vec![86, 95, 75, 79, 40, 16]);

        // Already partitioned
        test(vec![-55, -22, -57, -11, -81, 6, 48, 48, 37, 16]);

        // Swap, then all negative
        test(vec![10, -1, -2, -3, -4, -10]);

        // Swap, then all positive
        test(vec![10, 1, 2, 3, 4, -10]);
    }
}
