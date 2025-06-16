use crate::{Vec2, Point2};
use crate::convex::convex_polygon::ConvexPolygonView;
use crate::convex::convex_arc_bisector::ConvexArcBisector;
use crate::cyclic::{sub_modulo};

/// Returns whether the two convex polygons intersect.
pub fn intersect(a: ConvexPolygonView, b: ConvexPolygonView) -> bool {
    let a_rightmost_idx = a.rightmost_vertex_index();
    let b_leftmost_idx = b.leftmost_vertex_index();
    if a[a_rightmost_idx].x() <= b[b_leftmost_idx].x() {
        return false;
    }

    let a_leftmost_idx = a.leftmost_vertex_index();
    let b_rightmost_idx = b.rightmost_vertex_index();
    if a[a_leftmost_idx].x() >= b[b_rightmost_idx].x() {
        return false;
    }

    let a_is_lower = if a[a_leftmost_idx].x() < b[b_leftmost_idx].x() {
        Vec2::cross(a[a_rightmost_idx] - a[a_leftmost_idx], b[b_leftmost_idx] - a[a_leftmost_idx]) > 0.0
    } else {
        Vec2::cross(b[b_rightmost_idx] - b[b_leftmost_idx], a[a_leftmost_idx] - b[b_leftmost_idx]) < 0.0
    };

    if a_is_lower {
        arc_regions_intersect(
            ConvexArcBisector::new(a, a_rightmost_idx, sub_modulo(a_leftmost_idx, a_rightmost_idx, a.num_vertices())),
            ConvexArcBisector::new(b, b_leftmost_idx, sub_modulo(b_rightmost_idx, b_leftmost_idx, b.num_vertices()))
        )
    } else {
        arc_regions_intersect(
            ConvexArcBisector::new(b, b_rightmost_idx, sub_modulo(b_leftmost_idx, b_rightmost_idx, b.num_vertices())),
            ConvexArcBisector::new(a, a_leftmost_idx, sub_modulo(a_rightmost_idx, a_leftmost_idx, a.num_vertices()))
        )
    }
}

/// Returns whether two arc regions intersect.
///
/// The lower region is the region bounded by the edges in `lower_region_arc_bisector` and the rays extending downward
/// from the first and last vertices of this range.
///
/// Similarly, the upper region is the region bounded by the edges in `upper_region_arc_bisector` and the rays extending
/// upward from the first and last vertices of this range.
fn arc_regions_intersect(
    mut lower_region_arc_bisector: ConvexArcBisector,
    mut upper_region_arc_bisector: ConvexArcBisector,
) -> bool {
    lower_region_arc_bisector.bisect();
    upper_region_arc_bisector.bisect();

    while lower_region_arc_bisector.num_edges() > 1 && upper_region_arc_bisector.num_edges() > 1 {
        let lower_edge_left = lower_region_arc_bisector.mid_vertex();
        let lower_edge_dir = lower_region_arc_bisector.mid_vertex_outgoing_rev();

        let upper_edge_left = upper_region_arc_bisector.mid_vertex();
        let upper_edge_dir = upper_region_arc_bisector.mid_vertex_outgoing();
    
        let lower_edge_left_outside_upper = Vec2::cross(upper_edge_dir, lower_edge_left - upper_edge_left) < 0.0;
        let upper_edge_left_outside_lower = Vec2::cross(lower_edge_dir, upper_edge_left - lower_edge_left) > 0.0;
    
        if !lower_edge_left_outside_upper && !upper_edge_left_outside_lower {
            if lower_edge_left.x() < upper_edge_left.x() {
                lower_region_arc_bisector.move_cw();
                lower_region_arc_bisector.bisect();
                upper_region_arc_bisector.move_cw();
                upper_region_arc_bisector.bisect();
            } else {
                lower_region_arc_bisector.move_ccw();
                lower_region_arc_bisector.bisect();
                upper_region_arc_bisector.move_ccw();
                upper_region_arc_bisector.bisect();
            }
        } else {
            let overlap_on_right = Vec2::cross(lower_edge_dir, upper_edge_dir) < 0.0;
            
            if lower_edge_left_outside_upper {
                if overlap_on_right {
                    lower_region_arc_bisector.move_cw();
                    lower_region_arc_bisector.bisect();
                } else {
                    lower_region_arc_bisector.move_ccw();
                    lower_region_arc_bisector.bisect();
                }
            }

            if upper_edge_left_outside_lower {
                if overlap_on_right {
                    upper_region_arc_bisector.move_ccw();
                    upper_region_arc_bisector.bisect();
                } else {
                    upper_region_arc_bisector.move_cw();
                    upper_region_arc_bisector.bisect();
                }
            }
        }
    }

    if lower_region_arc_bisector.num_edges() == 1 && upper_region_arc_bisector.num_edges() == 1 {
        edge_regions_intersect(
            lower_region_arc_bisector.end_vertex(),
            lower_region_arc_bisector.begin_vertex(),
            upper_region_arc_bisector.begin_vertex(),
            upper_region_arc_bisector.end_vertex()
        )
    } else if lower_region_arc_bisector.num_edges() == 1{
        edge_arc_regions_intersect(
            lower_region_arc_bisector.end_vertex(),
            lower_region_arc_bisector.begin_vertex(),
            upper_region_arc_bisector
        )
    } else {
        std::assert_eq!(upper_region_arc_bisector.num_edges(), 1);
        arc_edge_regions_intersect(
            lower_region_arc_bisector,
            upper_region_arc_bisector.begin_vertex(),
            upper_region_arc_bisector.end_vertex()
        )
    }
}

