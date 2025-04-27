use crate::{Vec2, Point2};
use crate::convex::convex_polygon::ConvexPolygonView;
use crate::cyclic::{add_modulo, pred_modulo, succ_modulo};

/// A utility type which facilitates the implementation of binary search style algorithms on the boundary of a
/// convex polygon.
///
/// At any point, the `ConvexArcBisector` will hold a subrange of the edges of the polygon, and if the range contains at
/// least 2 edges, then a mid vertex will be selected. The 'move_left' and 'move_right functions can then be used to
/// repeatedly limit the current range to the part to the left/right of the mid vertex, until only a single edge is
/// left.
pub struct ConvexArcBisector<'a, const OUTGOING_IS_CCW: bool> {
    polygon: ConvexPolygonView<'a>,

    /// The index of the first vertex in the current range.
    begin_index: usize,

    /// The number of edges in the current range.
    num_edges: usize,

    /// The offset from 'begin_index' to the mid index.
    mid_offset: usize,

    /// The index of the mid vertex.
    mid_index: usize,

    // The mid vertex.
    mid_vertex: Point2,

    /// The outgoing direction of the mid vertex. If OUTGOING_IS_CCW is true, then this is the vector from the
    /// mid_vertex to the next vertex, otherwise it's the vector from mid_vertex to the previous vertex.  
    mid_vertex_outgoing: Vec2,
}

impl<'a, const OUTGOING_IS_CCW: bool> ConvexArcBisector<'a, OUTGOING_IS_CCW> {
    /// Constructs a new 'ConvexArcBisector'.
    pub fn new(polygon: ConvexPolygonView<'a>, begin_index: usize, num_edges: usize) -> Self {
        std::assert!(begin_index < polygon.num_vertices());
        std::assert!(num_edges >= 1);

        let mid_index = add_modulo(begin_index, num_edges / 2, polygon.num_vertices());
        ConvexArcBisector {
            polygon: polygon,
            begin_index: begin_index,
            num_edges: num_edges,
            mid_offset: num_edges / 2,
            mid_index: mid_index,
            mid_vertex: polygon[mid_index],
            mid_vertex_outgoing: Self::vertex_outgoing(polygon, mid_index),
        }
    }

    fn vertex_outgoing(polygon: ConvexPolygonView<'a>, index: usize) -> Vec2 {
        if OUTGOING_IS_CCW {
            polygon[succ_modulo(index, polygon.num_vertices())] - polygon[index]
        } else {
            polygon[pred_modulo(index, polygon.num_vertices())] - polygon[index]
        }
    }

    /// Returns the mid vertex of the current range.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn mid_vertex(&self) -> Point2 {
        std::debug_assert!(self.can_bisect());

        self.mid_vertex
    }

    /// Returns the outgoing vector of the mid vertex.
    ///
    /// If OUTGOING_IS_CCW is true, then this is the vector from the mid vertex to the next vertex, if OUTOING_IS_CCW is
    /// false, then it's the vector from the mid vertex to the previous vertex.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn mid_vertex_outgoing(&self) -> Vec2 {
        std::debug_assert!(self.can_bisect());

        self.mid_vertex_outgoing
    }

    /// Returns whether the range can still be bisected. This is the case when the current range contains at least 2
    /// edges.
    pub fn can_bisect(&self) -> bool {
        self.num_edges > 1
    }

    fn move_cw(&mut self) {
        std::debug_assert!(self.can_bisect());

        self.num_edges = self.mid_offset;
        self.bisect();
    }

    fn move_ccw(&mut self) {
        std::debug_assert!(self.can_bisect());
        
        self.begin_index = self.mid_index;
        self.num_edges -= self.mid_offset;
        self.bisect()
    }

    fn bisect(&mut self) {
        self.mid_offset = self.num_edges / 2;
        self.mid_index = add_modulo(self.begin_index, self.mid_offset, self.polygon.num_vertices());
        self.mid_vertex = self.polygon[self.mid_index];
        self.mid_vertex_outgoing = Self::vertex_outgoing(self.polygon, self.mid_index);
    }

    /// Returns the first vertex of the current range.
    pub fn v0(&self) -> Point2 {
        self.polygon[self.begin_index]
    }

    /// Returns the second vertex of the current range.
    pub fn v1(&self) -> Point2 {
        self.polygon[succ_modulo(self.begin_index, self.polygon.num_vertices())]
    }
}

