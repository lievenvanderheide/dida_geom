use crate::{Vec2, Point2};
use crate::convex::convex_polygon::ConvexPolygonView;
use crate::cyclic::{add_modulo, sub_modulo, pred_modulo, succ_modulo};

/// A utility type which facilitates the implementation of binary search style algorithms on the boundary of a
/// convex polygon.
///
/// At any point, the `ConvexArcBisector` will hold a subrange of the edges of the polygon, and if the range contains at
/// least 2 edges, then a mid vertex will be selected. The 'move_left' and 'move_right functions can then be used to
/// repeatedly limit the current range to the part to the left/right of the mid vertex, until only a single edge is
/// left.
pub struct ConvexArcBisector<'a> {
    polygon: ConvexPolygonView<'a>,

    /// The index of the first vertex in the current range.
    begin_index: usize,

    /// The number of edges in the current range.
    num_edges: usize,

    /// The offset from 'begin_index' to the mid index.
    mid_offset: usize,

    /// The index of the mid vertex.
    mid_index: usize,
}

impl<'a> ConvexArcBisector<'a> {
    /// Constructs a new 'ConvexArcBisector'.
    pub fn new(polygon: ConvexPolygonView<'a>, begin_index: usize, num_edges: usize) -> Self {
        std::assert!(begin_index < polygon.num_vertices());
        ConvexArcBisector {
            polygon: polygon,
            begin_index: begin_index,
            num_edges: num_edges,
            mid_offset: 0,
            mid_index: begin_index,
        }
    }

    /// Constructs a ConvexArcBisector whose initial range consists of the full boundary, where the part from
    /// `begin_vertex` to `mid_vertex is the polygon's lower arc and the part from `mid_vertex` to `end_vertex` its
    /// upper arc.
    pub fn new_with_lower_then_upper_arc(polygon: ConvexPolygonView<'a>) -> Self {
        let leftmost_idx = polygon.leftmost_vertex_index();
        let rightmost_idx = polygon.rightmost_vertex_index();
        ConvexArcBisector {
            polygon: polygon,
            begin_index: leftmost_idx,
            num_edges: polygon.num_vertices(),
            mid_offset: sub_modulo(rightmost_idx, leftmost_idx, polygon.num_vertices()),
            mid_index: rightmost_idx,
        }
    }

    /// Constructs a ConvexArcBisector whose initial range consists of the full boundary, where the part from
    /// `begin_vertex` to `mid_vertex is the polygon's upper arc and the part from `mid_vertex` to `end_vertex` its
    /// lower arc.
    pub fn new_with_upper_then_lower_arc(polygon: ConvexPolygonView<'a>) -> Self {
        let leftmost_idx = polygon.leftmost_vertex_index();
        let rightmost_idx = polygon.rightmost_vertex_index();
        ConvexArcBisector {
            polygon: polygon,
            begin_index: rightmost_idx,
            num_edges: polygon.num_vertices(),
            mid_offset: sub_modulo(leftmost_idx, rightmost_idx, polygon.num_vertices()),
            mid_index: leftmost_idx,
        }
    }

    /// Returns the number of edges in the range.
    pub fn num_edges(&self) -> usize {
        self.num_edges
    }

    /// Returns the first vertex of the current range.
    pub fn begin_vertex(&self) -> Point2 {
        self.polygon[self.begin_index]
    }

    /// Returns the outgoing vector of the begin vertex. THis is the vector connecting `begin_vertex` to the next
    /// vertex.
    pub fn begin_vertex_outgoing(&self) -> Vec2 {
        self.polygon[succ_modulo(self.begin_index, self.polygon.num_vertices())] - self.polygon[self.begin_index]
    }

    /// Returns the last vertex of the current range.
    pub fn end_vertex(&self) -> Point2 {
        self.polygon[add_modulo(self.begin_index, self.num_edges, self.polygon.num_vertices())]
    }

    /// Returns the mid vertex of the current range.
    pub fn mid_vertex(&self) -> Point2 {
        self.polygon[self.mid_index]
    }

    /// Returns the outgoing vector of the mid vertex. This is the vector connecting `mid_vertex` to the next vertex.
    pub fn mid_vertex_outgoing(&self) -> Vec2 {
        self.polygon[succ_modulo(self.mid_index, self.polygon.num_vertices())] - self.polygon[self.mid_index]
    }

    /// Returns the outgoing vector of the mid vertex in the reverse direction. This is the vector connecting
    /// `mid_vertex` to the previous vertex.
    pub fn mid_vertex_outgoing_rev(&self) -> Vec2 {
        self.polygon[pred_modulo(self.mid_index, self.polygon.num_vertices())] - self.polygon[self.mid_index]
    }

