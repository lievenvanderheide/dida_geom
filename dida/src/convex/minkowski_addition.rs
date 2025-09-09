use crate::{Vec2, Point2, ConvexPolygon, ConvexPolygonView};
use crate::cyclic::succ_modulo;

fn minkowski_addition<const IS_DIFFERENCE: bool>(a: ConvexPolygonView, b: ConvexPolygonView) -> ConvexPolygon<Vec<Point2>> {
    let a_first_index = a.leftmost_vertex_index();
    let b_first_index = if IS_DIFFERENCE {
        b.rightmost_vertex_index()
    } else {
        b.leftmost_vertex_index()
    };

    let mut a_index = a_first_index;
    let mut a_next_index = succ_modulo(a_index, a.num_vertices());
    let mut a_outgoing = a[a_next_index] - a[a_index];

    let mut b_index = b_first_index;
    let mut b_next_index = succ_modulo(b_index, b.num_vertices());
    let mut b_outgoing = b[b_next_index] - b[b_index];
    
    let mut result = Vec::<Point2>::with_capacity(a.num_vertices() + b.num_vertices());
    let mut num_merged = 0;
    while num_merged != a.num_vertices() + b.num_vertices() {
        result.push(
            if IS_DIFFERENCE {
                Point2::from_vec2(a[a_index] - b[b_index])
            } else {
                a[a_index] + b[b_index].to_vec2()
            }
        );

        let mut sign = Vec2::cross(a_outgoing, b_outgoing);
        if IS_DIFFERENCE {
            sign = -sign;
        }

        if sign >= 0.0 {
            a_index = a_next_index;
            a_next_index = succ_modulo(a_index, a.num_vertices());
            a_outgoing = a[a_next_index] - a[a_index];

            num_merged += 1;
        }

        if sign <= 0.0 {
            b_index = b_next_index;
            b_next_index = succ_modulo(b_index, b.num_vertices());
            b_outgoing = b[b_next_index] - b[b_index];
        
            num_merged += 1;
        }
    }

    ConvexPolygon::new_unchecked(result)
}

pub fn minkowski_sum(a: ConvexPolygonView, b: ConvexPolygonView) -> ConvexPolygon<Vec<Point2>> {
    minkowski_addition::<false>(a, b)
}

pub fn minkowski_difference(a: ConvexPolygonView, b: ConvexPolygonView) -> ConvexPolygon<Vec<Point2>> {
    minkowski_addition::<true>(a, b)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::str::FromStr;

    #[test]
    fn test_minkowski_sum() {
        fn test(
            mut a: ConvexPolygon<Vec<Point2>>,
            mut b: ConvexPolygon<Vec<Point2>>,
            expected_result: ConvexPolygon<Vec<Point2>>
        ) {
            for _ in 0..a.num_vertices() {
                for _ in 0..b.num_vertices() {
                    std::assert_eq!(minkowski_sum(a.as_view(), b.as_view()), expected_result);

                    b.rotate_vertex_list_right(1);
                }

                a.rotate_vertex_list_right(1);
            }
        }

        // General case
        test(
            ConvexPolygon::from_str("{{2, 3}, {5, 1}, {9, 2}, {3, 5}}").unwrap(),
            ConvexPolygon::from_str("{{-2, -1}, {-1, -3}, {2, 1}, {-1, 2}}").unwrap(),
            ConvexPolygon::from_str("{{0, 2}, {1, 0}, {4, -2}, {8, -1}, {11, 3}, {5, 6}, {2, 7}, {1, 5}}").unwrap()
        );

        // With parallel edges
        test(
            ConvexPolygon::from_str("{{1, 1}, {5, 1}, {5, 3}, {1, 3}}").unwrap(),
            ConvexPolygon::from_str("{{-4, -2}, {-1, -2}, {-1, -1}, {-4, -1}}").unwrap(),
            ConvexPolygon::from_str("{{-3, -1}, {4, -1}, {4, 2}, {-3, 2}}").unwrap(),
        );
    }
}
