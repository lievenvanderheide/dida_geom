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

    /// Returns the number of edges in the range.
    pub fn num_edges(&self) -> usize {
        self.num_edges
    }

    /// Returns the first vertex of the current range.
    pub fn begin_vertex(&self) -> Point2 {
        self.polygon[self.begin_index]
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
        self.mid_offset = self.num_edges / 2;
        self.mid_index = add_modulo(self.begin_index, self.mid_offset, self.polygon.num_vertices());
    }
}

#[cfg(test)]
mod tests {
    use super::*;

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
            std::assert_eq!(bisector.end_vertex(), Point2::new(18.04, 8.69));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(14.78, 2.35) - Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(9.36, 0.41) - Point2::new(12.4, 0.79));

            bisector.move_cw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 4);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(2.48, 5.67));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(9.36, 0.41) - Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(3.34, 2.65) - Point2::new(5.64, 0.87));

            bisector.move_ccw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 2);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(5.64, 0.87));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            bisector.move_ccw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 1);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.end_vertex(), Point2::new(12.4, 0.79));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            bisector.move_cw();
            bisector.bisect();
            std::assert_eq!(bisector.num_edges(), 0);
            std::assert_eq!(bisector.begin_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.end_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex(), Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing(), Point2::new(12.4, 0.79) - Point2::new(9.36, 0.41));
            std::assert_eq!(bisector.mid_vertex_outgoing_rev(), Point2::new(5.64, 0.87) - Point2::new(9.36, 0.41));

            vertices.rotate_right(1);
        }
    }
}