    /// Limits the current range to the part on the clockwise side of the midpoint (so the new set of edges will consist
    /// of those currently between `begin_vertex` and `mid_vertex`).
    pub fn move_cw(&mut self) {
        self.num_edges = self.mid_offset;
    }

    /// Limits the current range to the part on the counterclockwise side of the midpoint (so the new set of edges will
    /// consist of those currently between `mid_vertex` and `end_vertex`).
    pub fn move_ccw(&mut self) {
        self.begin_index = self.mid_index;
        self.num_edges -= self.mid_offset;
        self.mid_offset = 0;
    }

    /// Selects the vertex at the middle of the current range as the `mid_vertex`.
    pub fn bisect(&mut self) {
        self.bisect_at(self.num_edges / 2);
    }

    /// Selects the vertex with the given offset from `begin_vertex` as the `mid_offset`.
    pub fn bisect_at(&mut self, offset: usize) {
        self.mid_offset = offset;
        self.mid_index = add_modulo(self.begin_index, self.mid_offset, self.polygon.num_vertices());
    }

    /// Selects the vertex with the greatest projection onto `support_direction` as the `mid_vertex`.
    ///
    /// It's a precondition of the `support_direction` that the projection onto that vector of the vertices in the
    /// current range form a unimodal sequence (that is, a sequence which starts out monotonically increasing to its
    /// maximum and then continuous decreasing monotonically).  
    pub fn bisect_at_support_vertex(&mut self, support_direction: Vec2) {
        let mut bisect_begin_index = self.begin_index;
        let mut bisect_num_vertices = self.num_edges + 1;

        while bisect_num_vertices > 1 {
            let bisect_mid_offset = bisect_num_vertices / 2;
            let bisect_mid_index = add_modulo(bisect_begin_index, bisect_mid_offset, self.polygon.num_vertices());

            let bisect_mid_prev_index = pred_modulo(bisect_mid_index, self.polygon.num_vertices());
            let incoming_dir = self.polygon[bisect_mid_index] - self.polygon[bisect_mid_prev_index];
            if Vec2::dot(incoming_dir, support_direction) > 0.0 {
                bisect_begin_index = bisect_mid_index;
                bisect_num_vertices -= bisect_mid_offset;
            } else {
                bisect_num_vertices = bisect_mid_offset;
            }
        }

        self.mid_offset = sub_modulo(bisect_begin_index, self.begin_index, self.polygon.num_vertices());
        self.mid_index = bisect_begin_index;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ConvexPolygon;
    use std::str::FromStr;

    #[test]
    fn test_new() {
        let polygon = ConvexPolygon::<Vec<Point2>>::from_str(
            "{{-2.16, 1.57}, {0.99, -1.48}, {8.10, 1.62}, {8.62, 4.95}, {4.45, 8.05}, {-0.19, 8.04}, {-2.99, 5.86}}"
        ).unwrap();

        let bisector = ConvexArcBisector::new(polygon.as_view(), 3, 4);
        std::assert_eq!(bisector.begin_vertex(), Point2::new(8.62, 4.95));
        std::assert_eq!(bisector.mid_vertex(), Point2::new(8.62, 4.95));
        std::assert_eq!(bisector.end_vertex(), Point2::new(-2.16, 1.57));
    }

    #[test]
    fn test_new_with_lower_then_upper_arc() {
        let polygon = ConvexPolygon::<Vec<Point2>>::from_str(
            "{{0.74, -1.69}, {4.54, -0.80}, {8.35, 3.04}, {4.43, 5.91}, {-1.25, 6.12}, {-3.42, 2.37}}"
        ).unwrap();

        let bisector = ConvexArcBisector::new_with_lower_then_upper_arc(polygon.as_view());
        std::assert_eq!(bisector.begin_vertex(), Point2::new(-3.42, 2.37));
        std::assert_eq!(bisector.mid_vertex(), Point2::new(8.35, 3.04));
        std::assert_eq!(bisector.end_vertex(), Point2::new(-3.42, 2.37));
    }

    #[test]
    fn test_new_with_upper_then_lower_arc() {
        let polygon = ConvexPolygon::<Vec<Point2>>::from_str("{
            {1.76, -0.25}, {5.86, -0.72}, {8.96, 0.18}, {10.74, 2.61}, {9.52, 5.09}, {6.75, 6.45}, {2.88, 6.45},
            {-0.58, 4.31}, {-1.28, 2.31}, {0.26, 0.48}
        }").unwrap();

        let bisector = ConvexArcBisector::new_with_upper_then_lower_arc(polygon.as_view());
        std::assert_eq!(bisector.begin_vertex(), Point2::new(10.74, 2.61));
        std::assert_eq!(bisector.mid_vertex(), Point2::new(-1.28, 2.31));
        std::assert_eq!(bisector.end_vertex(), Point2::new(10.74, 2.61));
    }

    #[test]
    fn test_traverse() {
        let mut vertices = Point2::vec_from_str("{
            {2.48, 5.67},  {3.34, 2.65},  {5.64, 0.87},  {9.36, 0.41}, {12.4, 0.79}, {14.78, 2.35}, {16.48, 4.03},
            {17.28, 5.91}, {18.04, 8.69}, {15.04, 9.35}, {8.5, 9.99}
        }").unwrap();

        for i in 0..vertices.len() {
            let polygon = ConvexPolygonView::new(&vertices);
            let mut bisector = ConvexArcBisector::new(polygon, i, 8);

            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 8);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(2.48, 5.67));
            std::assert_eq!(bisector.begin_vertex_outgoing(), Point2::new(3.34, 2.65) - Point2::new(2.48, 5.67));
            std::assert_eq!(bisector.end_vertex(), Point2::new(18.04, 8.69));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(14.78, 2.35) - Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(9.36, 0.41) - Point2::new(12.4, 0.79));