/// Returns whether a lower arc region and an upper edge region intersect.
///
/// The lower region is the region bounded by the edges in `lower_region_arc_bisector` and the rays extending downward
/// from the first and last vertices of this range.
///
/// The upper region is the region bounded by the edge between 'upper_edge_left', 'upper_edge_right' and the rays
/// extending upward from these two vertices.
fn arc_edge_regions_intersect(
    mut lower_region_arc_bisector: ConvexArcBisector,
    upper_edge_left: Point2,
    upper_edge_right: Point2
) -> bool {
    let upper_edge_dir = upper_edge_right - upper_edge_left;

    while lower_region_arc_bisector.num_edges() > 1 {
        let lower_edge_left = lower_region_arc_bisector.mid_vertex();
        let lower_edge_dir = lower_region_arc_bisector.mid_vertex_outgoing_rev();

        if lower_edge_left.x() < upper_edge_left.x() ||
            (lower_edge_left.x() < upper_edge_right.x() && Vec2::cross(lower_edge_dir, upper_edge_dir) < 0.0)
        {
            lower_region_arc_bisector.move_cw();
        } else {
            lower_region_arc_bisector.move_ccw();
        }

        lower_region_arc_bisector.bisect();
    }

    edge_regions_intersect(
        lower_region_arc_bisector.end_vertex(),
        lower_region_arc_bisector.begin_vertex(),
        upper_edge_left,
        upper_edge_right
    )
}

/// Returns whether a lower edge region and an upper arc region intersect.
///
/// The lower region is the region bounded by the edge between 'lower_edge_left', 'lower_edge_right' and the rays
/// extending downward from these two vertices.
///
/// The upper region is the region bounded by the edges in `upper_region_arc_bisector` and the rays extending upward
/// from the first and last vertices of this range.
fn edge_arc_regions_intersect(
    lower_edge_left: Point2,
    lower_edge_right: Point2,
    mut upper_region_arc_bisector: ConvexArcBisector
) -> bool {
    let lower_edge_dir = lower_edge_right - lower_edge_left;

    while upper_region_arc_bisector.num_edges() > 1 {
        let upper_edge_left = upper_region_arc_bisector.mid_vertex();
        let upper_edge_dir = upper_region_arc_bisector.mid_vertex_outgoing();

        if upper_edge_left.x() < lower_edge_left.x() ||
            (upper_edge_left.x() < lower_edge_right.x() && Vec2::cross(lower_edge_dir, upper_edge_dir) < 0.0)
        {
            upper_region_arc_bisector.move_ccw();
        } else {
            upper_region_arc_bisector.move_cw();
        }

        upper_region_arc_bisector.bisect();
    }

    edge_regions_intersect(
        lower_edge_left,
        lower_edge_right,
        upper_region_arc_bisector.begin_vertex(),
        upper_region_arc_bisector.end_vertex()
    )
}