/// A 'ConvexArcBisector' to be used to bisect (part of) the lower arc of a convex polygon.
pub type LowerConvexArcBisector<'a> = ConvexArcBisector<'a, true>;

/// A 'ConvexArcBisector' to be used to bisect (part of) the upper arc of a convex polygon.
pub type UpperConvexArcBisector<'a> = ConvexArcBisector<'a, false>;

impl<'a> LowerConvexArcBisector<'a> {
    /// Limits the current range to the part to the left of the midpoint.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn move_left(&mut self) {
        self.move_cw();
    }

    /// Limits the current range to the part to the right of the midpoint.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn move_right(&mut self) {
        self.move_ccw();
    }
}

impl<'a> UpperConvexArcBisector<'a> {
    /// Limits the current range to the part to the left of the midpoint.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn move_left(&mut self) {
        self.move_ccw();
    }

    /// Limits the current range to the part to the right of the midpoint.
    ///
    /// This function should only be called when 'self.can_bisect()' is true.
    pub fn move_right(&mut self) {
        self.move_cw();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_lower_convex_arc_bisector() {
        let mut vertices = Point2::vec_from_str("{
            {2.48, 5.67},  {3.34, 2.65},  {5.64, 0.87},  {9.36, 0.41}, {12.4, 0.79}, {14.78, 2.35}, {16.48, 4.03},
            {17.28, 5.91}, {18.04, 8.69}, {15.04, 9.35}, {8.5, 9.99}
        }").unwrap();

        for i in 0..vertices.len() {
            let polygon = ConvexPolygonView::new(&vertices);
            let mut bisector = LowerConvexArcBisector::new(polygon, i, 8);

            std::assert!(bisector.can_bisect());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(14.78, 2.35) - Point2::new(12.4, 0.79));

            bisector.move_left();
            std::assert!(bisector.can_bisect());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(9.36, 0.41) - Point2::new(5.64, 0.87));

            bisector.move_right();
            std::assert!(bisector.can_bisect());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));

            bisector.move_right();
            std::assert!(!bisector.can_bisect());
            std::assert_eq!(bisector.v0(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.v1(), Point2::new(12.4, 0.79));

            vertices.rotate_right(1);
        }
    }

    #[test]
    fn test_upper_convex_arc_bisector() {
        let mut vertices = Point2::vec_from_str("{
            {12.56, 3.23}, {10.56, 7.47}, {5.26, 9.57}, {0.3, 9.59}, {-4.22, 7.31}, {-6.62, 2.97}, {4.38, -6.17}
        }").unwrap();

        for i in 0..vertices.len() {
            let polygon = ConvexPolygonView::new(&vertices);
            let mut bisector = UpperConvexArcBisector::new(polygon, i, 5);

            std::assert!(bisector.can_bisect());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(5.26, 9.57));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(10.56, 7.47) - Point2::new(5.26, 9.57));

            bisector.move_left();
            std::assert!(bisector.can_bisect());
            std::assert_eq!(bisector.mid_vertex(), Point2::new(0.3, 9.59));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(5.26, 9.57) - Point2::new(0.3, 9.59));

            bisector.move_right();
            std::assert!(!bisector.can_bisect());
            std::assert_eq!(bisector.v0(), Point2::new(5.26, 9.57));
            std::assert_eq!(bisector.v1(), Point2::new(0.3, 9.59));

            vertices.rotate_right(1);
        }
    }
}