            bisector.move_cw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 4);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(2.48, 5.67));
            std::assert_eq!(bisector.begin_vertex_outgoing(), Point2::new(3.34, 2.65) - Point2::new(2.48, 5.67));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(9.36, 0.41) - Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(3.34, 2.65) - Point2::new(5.64, 0.87));

            bisector.move_ccw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 2);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.begin_vertex_outgoing(), Point2::new(9.36, 0.41) - Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            bisector.move_ccw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 1);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.begin_vertex_outgoing(), Point2::new(12.40, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            bisector.move_cw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 0);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.begin_vertex_outgoing(), Point2::new(12.40, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.end_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            vertices.rotate_right(1);
        }
    }

    #[test]
    fn test_bisect_at_support_vertex() {
        let mut vertices = Point2::vec_from_str(
            "{{9.57, 1.64}, {8.99, 4.74}, {5.81, 7.54}, {2.29, 8.20}, {-2.56, 7.03}, {-5.55, 3.24}, {-6.72, -0.30}}"
        ).unwrap();

        for i in 0..vertices.len() {
            let polygon = ConvexPolygonView::new(&vertices);
            
            let mut bisector = ConvexArcBisector::new(polygon, i, 6);

            // General case
            bisector.bisect_at_support_vertex(Vec2::new(-0.10, 2.23));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(2.29, 8.20));

            bisector.move_cw();
            std::assert_eq!(bisector.num_edges(), 3);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.57, 1.64));
            std::assert_eq!(bisector.end_vertex(), Point2::new(2.29, 8.20));

            // Bisect at range begin vertex
            bisector.bisect_at_support_vertex(Vec2::new(1.98, 0.21));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.57, 1.64));

            bisector.move_ccw();
            std::assert_eq!(bisector.num_edges(), 3);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.57, 1.64));
            std::assert_eq!(bisector.end_vertex(), Point2::new(2.29, 8.20));

            // Bisect at range end vertex
            bisector.bisect_at_support_vertex(Vec2::new(-0.10, 2.23));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(2.29, 8.20));

            bisector.move_cw();
            std::assert_eq!(bisector.num_edges(), 3);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.57, 1.64));
            std::assert_eq!(bisector.end_vertex(), Point2::new(2.29, 8.20));

            // Bisect at start vertex of edge perpendicular to support direction.
            bisector.bisect_at_support_vertex((Point2::new(5.81, 7.54) - Point2::new(8.99, 4.74)).right_perpendicular());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(8.99, 4.74));

            vertices.rotate_right(1); 
        }
    }

    #[test]
    fn test_bisect_at() {
        let mut vertices = Point2::vec_from_str(
            "{{-3.22, 3.49}, {-1.04, 1.57}, {6.03, 0.88}, {11.07, 6.28}, {2.61, 7.48}}"
        ).unwrap();

        for i in 0..vertices.len() {
            let polygon = ConvexPolygonView::new(&vertices);
            
            let mut bisector = ConvexArcBisector::new(polygon, i, 4);

            bisector.bisect_at(2);
            std::assert_eq!(bisector.mid_vertex(), Point2::new(6.03, 0.88));

            bisector.move_cw();
            std::assert_eq!(bisector.num_edges(), 2);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(-3.22, 3.49));
            std::assert_eq!(bisector.end_vertex(), Point2::new(6.03, 0.88));

            bisector.bisect_at(1);
            std::assert_eq!(bisector.mid_vertex(), Point2::new(-1.04, 1.57));

            bisector.move_ccw();
            std::assert_eq!(bisector.num_edges(), 1);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(-1.04, 1.57));
            std::assert_eq!(bisector.end_vertex(), Point2::new(6.03, 0.88));

            vertices.rotate_right(1);
        }
    }
}