/// Returns whether a lower and an upper edge region intersect.
///
/// The lower region is the region bounded by the edge between 'lower_edge_left', 'lower_edge_right' and the rays
/// extending downward from these two vertices.
///
/// The upper region is the region bounded by the edge between 'upper_edge_left', 'upper_edge_right' and the rays
/// extending upward from these two vertices. 
fn edge_regions_intersect(
    lower_edge_left: Point2,
    lower_edge_right: Point2,
    upper_edge_left: Point2,
    upper_edge_right: Point2
) -> bool {
    std::debug_assert!(lower_edge_left.x() < lower_edge_right.x());
    std::debug_assert!(upper_edge_left.x() < upper_edge_right.x());

    if lower_edge_right.x() < upper_edge_left.x() || upper_edge_right.x() < lower_edge_left.x() {
        return false;
    }

    // The x-ranges overlap

    let lower_edge_dir = lower_edge_right - lower_edge_left;
    let upper_edge_dir = upper_edge_right - upper_edge_left;

    if Vec2::cross(lower_edge_dir, upper_edge_dir) > 0.0 {
        if lower_edge_left.x() < upper_edge_left.x() {
            Vec2::cross(lower_edge_dir, upper_edge_left - lower_edge_left) < 0.0
        } else {
            Vec2::cross(upper_edge_dir, lower_edge_left - upper_edge_left) > 0.0
        }
    } else {
        if lower_edge_right.x() > upper_edge_right.x() {
            Vec2::cross(lower_edge_dir, upper_edge_right - lower_edge_left) < 0.0
        } else {
            Vec2::cross(upper_edge_dir, lower_edge_right - upper_edge_left) > 0.0
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::convex::convex_polygon::ConvexPolygon;
    use crate::cyclic::succ_modulo;
    use std::str::FromStr;

    #[test]
    fn test_intersect() {
        fn call_intersect(a_str: &str, b_str: &str, expected_result: bool) {
            let mut a = ConvexPolygon::from_str(a_str).unwrap();
            let mut b = ConvexPolygon::from_str(b_str).unwrap();

            for _ in 0..a.num_vertices() {
                for _ in 0..b.num_vertices() {
                    a.rotate_vertex_list_right(1);

                    std::assert_eq!(intersect(a.as_view(), b.as_view()), expected_result);
                    std::assert_eq!(intersect(b.as_view(), a.as_view()), expected_result);
                }

                b.rotate_vertex_list_right(1);
            }
        }

        // Intersecting, lower left of upper
        call_intersect(
            "{{-2.14, -0.04}, {1.16, -2.40}, {7.38, -1.44}, {10.12, 1.66}, {6.24, 3.94}, {1.34, 3.76}}",
            "{{4.12, 1.16}, {8.90, 4.34}, {7.36, 7.12}, {0.28, 7.60}, {-1.20, 5.80}}",
            true
        );

        // Intesecting, upper left of lower
        call_intersect(
            "{{-2.52, -0.66}, {1.64, -5.34}, {9.24, -1.32}, {3.02, 4.46}}",
            "{{-5.28, 6.40}, {0.20, 0.74}, {5.68, 5.84}, {1.62, 8.94}}",
            true
        );

        // Horizontal ranges overlap, lower left of upper, not intersecting
        call_intersect(
            "{{-0.40, -3.90}, {3.90, 2.80}, {2.56, 4.50}, {0.66, 5.50}, {-1.24, 5.12}, {-2.72, 4.16}}",
            "{{2.22, 6.98}, {14.02, 5.62}, {8.98, 8.26}}",
            false
        );

        // Horizontal ranges overlap, upper left of lower, not intersecting
        call_intersect(
            "{{-4.50, 4.20}, {-0.84, 2.36}, {4.88, 2.24}, {7.24, 3.34}, {3.90, 5.16}, {-1.22, 5.52}}",
            "{{-0.64, -0.02}, {2.86, -2.74}, {8.42, -3.02}, {11.82, -1.20}, {7.80, 1.40}, {3.06, 1.36}}",
            false
        );

        // Horizontally disjoint
        call_intersect(
            "{{4.88, 4.46}, {0.60, 6.08}, {-4.50, 4.68}, {-6.80, 1.30}, {-3.34, -1.28}, {1.56, 0.82}}",
            "{{13.32, 6.20}, {8.12, 3.88}, {12.16, 0.64}, {15.12, 1.02}, {17.04, 2.92}, {15.84, 5.60}}",
            false
        );
    }

    #[test]
    fn test_arc_regions_intersect() {
        fn call_arc_regions_intersect(
            lower_polygon_str: &str,
            upper_polygon_str: &str
        ) -> bool {
            let lower_polygon = ConvexPolygon::from_str(lower_polygon_str).unwrap();
            let upper_polygon = ConvexPolygon::from_str(upper_polygon_str).unwrap();

            let lower_arc_begin_index = lower_polygon.rightmost_vertex_index();
            let lower_arc_num_edges = sub_modulo(
                lower_polygon.leftmost_vertex_index(),
                lower_arc_begin_index,
                lower_polygon.num_vertices()
            );

            let upper_arc_begin_index = upper_polygon.leftmost_vertex_index();
            let upper_arc_num_edges = sub_modulo(
                upper_polygon.rightmost_vertex_index(),
                upper_arc_begin_index,
                upper_polygon.num_vertices()
            );

            arc_regions_intersect(
                ConvexArcBisector::new(
                    lower_polygon.as_view(),
                    lower_arc_begin_index,
                    lower_arc_num_edges
                ),
                ConvexArcBisector::new(
                    upper_polygon.as_view(),
                    upper_arc_begin_index,
                    upper_arc_num_edges
                )
            )
        }

        // First iteration with both mids outside other halfplane, converging towards left, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{-1.89, -0.99}, {7.50, -2.46}, {6.43, -0.28}, {2.96, 1.81}, {-0.02, 0.76}}",
            "{{-2.72, 6.60}, {-0.90, 4.10}, {2.74, 3.19}, {6.48, 4.44}, {7.81, 6.93}}"
        ));

        // First iteration with both mids outside other halfplane, converging towards left, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{-3.39, 0.87}, {6.22, -1.10}, {5.01, 1.36}, {1.90, 3.44}, {-1.81, 4.15}}",
            "{{-3.28, 7.83}, {-0.80, 3.28}, {3.36, 4.68}, {5.42, 6.66}, {6.30, 9.20}}"
        ));

        // First iteration with both mids outside other halfplane, converging towards right, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{-2.94, -2.19}, {7.87, 3.56}, {6.34, 7.97}, {1.28, 7.01}, {-1.10, 2.71}}",
            "{{-5.97, 11.62}, {-4.02, 8.82}, {-0.69, 8.08}, {3.15, 12.58}}"
        ));

        // First iteration with both mids outside other halfplane, converging towards right, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{-2.19, -2.84}, {6.32, 5.52}, {3.15, 5.14}, {-0.30, 3.08}, {-1.65, 0.31}}",
            "{{-2.43, 11.09}, {-1.87, 9.32}, {-1.02, 7.92}, {1.09, 6.26}, {6.14, 4.52}}"
        ));

        // First iteration with both mids within other halfplane, lower mid left of upper mid, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{-3.47, -0.06}, {3.66, -1.36}, {2.38, 5.81}, {0.06, 6.61}, {-2.02, 3.33}}",
            "{{1.98, 11.52}, {2.35, 7.88}, {3.34, 3.89}, {5.06, 3.67}, {8.67, 11.09}}"
        ));

        // First iteration with both mids within other halfplane, lower mid left of upper mid, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{-3.39, -1.79}, {5.74, -1.61}, {-0.14, 4.80}, {-2.11, 4.92}, {-3.20, 1.54}}",
            "{{1.20, 8.34}, {1.70, 2.32}, {3.57, 1.60}, {6.69, 1.59}, {7.84, 8.39}}"
        ));

        // First iteration with both mids within other halfplane, lower mid right of upper mid, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{2.32, -1.95}, {9.23, 5.19}, {4.59, 5.12}}",
            "{{-3.78, 8.13}, {-2.30, 4.85}, {0.14, 2.47}, {2.22, 2.72}, {5.14, 6.82}}"
        ));

        // First iteration with both mids within other halfplane, lower mid right of upper mid, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{-2.08, -1.53}, {7.26, -1.31}, {2.70, 5.24}, {0.64, 5.17}, {-1.09, 2.42}}",
            "{{0.58, 8.26}, {2.78, 2.00}, {6.94, 3.62}}"
        ));

        // Lower edge slab - upper arc, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{1.42, -2.46}, {4.77, 3.51}, {-0.26, 3.73}}",
            "{{-2.56, 8.52}, {-1.87, 6.96}, {-0.66, 5.33}, {1.17, 4.42}, {4.37, 4.21}, {6.90, 5.52}, {7.79, 7.70}}"
        ));

        // Lower edge slab - upper arc, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{1.42, -2.46}, {4.77, 3.51}, {-0.26, 3.73}}",
            "{{-2.56, 7.48}, {-0.21, 4.56}, {2.67, 3.17}, {5.97, 4.26}}"
        ));
        
        // Lower arc - upper edge slab, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{-2.22, -1.60}, {8.72, -2.32}, {8.43, 0.05}, {6.86, 3.27}, {2.91, 4.08}, {-0.48, 2.58}}",
            "{{1.02, 7.91}, {0.11, 3.11}, {2.11, 3.96}}"
        ));

        // Lower arc - upper edge slab, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{-2.22, -1.60}, {8.72, -2.32}, {8.43, 0.05}, {6.86, 3.27}, {2.91, 4.08}, {-0.48, 2.58}}",
            "{{6.80, 7.75}, {4.35, 4.05}, {7.78, 1.97}}"
        ));

        // Edge slabs, not intersecting.
        std::assert!(!call_arc_regions_intersect(
            "{{-5.01, 3.94}, {-3.54, -3.87}, {-2.22, 3.24}}",
            "{{-1.33, 9.20}, {-1.74, 1.54}, {-0.29, 1.44}}"
        ));

        // Edge slabs, intersecting.
        std::assert!(call_arc_regions_intersect(
            "{{1.49, -1.16}, {3.28, 4.10}, {0.35, 3.60}}",
            "{{1.54, 7.65}, {1.28, 3.24}, {2.06, 3.28}}"
        ));
    }

    #[test]
    fn test_arc_edge_regions_intersect() {
        fn test_with_upper_edge(upper_edge_left: Point2, upper_edge_right: Point2, expected_result: bool) {
            let mut polygon = ConvexPolygon::from_str(
                "{{11.32, 4.24}, {9.64, 7.22}, {6.17, 8.57}, {2.61, 7.61}, {0.84, 4.66}}"
            ).unwrap();

            let mut lower_range_begin_index = 0;
            let lower_range_num_edges = 4;

            for _ in 0..polygon.num_vertices() {
                std::assert_eq!(
                    arc_edge_regions_intersect(
                        ConvexArcBisector::new(polygon.as_view(), lower_range_begin_index, lower_range_num_edges),
                        upper_edge_left,
                        upper_edge_right),
                    expected_result
                );

                polygon.rotate_vertex_list_right(1);
                lower_range_begin_index = succ_modulo(lower_range_begin_index, polygon.num_vertices());
            }
        }

        // First mid left of edge region, no overlap.
        test_with_upper_edge(Point2::new(7.20, 8.92), Point2::new(8.44, 7.99), false);
        
        // First mid left of edge region, overlap.
        test_with_upper_edge(Point2::new(8.50, 11.99), Point2::new(10.78, 4.91), true);

        // First mid right of edge region, no overlap.
        test_with_upper_edge(Point2::new(3.66, 8.78), Point2::new(4.87, 11.97), false);
        
        // First mid right of edge region, overlap.
        test_with_upper_edge(Point2::new(3.17, 9.20), Point2::new(5.50, 7.24), true);

        // First mid in edge region x-range, converging towards left, no overlap.
        test_with_upper_edge(Point2::new(5.36, 9.62), Point2::new(8.20, 10.62), false);

        // First mid in edge region x-range, converging towards left, overlap.
        test_with_upper_edge(Point2::new(5.52, 7.22), Point2::new(7.87, 10.41), true);

        // First mid in edge region x-range, converging towards right, no overlap.
        test_with_upper_edge(Point2::new(4.75, 11.20), Point2::new(10.55, 7.17), false);
        
        // First mid in edge region x-range, converging towards right, overlap.
        test_with_upper_edge(Point2::new(5.29, 10.57), Point2::new(9.93, 5.45), true);
    }

    #[test]
    fn test_edge_arc_regions_intersect() {
        fn test_with_slab(lower_edge_left: Point2, lower_edge_right: Point2, expected_result: bool) {
            let mut upper_polygon = ConvexPolygon::from_str(
                "{{4.28, 8.19}, {5.78, 5.07}, {10.54, 3.45}, {14.86, 5.31}, {16.82, 9.07}}"
            ).unwrap();

            let mut upper_range_begin_index = 0;
            let upper_range_num_edges = 4;

            for _ in 0..upper_polygon.num_vertices() {
                std::assert_eq!(
                    edge_arc_regions_intersect(
                        lower_edge_left,
                        lower_edge_right,
                        ConvexArcBisector::new(
                            upper_polygon.as_view(),
                            upper_range_begin_index,
                            upper_range_num_edges
                        )
                    ),
                    expected_result
                );

                upper_polygon.rotate_vertex_list_right(1);
                upper_range_begin_index = succ_modulo(upper_range_begin_index, upper_polygon.num_vertices());
            }
        }

        // First mid left of slab, no overlap.
        test_with_slab(Point2::new(11.6, 2.99), Point2::new(13.86, 3.87), false);
        
        // First mid left of slab, overlap.
        test_with_slab(Point2::new(11.94, 3.41), Point2::new(16.2, 7.31), true);

        // First mid right of slab, no overlap.
        test_with_slab(Point2::new(5.78, 2.57), Point2::new(8.92, 3.57), false);
        
        // First mid right of slab, overlap.
        test_with_slab(Point2::new(6.68, 6.51), Point2::new(9.16, 4.41), true);

        // First mid in slab x-range, converging towards left, no overlap.
        test_with_slab(Point2::new(8.84, 2.75), Point2::new(12.36, 2.73), false);

        // First mid in slab x-range, converging towards left, overlap.
        test_with_slab(Point2::new(5.52, 7.22), Point2::new(7.87, 10.41), true);

        // First mid in slab x-range, converging towards right, no overlap.
        test_with_slab(Point2::new(8.58, 1.09), Point2::new(13.6, 4.51), false);
        
        // First mid in slab x-range, converging towards right, no overlap.
        test_with_slab(Point2::new(8.58, 1.09), Point2::new(13.64, 5.77), true);
    }

    #[test]
    fn test_edge_regions_intersect() {
        // Lower fully left of upper
        std::assert!(!edge_regions_intersect(
            Point2::new(-1.22, 2.15),
            Point2::new(2.16, 1.05),
            Point2::new(3.22, 5.11),
            Point2::new(5.36, 5.49),
        ));

        // Upper fully left of lower
        std::assert!(!edge_regions_intersect(
            Point2::new(-1.2, -0.49),
            Point2::new(2.16, 1.05),
            Point2::new(-5.6, 4.57),
            Point2::new(-2.04, 4.25),
        ));

        // Overlap towards left, edge of lower, no intersection.
        std::assert!(!edge_regions_intersect(
            Point2::new(-4.8, -4.25),
            Point2::new(4.02, -1.77),
            Point2::new(-2.58, -1.55),
            Point2::new(1.74, 2.33),
        ));

        // Overlap towards left, edge of lower, intersection.
        std::assert!(edge_regions_intersect(
            Point2::new(3.12,-1.8),
            Point2::new(14.04,-4.26),
            Point2::new(4.86,-4.38),
            Point2::new(12.48,0.74),
        ));
         
        // Overlap towards left, edge of upper, no intersection.
        std::assert!(!edge_regions_intersect(
            Point2::new(10.08, 0.74),
            Point2::new(18.08, -4.82),
            Point2::new(5.6, 1.64),
            Point2::new(15.64, 4.22),
        ));

        // Overlap towards left, edge of upper, intersection.
        std::assert!(edge_regions_intersect(
            Point2::new(8.12, 2.8),
            Point2::new(12.38, -3.36),
            Point2::new(4.94, 1.42),
            Point2::new(13.78, 1.82),
        ));
        
        // Overlap towards right, edge of lower, no intersection.
        std::assert!(!edge_regions_intersect(
            Point2::new(-3.72, -3.07),
            Point2::new(5.24, 0.73),
            Point2::new(-2.5, 2.27),
            Point2::new(2.68, 2.81)
        ));

        // Overlap towards right, edge of lower, intersection.
        std::assert!(edge_regions_intersect(
            Point2::new(-1.52, -2.71),
            Point2::new(5.82, 1.17),
            Point2::new(-3.78, 3.23),
            Point2::new(3.76, -1.65)
        ));

        // Overlap towards right, edge of upper, no intersection.
        std::assert!(!edge_regions_intersect(
            Point2::new(2.08, 3.16),
            Point2::new(8.84, 6.6),
            Point2::new(5.48, 10.26),
            Point2::new(13.98, 5.68)
        ));

        // Overlap towards right, edge of upper, intersection.
        std::assert!(edge_regions_intersect(
            Point2::new(6.16, 4.56),
            Point2::new(11.46, 8.96),
            Point2::new(4.48, 9.36),
            Point2::new(14.42, 4.2)
        ));

        // TODO: Corner cases
    }
}
